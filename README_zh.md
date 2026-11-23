**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# MCU 嵌入式学习第三阶段工程归档

本仓库用于归档我在嵌入式学习第三阶段的工程，从使用 STM32CubeMX 和 Makefile 开发 STM32H7 开始。

## 归档说明

本仓库保存第三阶段积累的工程和笔记，用于记录学习过程、备份代码，以及回顾和复现实验。

工程按学习顺序编号，保留当时的代码、构建配置和实验状态。各工程独立构建，具体硬件、工具链和运行方法以各工程的说明为准。

## 工程索引

单击工程名称可查看对应的中文说明。

| 编号 | 工程 | 实现内容 |
|---:|---|---|
| 001 | [STM32H757XIH6 CubeMX 基础工程](001_STM32H757XIH6_CubeMX_Base/README_zh.md) | 使用 STM32CubeMX 生成 CM7 和 CM4 的初始化代码，并通过 Makefile 分别构建两份固件。 |
| 002 | [STM32H757XIH6 Makefile 串口日志实验](002_STM32H757XIH6_Makefile_UART_Log/README_zh.md) | 通过 Makefile 构建双核固件，并在 CM7 上将 `printf` 重定向到 USART1，约每秒输出一行运行日志。 |
| 003 | [STM32H757XIH6 按键控制 LED 实验](003_Key_Control_LED/README_zh.md) | 在 CM7 上轮询 KEY0，检测到高电平后延时 25 ms，翻转两个 LED 并通过 USART1 输出日志。 |
| 004 | [STM32H757XIH6 外部中断实验](004_Key_EXTI_LED/README_zh.md) | CM7 通过 KEY0 上升沿中断翻转蓝灯，主循环定时翻转绿灯，两处均通过 USART1 输出日志。 |

## 使用说明

1. 克隆或下载本仓库，从工程索引选择实验。
2. 阅读工程说明，核对 MCU、硬件连接和所需工具链。
3. 按工程说明构建固件。使用 Makefile 的工程可以在工程根目录执行相应的构建命令。提供 `.vscode/` 配置的工程可以在 VS Code 中使用配置好的构建任务和调试设置。
4. 按工程说明烧录固件，检查串口输出或其他预期现象。包含多个配套固件的工程按说明中的顺序构建和烧录。

可重新生成的编译产物不纳入版本管理。单独使用某个工程时，请一并保留源码、构建和调试配置，以及适用的许可证文件。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
