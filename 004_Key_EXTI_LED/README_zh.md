**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32H757XIH6 外部中断实验

## 工程概述

本实验在 STM32H757XIH6 的 Cortex-M7（CM7）上演示 GPIO 外部中断。主循环定时翻转绿灯，KEY0 触发的中断回调翻转蓝灯，两处均通过 USART1 输出日志。CM4 参与 HSEM 0 启动同步，完成 HAL 初始化后进入空循环。

## 引脚与中断流程

| 信号 | 引脚 | 配置 |
|---|---|---|
| KEY0 | PA8 | 下拉输入，EXTI8 上升沿触发 |
| LED0（绿） | PA3 | 推挽输出，初始化为高电平 |
| LED1（蓝） | PB1 | 推挽输出，初始化为高电平 |
| USART1 | PA9（TX）、PA10（RX） | 异步串口 |

CM7 配置 PA8 后使能 `EXTI9_5_IRQn`。按键使 PA8 从低电平变为高电平时，`EXTI9_5_IRQHandler()` 调用 `HAL_GPIO_EXTI_IRQHandler(KEY0_Pin)`，进入 `HAL_GPIO_EXTI_Callback()`。[GPIO 配置](CM7/Core/Src/gpio.c) · [中断处理](CM7/Core/Src/stm32h7xx_it.c) · [回调实现](CM7/Core/Src/main.c)

回调确认来源为 KEY0 且 PA8 仍为高电平后，翻转蓝灯并打印 `[CM7] ISR Running...`。长按保持高电平时，新的触发需要另一次上升沿。当前实现只在回调中复读电平，按键抖动产生的多个上升沿可能触发多次翻转。

主循环翻转绿灯，打印 `[CM7] System Running...`，再延时 1000 ms。主循环和中断都使用阻塞式串口发送。若中断打断主循环正在进行的串口发送，中断日志可能因 USART1 正忙而丢失。

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
3. 复位并运行板卡。绿灯约每秒翻转一次，终端打印 `[CM7] System Running...`。分别按下和松开 KEY0，观察按下时蓝灯翻转，并查找 `[CM7] ISR Running...`。主循环延时期间也能响应按键中断。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
