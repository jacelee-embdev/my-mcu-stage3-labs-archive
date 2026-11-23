[简体中文](README_zh.md) | **English**

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32H757XIH6 Button-Controlled LED Lab

## Overview

The STM32H757XIH6 Cortex-M7 (CM7) reads a button, drives two LEDs, and logs over USART1. During startup, CM4 waits for CM7 to wake it using hardware semaphore 0 (HSEM 0). CM4 then initializes the HAL and enters an empty loop.

## GPIO configuration and button behavior

| Signal | Pin | Configuration |
|---|---|---|
| KEY0 | PA8 | Pull-down input, high when pressed |
| LED0 (green) | PA3 | Push-pull output, initialized high |
| LED1 (blue) | PB1 | Push-pull output, initialized high |
| USART1 | PA9 (TX), PA10 (RX) | Asynchronous serial |

See the [CubeMX project file](STM32H757XIH6.ioc) and CM7 [`main.h`](CM7/Core/Inc/main.h) for the pin assignments. Whether a high output turns an LED on or off depends on the board wiring.

CM7 polls KEY0 in the `USER CODE BEGIN 3` section of [`main.c`](CM7/Core/Src/main.c). Each high reading triggers a 25 ms delay, followed by both LED toggles and a `[CM7] System Running...` message. Holding the button down repeats the sequence. If you release it during a delay, that iteration still toggles the LEDs and prints the message.

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
2. Connect PA9 to RX on a 3.3 V USB-to-serial adapter and connect their grounds. Set the terminal to 115200 baud, 8 data bits, no parity, one stop bit, and no hardware flow control.
3. Reset the board and let the firmware run. Hold KEY0 to see both LEDs toggle repeatedly and the terminal print the log. Release the button to stop the output and leave the LEDs in their final state.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in the project root for the full license text. Third-party components are subject to their respective licenses.
