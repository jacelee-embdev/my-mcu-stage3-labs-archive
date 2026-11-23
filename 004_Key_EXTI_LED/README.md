[简体中文](README_zh.md) | **English**

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32H757XIH6 External Interrupt Lab

## Overview

This lab demonstrates a GPIO external interrupt on the STM32H757XIH6 Cortex-M7 (CM7). The main loop toggles the green LED periodically, and the KEY0 interrupt callback toggles the blue LED. Both paths log over USART1. CM4 synchronizes startup through HSEM 0, initializes the HAL, and enters an empty loop.

## Pin assignments and interrupt flow

| Signal | Pin | Configuration |
|---|---|---|
| KEY0 | PA8 | Pull-down input, rising-edge trigger on EXTI8 |
| LED0 (green) | PA3 | Push-pull output, initialized high |
| LED1 (blue) | PB1 | Push-pull output, initialized high |
| USART1 | PA9 (TX), PA10 (RX) | Asynchronous serial |

CM7 configures PA8 and enables `EXTI9_5_IRQn`. When a press takes PA8 from low to high, `EXTI9_5_IRQHandler()` calls `HAL_GPIO_EXTI_IRQHandler(KEY0_Pin)`, which invokes `HAL_GPIO_EXTI_Callback()`. [GPIO setup](CM7/Core/Src/gpio.c) · [Interrupt handler](CM7/Core/Src/stm32h7xx_it.c) · [Callback](CM7/Core/Src/main.c)

The callback checks that KEY0 triggered it and that PA8 is still high. It then toggles the blue LED and prints `[CM7] ISR Running...`. Once the input stays high, another trigger requires a new rising edge. The callback checks the level once, so switch bounce can produce multiple toggles.

The main loop toggles the green LED, prints `[CM7] System Running...`, and waits 1000 ms. Both paths use blocking UART transmission. If the interrupt occurs while the main loop is transmitting, its log message can be lost because USART1 is busy.

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
3. Reset the board and let the firmware run. The green LED toggles about once a second and the terminal prints `[CM7] System Running...`. Press and release KEY0 and check that the blue LED toggles on the press. Look for `[CM7] ISR Running...`. The interrupt can run while the main loop is waiting.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in the project root for the full license text. Third-party components are subject to their respective licenses.
