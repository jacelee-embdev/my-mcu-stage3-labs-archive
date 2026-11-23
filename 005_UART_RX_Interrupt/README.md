[简体中文](README_zh.md) | **English**

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32H757XIH6 UART Receive Interrupt Lab

## Overview

This lab demonstrates byte-by-byte interrupt reception on USART1 on the STM32H757XIH6 Cortex-M7 (CM7). The receive callback prints each byte's decimal value, while the main loop sends a count and toggles the green LED about once per second. CM4 synchronizes startup through HSEM 0, initializes the HAL, and enters an empty loop.

## Serial reception and transmission

| Signal | Pin | Configuration |
|---|---|---|
| USART1 TX | PA9 | Connect to RX on the USB-to-serial adapter |
| USART1 RX | PA10 | Connect to TX on the USB-to-serial adapter |
| LED0 (green) | PA3 | Push-pull output, initialized high |

After initializing USART1, CM7 calls `HAL_UART_Receive_IT(&huart1, &rx_data, 1)` to receive one byte. `USART1_IRQHandler()` calls `HAL_UART_IRQHandler()`, which invokes `HAL_UART_RxCpltCallback()` when reception completes. The callback checks that the source is USART1, prints `recv:<decimal value>`, and starts the next one-byte receive request. [UART setup](CM7/Core/Src/usart.c) · [Interrupt handler](CM7/Core/Src/stm32h7xx_it.c) · [Application logic](CM7/Core/Src/main.c)

The main loop increments an eight-bit counter, sends its decimal value with a blocking `HAL_UART_Transmit()` call, toggles the green LED, and waits 1000 ms. The count starts at 1 and wraps to 0 after 255. The callback's `printf` uses the same blocking transmit interface, so the reply can be lost if USART1 is already sending the count. The callback starts the next receive request after printing, which makes individual bytes the simplest way to observe the behavior.

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
2. Use a 3.3 V USB-to-serial adapter, cross-connect TX and RX as shown in the table, and connect the grounds. Set the terminal to 115200 baud, 8 data bits, no parity, one stop bit, and no hardware flow control.
3. Reset the board and let it run. Check that the green LED toggles about once a second and that the terminal displays the count. Send the single ASCII character `A` and look for `recv:65`. Disable automatic line endings in the terminal to send just one byte.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in the project root for the full license text. Third-party components are subject to their respective licenses.
