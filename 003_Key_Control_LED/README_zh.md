**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32H757XIH6 按键控制 LED 实验

## 工程概述

本实验使用 STM32H757XIH6 的 Cortex-M7（CM7）读取按键、控制两路 LED，并通过 USART1 输出日志。CM4 在启动时等待 CM7 通过硬件信号量 HSEM 0 唤醒，完成 HAL 初始化后进入空循环。

## GPIO 配置与按键行为

| 信号 | 引脚 | 配置 |
|---|---|---|
| KEY0 | PA8 | 下拉输入，按下时为高电平 |
| LED0（绿） | PA3 | 推挽输出，初始化为高电平 |
| LED1（蓝） | PB1 | 推挽输出，初始化为高电平 |
| USART1 | PA9（TX）、PA10（RX） | 异步串口 |

引脚配置见 [CubeMX 工程文件](STM32H757XIH6.ioc) 和 CM7 的 [`main.h`](CM7/Core/Inc/main.h)。LED 的亮灭与输出电平的对应关系取决于板上接法。

CM7 在 [`main.c`](CM7/Core/Src/main.c) 的 `USER CODE BEGIN 3` 区域轮询 KEY0。每次读到高电平，程序延时 25 ms，翻转两个 LED，并打印 `[CM7] System Running...`。长按会重复触发。即使在延时期间松开，已经触发的那次翻转和打印仍会执行。

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
2. 将 PA9 接到 3.3 V USB 转串口模块的 RX，并共地。串口设为 115200 波特率、8 数据位、无校验、1 停止位、无硬件流控。
3. 复位并运行板卡。按住 KEY0 时，两个 LED 反复翻转，终端持续输出日志。松开后，LED 保持最后的状态，串口输出停止。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
