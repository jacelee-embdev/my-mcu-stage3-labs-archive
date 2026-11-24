**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# My MCU Stage 3 Labs Archive

An archive of projects from the third stage of my embedded systems learning, starting with STM32H7 development using STM32CubeMX and Makefiles.

## About This Archive

This repository preserves the projects and notes from my third stage of learning. I use it to document my progress, back up code, and revisit and reproduce experiments.

Projects are numbered in learning order and preserve the code, build configuration, and state of each experiment at the time. Each project is built independently. Refer to its documentation for hardware, toolchain, and instructions for running it.

## Project Index

Select a project name to open its English documentation.

| No. | Project | Description |
|---:|---|---|
| 001 | [STM32H757XIH6 CubeMX Base Project](001_STM32H757XIH6_CubeMX_Base/README.md) | Generates initialization code for CM7 and CM4 with STM32CubeMX and builds a separate firmware image for each core using Makefiles. |
| 002 | [STM32H757XIH6 Makefile UART Logging Lab](002_STM32H757XIH6_Makefile_UART_Log/README.md) | Builds firmware for both cores with Makefiles and retargets CM7 `printf` to USART1 to emit a status line about once per second. |
| 003 | [STM32H757XIH6 Button-Controlled LED Lab](003_Key_Control_LED/README.md) | Polls KEY0 on CM7, waits 25 ms after a high reading, toggles both LEDs, and logs over USART1. |
| 004 | [STM32H757XIH6 External Interrupt Lab](004_Key_EXTI_LED/README.md) | Toggles the blue LED on a KEY0 rising-edge interrupt and the green LED periodically in the CM7 main loop, with USART1 logs from both. |
| 005 | [STM32H757XIH6 UART Receive Interrupt Lab](005_UART_RX_Interrupt/README.md) | Receives USART1 bytes by interrupt and replies with their decimal values while the CM7 main loop sends a periodic count. |
| 006 | [STM32H757XIH6 MPU Read-Only Protection and MemManage Fault Lab](006_MPU_ReadOnly_Fault/README.md) | Makes a CM7 SRAM buffer read-only with the MPU, triggers a MemManage fault by attempting a write, and reports diagnostics over USART1. |
| 007 | [STM32H757XIH6 USART DMA Idle Reception and D-Cache Lab](007_USART_DMA_IDLE_Cache/README.md) | Receives USART1 commands with DMA and IDLE detection to control the blue LED and maintains the receive buffer when D-Cache is enabled. |

## Usage

1. Clone or download this repository, then select a lab from the project index.
2. Read the project's documentation and check the MCU, hardware connections, and required toolchain.
3. Build the firmware as documented. For projects that use Makefiles, run the corresponding build commands from the project root. Projects with a `.vscode/` directory can use the configured build tasks and debug settings in VS Code.
4. Flash the firmware as documented, then check the serial output or other expected results. If a project includes multiple related firmware images, follow the documented build and flashing order.

Reproducible build outputs are excluded from version control. When using a project on its own, keep its source files, build and debug configuration, and applicable license files together.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in the repository root for the full terms. Third-party components are subject to their respective licenses.
