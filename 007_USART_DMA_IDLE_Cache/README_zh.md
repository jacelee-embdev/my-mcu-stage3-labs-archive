**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32H757XIH6 USART DMA 空闲接收与 D-Cache 实验

## 工程概述

CM7 使用 USART1 和 DMA 接收不定长的串口数据，以 IDLE 空闲事件或缓冲区收满作为本轮接收的结束条件。程序识别收到的命令并驱动蓝灯，主循环约每秒输出一行运行日志。CM4 参与 HSEM 0 双核启动同步，初始化 DMA 后进入空循环。

## 接收与缓存维护

1. 初始化 GPIO、DMA 和 USART1 后，CM7 调用 `HAL_UARTEx_ReceiveToIdle_DMA()`，通过 DMA1 Stream 0 普通模式接收最多 100 字节，并关闭 DMA 半传输中断。
2. 收到 IDLE 事件或收满 100 字节时，HAL 调用 `HAL_UARTEx_RxEventCallback()`。程序按回调提供的长度将数据复制到本地数组，追加字符串结束符，再启动下一轮 DMA 接收。
3. 程序在本轮数据中查找区分大小写的 `LED ON` 或 `LED OFF`。蓝灯 LED1 连接在 PB1，低电平点亮，高电平熄灭。串口运行日志经阻塞式 USART1 发送。

接收缓冲区实际占 128 字节，按 32 字节对齐。CM7 的 Flash 链接脚本将 `.dma_buffer` 段放在从 `0x24000000` 开始的 D1 AXI SRAM 中。启用 D-Cache 时，程序在启动 DMA 前和读取接收数据前调用 `SCB_InvalidateDCache_by_Addr()`，让 CPU 从 SRAM 读取 DMA 已写入的数据。固件启动代码未主动开启 D-Cache。

[接收与命令处理](CM7/Core/Src/main.c) · [USART1 与 DMA 配置](CM7/Core/Src/usart.c) · [CM7 链接脚本](Makefile/CM7/stm32h757xx_flash_CM7.ld)

## 构建固件

将 GNU Make 和 Arm GNU Toolchain 加入 `PATH`，从本实验目录构建两个核心。

```sh
make -C Makefile
```

单独构建可运行 `make -C Makefile/CM4` 或 `make -C Makefile/CM7`。各核心产出 `.elf`、`.hex` 和 `.bin` 文件。

| 核心 | 输出文件名（不含扩展名） | Flash 起始地址 |
|---|---|---|
| CM7 | `Makefile/CM7/build/STM32H757XIH6_CM7` | `0x08000000` |
| CM4 | `Makefile/CM4/build/STM32H757XIH6_CM4` | `0x08100000` |

VS Code 提供 `Build All`、`Build CM4` 和 `Build CM7` 任务。通过 J-Link 和 Cortex-Debug 调试 CM7 时，在 `launch.json` 中设置本机 J-Link GDB Server 路径，并确保 `arm-none-eabi-gdb` 可用。[调试配置](.vscode/launch.json)

## 烧录与观察

1. 确认板卡提供 HSE 旁路所需的 25 MHz 外部时钟，并启用双核启动。通过 SWD 将两个镜像烧录到各自的 Flash 区域。
2. 使用 3.3 V USB 转串口模块，将 PA9（USART1 TX）接到模块 RX，PA10（USART1 RX）接到模块 TX，并共地。终端设为 115200 波特率、8 数据位、无校验、1 停止位、无硬件流控。
3. 打开终端，复位板卡并运行，观察 `[CM7] System Running...`。分别一次性发送 `LED ON` 和 `LED OFF`，每条发送后留出空闲间隔，观察蓝灯点亮和熄灭。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
