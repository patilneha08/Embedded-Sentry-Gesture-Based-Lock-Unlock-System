# Embedded Sentry – Gesture-Based Lock/Unlock System

**Group 43 – Embedded Challenge, Fall 2024**  
Neha Patil, Tanvi Abhijit Takavane, Ruchi Jha

## Project Overview

This project implements an embedded security mechanism using gesture recognition as a locking and unlocking feature. The system is built on the STM32F429I-DISC1 development board and uses onboard IMU data (accelerometer/gyroscope) to capture and authenticate hand movement patterns. The project was developed as part of the Embedded Systems course term project.

## Objective

- Utilize data from the onboard accelerometer and/or gyroscope to record a hand gesture sequence for unlocking a protected resource.
- Store the recorded sequence on the microcontroller via a "Record Key" feature.
- Authenticate by comparing a new gesture input with the saved gesture pattern within a defined tolerance.
- Indicate success or failure through visual feedback (color-coded screen/LED).

## Features

- **Platform**: STM32F429I-DISC1 development board
- **Input**: Onboard IMU (accelerometer/gyro)
- **User Interface**: Onboard blue push-button
- **Visual Feedback**: Color-coded TFT screen display
  - Red Screen: No key recorded or unlock failed
  - Blue Screen: Recording or entering key
  - Green Screen: Unlock successful
- **Development Environment**: PlatformIO with STM32 HAL drivers

## System Workflow

1. **Startup**: Upload the code to the board. The white screen indicates system ready.
2. **No Key Recorded**: First button press prompts a red screen indicating no stored gesture.
3. **Record Gesture**: Second button press triggers "Recording" (blue screen). Gesture is captured and stored.
4. **Enter Gesture**: Third button press begins authentication. Matching gesture leads to "Unlock Success" (green screen).
5. **Incorrect Gesture**: Further input with a non-matching gesture displays "Unlock Failed" (red screen).

## Technical Highlights

- **Gesture Comparison**: Captures motion vectors and compares them using signal similarity metrics (e.g., Euclidean or DTW).
- **Tolerance Window**: Ensures flexibility in movement while maintaining robustness in unlocking.
- **Memory Management**: Stored sequence is written to RAM or onboard flash for persistence.
- **Power & Portability**: Low-power design suitable for embedded, battery-operated environments.

## Usage Instructions

1. Connect the STM32 board via USB.
2. Use PlatformIO to upload the code.
3. Follow on-screen prompts using the blue user button:
   - First press: Check key status.
   - Second press: Record gesture.
   - Third press: Attempt to unlock.
4. Observe visual feedback to determine success/failure.

## Repository Contents

- `main.cpp` (or `main.c`): Contains main control logic and gesture handling
- `imu_driver.c/h`: Interfaces with the accelerometer/gyro sensor
- `display.c/h`: Manages on-screen feedback using the LCD controller
- `utils.c/h`: Includes gesture comparison algorithms and tolerance settings
- `platformio.ini`: Project configuration for PlatformIO

## Future Work

- Add buzzer feedback or haptic motor for silent unlock indication
- Implement EEPROM saving for gesture persistence across resets
- Add gesture complexity analysis for multi-step patterns

## License

This project is intended for academic use as part of NYU’s Embedded Systems coursework. All code and design contributions were made by the listed team members.
