**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32H757XIH6 TIM1 更新中断实验

## 工程概述

本实验在 STM32H757XIH6 的 Cortex-M7（CM7）上用 TIM1 产生周期性更新中断。每次中断翻转绿灯，并通过 USART1 输出一行日志。CM7 主循环同时轮询 KEY0，用按键控制蓝灯。启动时，CM4 等待 CM7 通过硬件信号量 HSEM 0 唤醒，完成 HAL 初始化后进入空循环。

## TIM1 配置与中断流程

在 [`STM32H757XIH6.ioc`](STM32H757XIH6.ioc) 的时钟配置中，APB2 时钟为 120 MHz，TIM1 内部时钟为 240 MHz。CM7 在 [`tim.c`](CM7/Core/Src/tim.c) 中将 TIM1 配置为向上计数，预分频值 `PSC = 2399`，自动重装值 `ARR = 4999`，重复计数值 `RCR = 0`。因此，计数频率为 `240 MHz / (2399 + 1) = 100 kHz`，每计满 `4999 + 1` 个计数产生一次更新，间隔为 `50 ms`。

1. `MX_TIM1_Init()` 初始化定时器，并通过 `HAL_TIM_Base_MspInit()` 在 NVIC 中启用 `TIM1_UP_IRQn`。
2. CM7 完成外设初始化后，调用 `HAL_TIM_Base_Start_IT(&htim1)` 启动计数并打开更新中断。
3. 更新事件进入 [`TIM1_UP_IRQHandler()`](CM7/Core/Src/stm32h7xx_it.c)，并由 `HAL_TIM_IRQHandler(&htim1)` 调用 [`HAL_TIM_PeriodElapsedCallback()`](CM7/Core/Src/main.c)。回调确认中断来自 TIM1 后翻转绿灯，并输出 `[CM7] ISR Running...`。

每次中断只翻转一次电平，因此绿灯的完整亮灭周期为 100 ms，对应约 10 Hz。中断回调和主循环都通过 [`usart.c`](CM7/Core/Src/usart.c) 中的 `__io_putchar()` 使用阻塞式 USART1 输出。

## GPIO 与按键行为

| 信号 | 引脚 | 配置 |
|---|---|---|
| KEY0 | PA8 | 下拉输入，按下时为高电平 |
| LED0（绿） | PA3 | 推挽输出，初始化为高电平，由 TIM1 中断翻转 |
| LED1（蓝） | PB1 | 推挽输出，初始化为高电平，由主循环翻转 |
| USART1 | PA9（TX）、PA10（RX） | 异步串口，115200 波特率，8 数据位、无校验、1 停止位、无硬件流控 |

CM7 主循环读到 KEY0 为高电平后，延时 25 ms，翻转蓝灯并输出 `[CM7] System Running...`。长按会重复触发。松开后，本次已经触发的操作仍会完成，随后只剩定时器日志，绿灯继续翻转。

## 构建固件

将 GNU Make 和 Arm GNU Toolchain 加入 `PATH`，在工程根目录运行以下命令构建两个核心的固件。

```sh
make -C Makefile
```

单独构建时，使用 `make -C Makefile/CM4` 或 `make -C Makefile/CM7`。每个核心均生成 `.elf`、`.hex` 和 `.bin` 文件。烧录 `.bin` 时使用下表中的 Flash 起始地址。

| 核心 | 输出文件名（不含扩展名） | Flash 起始地址 |
|---|---|---|
| CM7 | `Makefile/CM7/build/STM32H757XIH6_CM7` | `0x08000000` |
| CM4 | `Makefile/CM4/build/STM32H757XIH6_CM4` | `0x08100000` |

VS Code 提供 `Build All`、`Build CM4` 和 `Build CM7` 任务。使用 Cortex-Debug 和 J-Link 调试 CM7 时，将 [`launch.json`](.vscode/launch.json) 中的 `serverpath` 指向本机的 J-Link GDB Server，并确保 `arm-none-eabi-gdb` 可用。

## 烧录与观察

1. 确认板卡提供 HSE 旁路所需的 25 MHz 外部时钟，并启用双核启动。通过 SWD 将两份固件烧录到各自的 Flash 区域。分开烧录时，保留先前写入的区域。
2. 将 PA9 接到 3.3 V USB 转串口模块的 RX，并共地。按上表的串口参数打开终端。
3. 复位板卡。未按键时，观察绿灯持续闪烁和周期性的 `[CM7] ISR Running...`。按住 KEY0 后，蓝灯反复翻转，终端也会输出 `[CM7] System Running...`。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
