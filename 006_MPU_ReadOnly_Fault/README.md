[简体中文](README_zh.md) | **English**

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32H757XIH6 MPU Read-Only Protection and MemManage Fault Lab

## Overview

This lab uses the Cortex-M7 (CM7) Memory Protection Unit (MPU) to make an SRAM buffer read-only for privileged code. The demo verifies that reads still work and shows how a write triggers a MemManage fault. CM4 takes part in HSEM 0 startup synchronization, initializes the HAL, and enters an empty loop.

## What the demo does

1. After startup synchronization and USART1 initialization, CM7 writes `0x12345678` to the first word of a 32-byte-aligned buffer and prints its address and value.
2. The demo configures MPU region 15 to cover the entire 32-byte buffer. It gives privileged code read-only access and marks the region execute-never (XN). The firmware then reads the first word again and should print `Read back = 0x12345678`.
3. After two seconds, the code tries to write `0xA5A5A5A5` to the same address, which should invoke `MemManage_Handler()`. The handler polls the USART1 registers to report the data access violation, the target address, and `SCB->CFSR`. It also prints the fault address from `SCB->MMFAR` when `MMARVALID` is set. The diagnostics appear once, and CM7 stays in the fault handler's loop.

[Demo and diagnostics](CM7/Core/Src/main.c) · [Exception entry](CM7/Core/Src/stm32h7xx_it.c)

## Build the firmware

Add GNU Make and the Arm GNU Toolchain to `PATH`. Run this command from the lab directory to build the CM4 and CM7 firmware images.

```sh
make -C Makefile
```

To build a single core, run `make -C Makefile/CM4` or `make -C Makefile/CM7`. Each build produces `.elf`, `.hex`, and `.bin` files. Use the Flash start address in the table when programming a `.bin` file.

| Core | Output basename | Flash start address |
|---|---|---|
| CM7 | `Makefile/CM7/build/STM32H757XIH6_CM7` | `0x08000000` |
| CM4 | `Makefile/CM4/build/STM32H757XIH6_CM4` | `0x08100000` |

VS Code provides `Build All`, `Build CM4`, and `Build CM7` tasks. To debug CM7 with J-Link and Cortex-Debug, point `serverpath` in `launch.json` to the local J-Link GDB Server and make sure `arm-none-eabi-gdb` is available. [Debug configuration](.vscode/launch.json)

## Flash and observe the output

1. Confirm that the board supplies a 25 MHz external clock for HSE bypass and that both cores are enabled to boot. Program both images into their respective Flash regions over SWD. If programming them separately, preserve the first region when writing the second.
2. Connect USART1 TX (PA9) to RX on a 3.3 V USB-to-serial adapter and connect their grounds. Set the terminal to 115200 baud, 8 data bits, no parity, one stop bit, and no hardware flow control.
3. Open the terminal, reset the board, and let it run. Check the initial value and the read-back value, then look for `MemManage fault captured` about two seconds later. If the fault address is valid, it should match the target address. Reset the board to run the demo again.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in the project root for the full license text. Third-party components are subject to their respective licenses.
