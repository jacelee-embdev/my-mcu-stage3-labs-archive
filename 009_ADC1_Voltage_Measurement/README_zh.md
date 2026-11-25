**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32H757XIH6 ADC1 电压测量实验

## 工程概述

本实验在 STM32H757XIH6 的 Cortex-M7（CM7）上使用 ADC1 读取 PA1 的模拟输入，将 16 位转换值换算成电压，并通过 USART1 输出。CM4 在启动时等待 CM7 通过硬件信号量 HSEM 0 唤醒，完成 HAL 初始化后进入空循环。

## 引脚配置

| 信号 | 引脚 | 配置 |
|---|---|---|
| ADC1_INP17 | PA1 | ADC1 通道 17，单端模拟输入 |
| KEY0 | PA8 | 下拉输入，按下时为高电平 |
| LED1（蓝） | PB1 | 推挽输出，初始化为高电平，检测到 KEY0 为高电平后翻转 |
| USART1 | PA9（TX）、PA10（RX） | 115200 波特率，8 数据位，无校验，1 停止位，无硬件流控 |

引脚配置见 [CubeMX 工程文件](STM32H757XIH6.ioc) 和 CM7 的 [`main.h`](CM7/Core/Inc/main.h)。主循环在读出 ADC 值之前检查 KEY0。读到高电平时，延时 25 ms 后翻转蓝灯。

## ADC 转换与串口输出

CM7 的 [`adc.c`](CM7/Core/Src/adc.c) 将 ADC1 配置为 16 位、单通道、软件触发的单次转换，采样时间为 1.5 个 ADC 时钟周期，转换结果写入数据寄存器。

[`main.c`](CM7/Core/Src/main.c) 在初始化外设后调用一次 `HAL_ADC_Start()`，随后在主循环中调用 `HAL_ADC_GetValue()` 读取数据寄存器。程序按 `原始值 × 3.3 / 65536` 计算电压，四舍五入到毫伏，并通过重定向到 USART1 的 `printf` 输出，例如 `VOLT: 1.650 V`。每次输出后延时 2 秒。

当前固件只在启动时触发一次转换，后续串口输出重复读取该结果。更改 PA1 的输入电压后，复位板卡即可进行下一次测量。公式中的 3.3 V 是代码使用的参考值，读数取决于板卡实际的 ADC 参考电压。

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
2. 将 PA9 接到 3.3 V USB 转串口模块的 RX，并共地。按上表配置串口终端。
3. 将已知且符合板卡模拟输入范围的直流电压接到 PA1，并将信号源与板卡共地。先让输入电压稳定，再复位板卡。

4. 在终端查看约每两秒输出一次的 `VOLT:` 读数。用另一个已知电压验证时，先调整输入，再复位并比较读数。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
