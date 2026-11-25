[简体中文](README_zh.md) | **English**

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32H757XIH6 ADC1 Voltage Measurement Lab

## Overview

The STM32H757XIH6 Cortex-M7 (CM7) uses ADC1 to measure the analog input on PA1, converts the 16-bit result to a voltage, and prints it over USART1. During startup, CM4 waits for CM7 to wake it using hardware semaphore 0 (HSEM 0). CM4 then initializes the HAL and enters an empty loop.

## Pin configuration

| Signal | Pin | Configuration |
|---|---|---|
| ADC1_INP17 | PA1 | ADC1 channel 17, single-ended analog input |
| KEY0 | PA8 | Pull-down input, high when pressed |
| LED1 (blue) | PB1 | Push-pull output, initialized high, toggles when KEY0 reads high |
| USART1 | PA9 (TX), PA10 (RX) | 115200 baud, 8 data bits, no parity, one stop bit, no hardware flow control |

See the [CubeMX project file](STM32H757XIH6.ioc) and CM7 [`main.h`](CM7/Core/Inc/main.h) for the pin assignments. The main loop checks KEY0 before reading the ADC value. If KEY0 reads high, the loop waits 25 ms and toggles the blue LED.

## ADC conversion and serial output

The CM7 [`adc.c`](CM7/Core/Src/adc.c) configures ADC1 for one 16-bit conversion on a single channel, triggered by software. The ADC samples for 1.5 ADC clock cycles and stores the result in its data register.

[`main.c`](CM7/Core/Src/main.c) calls `HAL_ADC_Start()` once after initializing the peripherals, then reads the data register in the main loop with `HAL_ADC_GetValue()`. The firmware calculates `raw value × 3.3 / 65536`, rounds to the nearest millivolt, and prints a line such as `VOLT: 1.650 V` using `printf` redirected to USART1. It waits two seconds after each line.

The current firmware triggers one conversion at startup and reports that same result on each pass through the loop. Reset the board after changing the input voltage on PA1 to take a fresh measurement. The calculation assumes a 3.3 V reference, so the reading depends on the board's actual ADC reference voltage.

## Build the firmware

Add GNU Make and the Arm GNU Toolchain to `PATH`. Run this command from the lab directory to build firmware for both cores.

```sh
make -C Makefile
```

For a single core, use `Makefile/CM4` or `Makefile/CM7` in the command. Each build produces `.elf`, `.hex`, and `.bin` files. If you flash a `.bin` file, enter its Flash start address from the table below.

| Core | Output basename | Flash start address |
|---|---|---|
| CM7 | `Makefile/CM7/build/STM32H757XIH6_CM7` | `0x08000000` |
| CM4 | `Makefile/CM4/build/STM32H757XIH6_CM4` | `0x08100000` |

VS Code provides `Build All`, `Build CM4`, and `Build CM7` tasks. To debug CM7 with Cortex-Debug, point `serverpath` in [`launch.json`](.vscode/launch.json) to your local J-Link GDB Server and make sure `arm-none-eabi-gdb` is available.

## Flash and observe the output

1. Confirm that the board supplies a 25 MHz external clock for HSE bypass and that both cores are enabled to boot. Use SWD to program each image into its Flash region. If you program them separately, preserve the first region when programming the second.
2. Connect PA9 to RX on a 3.3 V USB-to-serial adapter and connect their grounds. Configure the terminal using the settings in the table above.
3. Connect a known DC voltage within the board's analog input range to PA1 and connect the source ground to the board ground. Let the input settle, then reset the board.

4. Check the `VOLT:` reading in the terminal about every two seconds. To check a second known voltage, adjust the input, reset the board, and compare the readings.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in the project root for the full license text. Third-party components are subject to their respective licenses.
