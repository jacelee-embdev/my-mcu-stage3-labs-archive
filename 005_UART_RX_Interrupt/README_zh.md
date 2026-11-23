**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32H757XIH6 串口接收中断实验

## 工程概述

本实验在 STM32H757XIH6 的 Cortex-M7（CM7）上演示 USART1 逐字节中断接收。接收回调打印字节的十进制值，主循环约每秒发送一次计数并翻转绿灯。CM4 参与 HSEM 0 启动同步，完成 HAL 初始化后进入空循环。

## 串口接收与发送

| 信号 | 引脚 | 配置 |
|---|---|---|
| USART1 TX | PA9 | 连接 USB 转串口模块的 RX |
| USART1 RX | PA10 | 连接 USB 转串口模块的 TX |
| LED0（绿） | PA3 | 推挽输出，初始化为高电平 |

USART1 初始化后，CM7 调用 `HAL_UART_Receive_IT(&huart1, &rx_data, 1)` 等待一个字节。接收时，`USART1_IRQHandler()` 调用 `HAL_UART_IRQHandler()`，由 HAL 在接收完成后调用 `HAL_UART_RxCpltCallback()`。回调确认来源为 USART1，打印 `recv:<十进制值>`，再启动下一次单字节接收。[串口配置](CM7/Core/Src/usart.c) · [中断处理](CM7/Core/Src/stm32h7xx_it.c) · [应用逻辑](CM7/Core/Src/main.c)

主循环递增 8 位计数器，用阻塞式 `HAL_UART_Transmit()` 发送十进制计数，然后翻转绿灯并延时 1000 ms。计数从 1 开始，达到 255 后回到 0。回调中的 `printf` 也使用同一个阻塞式发送接口，若 USART1 正在发送计数，回显可能丢失。回调打印完成后才重新启动接收，适合逐字节观察。

## 构建固件

将 GNU Make 和 Arm GNU Toolchain 加入 `PATH`，在实验目录运行以下命令构建两个核心的固件。

```sh
make -C Makefile
```

单独构建时，在命令中使用 `Makefile/CM4` 或 `Makefile/CM7`。两个核心各自产出 `.elf`、`.hex` 和 `.bin` 文件。烧录 `.bin` 时需指定下表中的 Flash 起始地址。

| 核心 | 输出文件名（不含扩展名） | Flash 起始地址 |
|---|---|---|
| CM7 | `Makefile/CM7/build/STM32H757XIH6_CM7` | `0x08000000` |
| CM4 | `Makefile/CM4/build/STM32H757XIH6_CM4` | `0x08100000` |

VS Code 提供 `Build All`、`Build CM4` 和 `Build CM7` 任务。用 Cortex-Debug 调试 CM7 时，将 [`launch.json`](.vscode/launch.json) 中的 `serverpath` 指向本机 J-Link GDB Server，并确保 `arm-none-eabi-gdb` 可用。

## 烧录与观察

1. 确认板卡提供 HSE 旁路所需的 25 MHz 外部时钟，并启用双核启动。通过 SWD 将两份固件烧录到各自的 Flash 区域。分开烧录时，保留先前写入的区域。
2. 使用 3.3 V USB 转串口模块，按上表交叉连接 TX 和 RX，并共地。终端设为 115200 波特率、8 数据位、无校验、1 停止位、无硬件流控。
3. 复位并运行板卡，确认绿灯约每秒翻转一次，终端逐行显示计数。发送单个 ASCII 字符 `A`，观察 `recv:65`。发送时关闭终端自动追加换行，以便只发送一个字节。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
