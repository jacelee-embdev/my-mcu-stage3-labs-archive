[简体中文](README_zh.md) | **English**

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32H757XIH6 TIM1 Update Interrupt Lab

## Overview

This lab uses TIM1 on the STM32H757XIH6 Cortex-M7 (CM7) to generate periodic update interrupts. Each interrupt toggles the green LED and prints one line over USART1. The CM7 main loop also polls KEY0 to control the blue LED. At startup, CM4 waits for CM7 to wake it using hardware semaphore 0 (HSEM 0). CM4 then initializes the HAL and enters an empty loop.

## TIM1 configuration and interrupt flow

The clock configuration in [`STM32H757XIH6.ioc`](STM32H757XIH6.ioc) sets the APB2 clock to 120 MHz and the TIM1 internal clock to 240 MHz. The CM7 [`tim.c`](CM7/Core/Src/tim.c) configures TIM1 to count up with prescaler `PSC = 2399`, auto-reload value `ARR = 4999`, and repetition counter `RCR = 0`. The counter therefore runs at `240 MHz / (2399 + 1) = 100 kHz`. An update occurs every `4999 + 1` counts, or every `50 ms`.

1. `MX_TIM1_Init()` initializes the timer. Its `HAL_TIM_Base_MspInit()` hook enables `TIM1_UP_IRQn` in the NVIC.
2. After initializing the peripherals, CM7 calls `HAL_TIM_Base_Start_IT(&htim1)` to start the counter and enable its update interrupt.
3. An update enters [`TIM1_UP_IRQHandler()`](CM7/Core/Src/stm32h7xx_it.c). `HAL_TIM_IRQHandler(&htim1)` then calls [`HAL_TIM_PeriodElapsedCallback()`](CM7/Core/Src/main.c). The callback checks that TIM1 raised the interrupt, toggles the green LED, and prints `[CM7] ISR Running...`.

Each interrupt toggles the output once, so a full green LED blink cycle takes 100 ms, or about 10 Hz. The interrupt callback and the main loop both use blocking USART1 output through `__io_putchar()` in [`usart.c`](CM7/Core/Src/usart.c).

## GPIO and button behavior

| Signal | Pin | Configuration |
|---|---|---|
| KEY0 | PA8 | Pull-down input, high when pressed |
| LED0 (green) | PA3 | Push-pull output, initialized high and toggled by the TIM1 interrupt |
| LED1 (blue) | PB1 | Push-pull output, initialized high and toggled by the main loop |
| USART1 | PA9 (TX), PA10 (RX) | Asynchronous serial, 115200 baud, 8 data bits, no parity, one stop bit, and no hardware flow control |

When the CM7 main loop reads KEY0 high, it waits 25 ms, toggles the blue LED, and prints `[CM7] System Running...`. Holding the button repeats the sequence. Releasing it lets any iteration already in progress finish, after which the timer continues to toggle the green LED and print its periodic log.

## Build the firmware

Add GNU Make and the Arm GNU Toolchain to `PATH`, then run the following command from the project root to build both firmware images.

```sh
make -C Makefile
```

Use `make -C Makefile/CM4` or `make -C Makefile/CM7` to build one core at a time. Each build produces `.elf`, `.hex`, and `.bin` files. Use the Flash start addresses below when programming the `.bin` files.

| Core | Output path without extension | Flash start address |
|---|---|---|
| CM7 | `Makefile/CM7/build/STM32H757XIH6_CM7` | `0x08000000` |
| CM4 | `Makefile/CM4/build/STM32H757XIH6_CM4` | `0x08100000` |

VS Code provides `Build All`, `Build CM4`, and `Build CM7` tasks. To debug CM7 with Cortex-Debug and J-Link, set `serverpath` in [`launch.json`](.vscode/launch.json) to your local J-Link GDB Server and make sure `arm-none-eabi-gdb` is available.

## Flash and observe the output

1. Confirm that the board supplies a 25 MHz external clock for HSE bypass and that both cores are enabled to boot. Program both images to their respective Flash regions over SWD. If you program them separately, preserve the region programmed first.
2. Connect PA9 to RX on a 3.3 V USB-to-serial adapter and connect their grounds. Open a terminal with the USART1 settings in the table above.
3. Reset the board. With KEY0 released, the green LED flashes continuously and the terminal shows periodic `[CM7] ISR Running...` messages. Hold KEY0 to make the blue LED toggle repeatedly and produce `[CM7] System Running...` messages.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in the project root for the full license text. Third-party components are subject to their respective licenses.
