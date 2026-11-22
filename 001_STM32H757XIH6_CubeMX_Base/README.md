[简体中文](README_zh.md) | **English**

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32H757XIH6 CubeMX Base Project

## Project Overview

This project targets the STM32H757XIH6 and uses STM32CubeMX to generate initialization code for the Cortex-M7 (CM7) and Cortex-M4 (CM4). `STM32H757XIH6.ioc` stores the pin, clock, peripheral, and toolchain settings. Each core has its own entry point, startup file, and linker script. The top-level Makefile in `Makefile/` builds a separate firmware image for each core. For details on the chip's dual-core architecture, see [STMicroelectronics application note AN5557](https://www.st.com/resource/en/application_note/dm00733995-stm32h745755-and-stm32h747757-lines-dualcore-architecture-stmicroelectronics.pdf).

During startup, CM4 enters Stop mode and waits for a hardware semaphore (HSEM) notification. After configuring the system clock, CM7 releases the semaphore to wake CM4, then initializes GPIO and USART1. Both cores enter empty main loops once their initialization is complete. The project provides a starting point for studying the dual-core startup sequence and adding application logic.


## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in the project root for the full license text. Third-party components are subject to their respective licenses.
