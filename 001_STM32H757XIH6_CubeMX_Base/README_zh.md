**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32H757XIH6 CubeMX 基础工程

## 工程概述

本工程以 STM32H757XIH6 为目标芯片，使用 STM32CubeMX 生成 Cortex-M7（CM7）和 Cortex-M4（CM4）的初始化代码。`STM32H757XIH6.ioc` 保存引脚、时钟、外设及工具链配置。两个核心各有独立的入口函数、启动文件和链接脚本，由 `Makefile/` 下的总 Makefile 分别构建两份固件。芯片的双核架构见 [STMicroelectronics 应用笔记 AN5557](https://www.st.com/resource/en/application_note/dm00733995-stm32h745755-and-stm32h747757-lines-dualcore-architecture-stmicroelectronics.pdf)。

启动时，CM4 进入停止模式，等待硬件信号量（HSEM）通知。CM7 完成系统时钟配置后释放信号量，唤醒 CM4，随后初始化 GPIO 和 USART1。两个核心在各自的初始化结束后进入空的主循环。这份工程可作为了解双核启动流程和添加应用逻辑的起点。


## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
