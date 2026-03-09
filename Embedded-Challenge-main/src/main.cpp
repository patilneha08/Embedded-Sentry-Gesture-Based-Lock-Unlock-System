/* 
* ECE-GY 6483 : Embedded Challenge for Fall 2024.
* GROUP 43
* Participants: Neha Patil               - np2998
              : Tanvi Abhijit Takavane   - tt2884
              : Ruchi Jha                - rj2807

*/

// Standard Libraries
#include <cstdint>
#include <cstring>
#include <limits>

// Mbed and STM32 Drivers
#include "mbed.h"
#include "drivers/LCD_DISCO_F429ZI.h"
#include "stm32f4xx.h"

// Local Header Files
#include "macros.hpp"
#include "dtw_distance.hpp"
#include "utilities.hpp"

// Gyroscope Register Definitions
#define L3GD20_CTRL_REG1        0x20
#define L3GD20_CTRL_REG4        0x23
#define L3GD20_OUT_X_L          0x28
#define L3GD20_OUT_X_H          0x29
#define L3GD20_OUT_Y_L          0x2A
#define L3GD20_OUT_Y_H          0x2B
#define L3GD20_OUT_Z_L          0x2C
#define L3GD20_OUT_Z_H          0x2D

// Gyroscope Scaling Factor
#define GYRO_SCALE_FACTOR       (17.5f * 0.017453292519943295769236907684886f / 1000.0f)

// Recording Duration (milliseconds)
#define RECORDING_DURATION      2000

// Threshold for Gesture Comparison
#define GESTURE_TOLERANCE       90 

// Long Press Duration for Recording (milliseconds)
#define LONG_PRESS_DURATION     2000

// Sampling Rate and Total Number of Samples
#define SAMPLES_PER_SECOND 50
#define NUM_SAMPLES (RECORDING_DURATION / 1000 * SAMPLES_PER_SECOND)

// --- Register Addresses and Configuration Settings ---
#define CTRL_REG1 0x20               // Control Register 1 Address
#define CTRL_REG1_CONFIG 0b01'10'1'1'1'1 //0110 1111  // Config: ODR=100Hz, Enable X/Y/Z Axes, Power On
#define CTRL_REG4 0x23               // Control Register 4 Address
#define CTRL_REG4_CONFIG 0b0'0'01'0'00'0  // Config: High Resolution, 2000dps Sensitivity

// SPI Transfer Flag
#define SPI_FLAG 1

// Gyroscope Data Registers
#define OUT_X_L 0x28

// Conversion Factor from Raw Sensor Data to Angular Velocity (in radians)
#define DEG_TO_RAD (17.5f * 0.0174532925199432957692236907684886f / 1000.0f)

// Synchronization Flag for SPI Transfers
EventFlags flags;

// --- SPI Transfer Callback Function ---
// Triggered when an SPI transfer completes
void spi_cb(int event) {
    flags.set(SPI_FLAG);  // Set the SPI_FLAG to indicate transfer completion
}

using namespace std::chrono;
using namespace std;
Timer t;

// LCD Display
LCD_DISCO_F429ZI lcd;

// Button Input for Gesture Recording and Entry
DigitalIn button(PA_0);

// LED Output for Indicating Success
DigitalOut led(LED1);
I2C_HandleTypeDef hi2c;

// Arrays to Store Gesture Data
float key_vals[MAX_ARRAY_SIZE][3];
float gyro_vals[MAX_ARRAY_SIZE][3];

bool key_recorded = false;

// Millisecond Delay Function
void delay_ms(uint32_t ms)
{
    HAL_Delay(ms);
}

// Function to Retrieve Gyroscope Data
void read_gyro(int row, uint8_t write_buf[32], uint8_t read_buf[32], SPI *spi, EventFlags *flags, float arr[MAX_ARRAY_SIZE][3]) {
    uint16_t raw_gx, raw_gy, raw_gz;
    float gx, gy, gz;
    // Prepare to Read Gyroscope Output from OUT_X_L Register
    write_buf[0] = OUT_X_L | 0x80 | 0x40; // Read Mode + Auto-increment

    // Execute SPI Transfer to Read 6 Bytes (X, Y, Z Axes Data)
    spi->transfer(write_buf, 7, read_buf, 7, spi_cb);
    flags->wait_all(SPI_FLAG);  // Wait for Transfer to Complete

    // --- Process and Convert Raw Data ---
    raw_gx = (((uint16_t)read_buf[2]) << 8) | read_buf[1];
    raw_gy = (((uint16_t)read_buf[4]) << 8) | read_buf[3];
    raw_gz = (((uint16_t)read_buf[6]) << 8) | read_buf[5];

    gx = raw_gx * DEG_TO_RAD;
    gy = raw_gy * DEG_TO_RAD;
    gz = raw_gz * DEG_TO_RAD;

    arr[row][0] = gx;
    arr[row][1] = gy;
    arr[row][2] = gz;
    delay_ms(1000 / SAMPLES_PER_SECOND);
}

// Function to Show Unlock Message on Display
void display_unlock_message(bool success) {
    lcd.SetFont(&Font24);  // Set Font Size

    // Display Appropriate Message Based on Success or Failure
    if (success) {
        lcd.Clear(LCD_COLOR_GREEN); // Green Background for Success
        lcd.DisplayStringAt(0, LINE(7), (uint8_t *)"   UNLOCK SUCCESS", CENTER_MODE);
    } else {
        lcd.Clear(LCD_COLOR_RED); // Red Background for Failure
        lcd.DisplayStringAt(0, LINE(7), (uint8_t *)"UNLOCK FAILED", CENTER_MODE);
    }

    delay_ms(2000);  // Wait for 2 Seconds to Display the Message

    lcd.SetFont(NULL);  // Reset Font to Default
}

// Main Program Execution
int main() {
    // --- SPI Initialization ---
    SPI spi(PF_9, PF_8, PF_7, PC_1, use_gpio_ssel);

    uint8_t write_buf[32], read_buf[32];  // Buffers for SPI Data

    // Configure SPI Interface: 
    // - 8-bit Data Size
    // - Mode 3 (CPOL=1, CPHA=1): Clock High in Idle, Data Sampled on Falling Edge
    spi.format(8, 3);

    // Set SPI Frequency to 1 MHz
    spi.frequency(1'000'000);

    // --- Gyroscope Initialization ---
    // Configure Control Register 1 (CTRL_REG1)
    write_buf[0] = CTRL_REG1;
    write_buf[1] = CTRL_REG1_CONFIG;
    spi.transfer(write_buf, 2, read_buf, 2, spi_cb);
    flags.wait_all(SPI_FLAG);  // Wait for Transfer Completion

    // Configure Control Register 4 (CTRL_REG4)
    write_buf[0] = CTRL_REG4;
    write_buf[1] = CTRL_REG4_CONFIG;
    spi.transfer(write_buf, 2, read_buf, 2, spi_cb);
    flags.wait_all(SPI_FLAG);  // Wait for Transfer Completion

    // Initialize LCD Display
    lcd.DisplayOn();
    lcd.Clear(LCD_COLOR_WHITE);

    // Initialize Arrays for Key and Entered Gestures
    memset(key_vals, 0, sizeof key_vals);
    memset(gyro_vals, 0, sizeof gyro_vals);
    
    // Main Processing Loop
    while (1) {
        float dtw_distance = std::numeric_limits<float>::infinity();

        if (button.read() != 1) {
            continue;
        }
        
        uint32_t press_time = HAL_GetTick();  // Capture Button Press Time

        while (button.read() == 1) {}  // Wait for Button Release

        // Check if Press Duration is Long or Short
        if ((HAL_GetTick() - press_time) >= LONG_PRESS_DURATION) {
            
            // Long Press: Record Key Gesture
            memset(key_vals, 0, sizeof key_vals);
            lcd.Clear(LCD_COLOR_BLUE);
            lcd.DisplayStringAt(0, LINE(7), (uint8_t *)"RECORDING !!", CENTER_MODE);
            delay_ms(1000);  // Allow User to Prepare

            for (int i = 0; i < MAX_ARRAY_SIZE; i++) {
                read_gyro(i, write_buf, read_buf, &spi, &flags, key_vals);
                delay_ms(1000 / SAMPLES_PER_SECOND);
            }

            standard_scaler(key_vals);  // Normalize Recorded Data

            key_recorded = true;

            lcd.Clear(LCD_COLOR_WHITE);
            lcd.DisplayStringAt(0, LINE(7), (uint8_t *)"RECORDING COMPLETE", CENTER_MODE);
            delay_ms(1000);
        } else {
            // Short Press: Enter Gesture
            if (!key_recorded) {
                lcd.Clear(LCD_COLOR_RED);
                lcd.DisplayStringAt(0, LINE(7), (uint8_t *)"NO KEY RECORDED", CENTER_MODE);
                delay_ms(1000);
                continue;
            }

            lcd.Clear(LCD_COLOR_BLUE);
            lcd.DisplayStringAt(0, LINE(7), (uint8_t *)"ENTER KEY", CENTER_MODE);
            delay_ms(1000);  // Delay Before Recording Entered Gesture

            memset(gyro_vals, 0, sizeof gyro_vals);

            for (int i = 0; i < MAX_ARRAY_SIZE; i++) {
                read_gyro(i, write_buf, read_buf, &spi, &flags, gyro_vals);
                delay_ms(1000 / SAMPLES_PER_SECOND);
            }

            standard_scaler(gyro_vals);  // Normalize the Entered Gesture

            dtw_distance = dtw_distance_only(key_vals, MAX_ARRAY_SIZE, 3, gyro_vals, MAX_ARRAY_SIZE, 3, 2);
            printf("%f\n\n", dtw_distance);
            printf("**********************");

            // Compare DTW Distance to Tolerance and Display Result
            if (dtw_distance <= GESTURE_TOLERANCE) {
                display_unlock_message(true);  // Unlock Successful
            } else {
                display_unlock_message(false);  // Unlock Failed
            }
        }
    }
}

