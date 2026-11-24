[简体中文](README_zh.md) | **English**

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32H757XIH6 USART DMA Idle Reception and D-Cache Lab

## Overview

CM7 uses USART1 and DMA to receive variable-length serial input. An IDLE event or a full receive buffer ends the current transfer. The firmware uses incoming commands to control the blue LED, and its main loop prints a status line about once per second. CM4 takes part in HSEM 0 startup synchronization, initializes DMA, and enters an empty loop.

## Reception and cache maintenance

1. After initializing GPIO, DMA, and USART1, CM7 calls `HAL_UARTEx_ReceiveToIdle_DMA()`. It receives up to 100 bytes using DMA1 Stream 0 in normal mode and disables the DMA half-transfer interrupt.
2. The HAL invokes `HAL_UARTEx_RxEventCallback()` on an IDLE event or after receiving 100 bytes. The callback uses the reported length to copy data into a local array, adds a string terminator, and starts the next DMA receive.
3. The application searches the received data for the case-sensitive strings `LED ON` and `LED OFF`. The blue LED, LED1 on PB1, turns on when the output goes low and off when it goes high. Status logs use blocking USART1 transmission.

The receive buffer occupies 128 bytes and is aligned to 32 bytes. The CM7 Flash linker script places the `.dma_buffer` section in D1 AXI SRAM, starting at `0x24000000`. When D-Cache is enabled, the firmware calls `SCB_InvalidateDCache_by_Addr()` before starting DMA and before reading the received data so that the CPU reads the bytes DMA wrote to SRAM. The firmware startup leaves D-Cache in its existing state.

[Receive callback and command handling](CM7/Core/Src/main.c) · [USART1 and DMA setup](CM7/Core/Src/usart.c) · [CM7 linker script](Makefile/CM7/stm32h757xx_flash_CM7.ld)

## Build the firmware

Add GNU Make and the Arm GNU Toolchain to `PATH`. Run this command from the lab directory to build firmware for both cores.

```sh
make -C Makefile
```

To build a single core, run `make -C Makefile/CM4` or `make -C Makefile/CM7`. Each build produces `.elf`, `.hex`, and `.bin` files.

| Core | Output basename | Flash start address |
|---|---|---|
| CM7 | `Makefile/CM7/build/STM32H757XIH6_CM7` | `0x08000000` |
| CM4 | `Makefile/CM4/build/STM32H757XIH6_CM4` | `0x08100000` |

VS Code provides `Build All`, `Build CM4`, and `Build CM7` tasks. To debug CM7 with J-Link and Cortex-Debug, set the local J-Link GDB Server path in `launch.json` and make sure `arm-none-eabi-gdb` is available. [Debug configuration](.vscode/launch.json)

## Flash and test

1. Confirm that the board supplies a 25 MHz external clock for HSE bypass and that both cores are enabled to boot. Program both images into their respective Flash regions over SWD.
2. Use a 3.3 V USB-to-serial adapter. Connect PA9 (USART1 TX) to the adapter's RX, PA10 (USART1 RX) to its TX, and connect their grounds. Set the terminal to 115200 baud, 8 data bits, no parity, one stop bit, and no hardware flow control.
3. Open the terminal, reset the board, and look for `[CM7] System Running...`. Send `LED ON` and `LED OFF` separately, each as a single string followed by an idle interval. The blue LED should turn on and off, respectively.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in the project root for the full license text. Third-party components are subject to their respective licenses.
