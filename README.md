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

## Usage

1. Clone or download this repository, then select a lab from the project index.
2. Read the project's documentation and check the MCU, hardware connections, and required toolchain.
3. Build the firmware as documented. For projects that use Makefiles, run the corresponding build commands from the project root. Projects with a `.vscode/` directory can use the configured build tasks and debug settings in VS Code.
4. Flash the firmware as documented, then check the serial output or other expected results. If a project includes multiple related firmware images, follow the documented build and flashing order.

Reproducible build outputs are excluded from version control. When using a project on its own, keep its source files, build and debug configuration, and applicable license files together.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in the repository root for the full terms. Third-party components are subject to their respective licenses.
