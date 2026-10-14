# S32K144-NvM-Flexible-RAM-Pointer-Demo


## Project Overview

This project demonstrates the concept of a **Flexible RAM Pointer** for memory block operations. The application passes a RAM buffer to custom NvM-style read and write functions, making it possible to modify a value, save it to a simulated storage block, and restore the last saved value.

### Key Features

- Potentiometer input using the ADC Peripheral Abstraction Layer (ADC PAL).
- RAM value updates based on the potentiometer position.
- Save operation using the SW1 input.
- Discard operation using the SW2 input.
- Simulated NvM block read and write functions.
- RAM block status tracking using `NvM_SetRamBlockStatus()`.
- UART logging through LPUART and PuTTY.
- Button debouncing and potentiometer change filtering.
  
### Architecture 
<img width="757" height="850" alt="image" src="https://github.com/user-attachments/assets/fb15da3a-234b-4f47-bfb4-db31f6acc4b5" />

## Hardware Requirements

- NXP S32K144 Evaluation Board (EVB)
- Onboard or externally connected potentiometer supported by the configured ADC input
- USB connection for programming and serial communication
- PC with PuTTY or another serial terminal

## Software Requirements

- NXP S32 Design Studio
- S32 SDK for S32K1xx
- C compiler and debugger supported by S32 Design Studio
- PuTTY or an equivalent UART terminal application

## Project Structure

```text
project_1/
├── board/
│   ├── clock_config.c
│   ├── clock_config.h
│   ├── pin_mux.c
│   ├── pin_mux.h
│   ├── peripherals_adc_pal_1.c
│   ├── peripherals_adc_pal_1.h
│   ├── peripherals_lpuart_1.c
│   └── sdk_project_config.h
├── src/
│   ├── main.c
│   ├── nvm.c
│   └── nvm.h
└── README.md
```

*The project also contains S32 Design Studio settings and build output directories.*

## How It Works

### 1. Initialization

The application initializes the clock, pin multiplexer, LPUART, and ADC PAL using the generated SDK configuration.

### 2. Initial NvM Read

The application reads the configured NvM block into a RAM buffer. The first byte represents the value used by the potentiometer demo.

### 3. Potentiometer Monitoring

The ADC conversion result is scaled to a percentage from 0 to 100. When the value changes beyond the configured threshold, the application updates the RAM buffer and prints the current state.

### 4. Save Operation — SW1

When SW1 is pressed:

- The current RAM buffer is passed to `NvM_WriteBlock()`.
- The saved value is updated.
- The RAM block status is marked unchanged.
- The application reports the save result over UART.

### 5. Discard Operation — SW2

When SW2 is pressed:

- The application reads the selected block using `NvM_ReadBlock()`.
- The RAM buffer is restored from the stored data.
- The application reports the restored value and save status.

### 6. UART Monitoring

The application prints the potentiometer value, RAM value, and whether the current value is saved or unsaved.

## UART Output
S32K144 NvM Potentiometer Demo
Potentiometer: 35
RAM value: 35
Status: UNSAVED

SW1 pressed
Saved value: 35
Status: SAVED

Potentiometer: 72
RAM value: 72
Status: UNSAVED

SW2 pressed
Unsaved changes discarded
RAM value restored: 35
Status: SAVED


## NvM Implementation

The project provides three custom NvM-style functions:

| Function | Purpose |
|---|---|
| `NvM_ReadBlock()` | Copies data from a simulated storage block to the supplied RAM buffer |
| `NvM_WriteBlock()` | Copies data from the supplied RAM buffer to a simulated storage block |
| `NvM_SetRamBlockStatus()` | Tracks whether the RAM block has been marked as changed |

The selected block is identified by a block ID. The RAM buffer is supplied by the application rather than being fixed inside the read/write API.

## Configuration Notes

- The ADC PAL configuration currently uses **8-bit ADC resolution**.
- The application scales ADC readings into a percentage range of 0–100.
- The application uses PTC12 and PTC13 as its configured button input pin numbers; verify their mapping against your board and generated pin configuration.
- The selected NvM block ID is `1`.
- The configured RAM block size is 4 bytes.

## Important: Storage Limitations

**The current implementation uses a volatile RAM array as simulated NvM storage.**

It demonstrates memory block read/write behavior, but it is not a complete non-volatile memory implementation. The stored value is not guaranteed to survive a reset or power loss.

To retain values across resets, the storage layer must be implemented using an appropriate Flash-based persistence mechanism, such as EEPROM emulation, with proper erase handling, programming alignment, and reserved memory layout.

## How to Build and Run

1. Open S32 Design Studio.
2. Import the project into the workspace.
3. Check the generated clock, pin, ADC, and LPUART configurations.
4. Build the project.
5. Flash the application to the S32K144 EVB.
6. Connect to the board's configured UART port using PuTTY.
7. Rotate the potentiometer and observe the RAM value.
8. Press SW1 to save the current value.
9. Change the potentiometer position and press SW2 to discard unsaved changes.
10. Observe the restored value and status messages.

## Learning Outcomes

- Embedded C development on the S32K144 MCU.
- ADC configuration and analog input processing.
- GPIO input handling and button debouncing.
- UART-based debugging.
- RAM buffer management and pointer-based APIs.
- NvM-style block read/write operations.
- Difference between simulated storage and persistent non-volatile memory.

## Author
1.Suraj Sharma 
2.Mani Ram Balan

Embedded Systems | Automotive Embedded Software | NXP S32K144 | S32 SDK


