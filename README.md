# ECU Project – Embedded C

This repository contains Embedded C code for 3 ECU nodes that communicate over CAN.

## ECU1
- Reads RPM from ADC
- Sends CAN message with RPM and indicator flags

## ECU2
- Reads speed from ADC
- Reads gear from keypad
- Sends speed + gear via CAN

## ECU3
- Receives RPM, speed, gear
- Updates LCD display
- Handles indicator blinking logic
