[简体中文](README_zh.md) | **English**

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32H757XIH6 Makefile UART Logging Lab

## Overview

This lab adds serial logging to the Cortex-M7 (CM7) side of a dual-core STM32H757XIH6 Makefile project. After initializing USART1, the CM7 main loop uses `printf` to send `[CM7] System Running...` and waits 1000 ms after each line. Cortex-M4 (CM4) takes part in dual-core startup synchronization, completes HAL initialization, and enters an empty main loop. The serial output shows when CM7 has completed initialization and reached the application loop.

## Dual-core startup and log output

1. CM4 enables notifications for hardware semaphore 0 (HSEM 0) and puts the D2 domain in Stop mode. CM7 waits for that state before initializing the HAL and configuring the system clock.
2. CM7 takes and releases HSEM 0 to wake CM4. After the D2 domain wakes, CM7 initializes GPIO and USART1. CM7 calls `Error_Handler()` if either wait times out.
3. In the GNU Arm build, `_write()` in the CM7 [`syscalls.c`](CM7/Core/Src/syscalls.c) passes each byte to `__io_putchar()` in [`usart.c`](CM7/Core/Src/usart.c). The function sends one byte at a time using the blocking `HAL_UART_Transmit()` call. Each log line ends with `\r\n`.

## Build the firmware

Add GNU Make and the Arm GNU Toolchain (`arm-none-eabi-gcc`, `arm-none-eabi-objcopy`, and `arm-none-eabi-size`) to `PATH`. Run the following command from the lab directory to build the CM4 and CM7 firmware images.

```sh
make -C Makefile
```

To build a single core, run `make -C Makefile/CM4` or `make -C Makefile/CM7`. Each core's `build/` directory sits next to its Makefile and contains `.elf`, `.hex`, and `.bin` files with the same basename. Use the `.elf` file for debugging. A `.hex` file carries programming addresses. When programming a `.bin` file, enter the corresponding Flash start address in the programming tool.

| Core | Output basename | Flash start address |
|---|---|---|
| CM7 | `Makefile/CM7/build/STM32H757XIH6_CM7` | `0x08000000` |
| CM4 | `Makefile/CM4/build/STM32H757XIH6_CM4` | `0x08100000` |

Open the lab directory in VS Code to use the `Build All`, `Build CM4`, or `Build CM7` tasks. The supplied Cortex-Debug launch configuration uses J-Link over SWD to debug CM7. Build the firmware first, set `serverpath` in [`launch.json`](.vscode/launch.json) to your local J-Link GDB Server executable, and make sure `arm-none-eabi-gdb` is available.

## Flash and observe the output

1. Confirm that the board supplies the 25 MHz external clock expected by the HSE bypass configuration and that booting is enabled for both cores. Use SWD to program each image into its Flash region. If you program the images separately, preserve the first region when programming the second.
2. Connect the board's USART1 TX (PA9) to RX on a 3.3 V USB-to-serial adapter and connect their grounds. PA10 is USART1 RX and can be left unconnected when observing the log. Set the serial terminal to 115200 baud, 8 data bits, no parity, one stop bit, and no hardware flow control.
3. Open the terminal, reset the board, and let the firmware run. Expect one `[CM7] System Running...` line about every second. If output is missing, check the TX-to-RX connection and ground, confirm that both images were programmed and the external clock is running, and check that CM7 passed startup synchronization and the debugger resumed from `main`.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in the project root for the full license text. Third-party components are subject to their respective licenses.
