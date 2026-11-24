**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32H757XIH6 MPU 只读保护与 MemManage 异常实验

## 工程概述

本实验在 Cortex-M7（CM7）上用内存保护单元（MPU）将一个 SRAM 缓冲区设为特权只读，验证读取仍可进行，并观察写入触发的 MemManage 异常。CM4 参与 HSEM 0 启动同步，完成 HAL 初始化后进入空循环。

## 实验流程

1. CM7 完成启动同步并初始化 USART1 后，向按 32 字节对齐的缓冲区首字写入 `0x12345678`，并打印缓冲区地址和值。
2. 将整个 32 字节缓冲区配置为 MPU 区域 15，权限设为特权只读并禁止执行（XN）。随后读取首字，串口预期输出 `Read back = 0x12345678`。
3. 延时 2 秒后，程序尝试向同一地址写入 `0xA5A5A5A5`，预期进入 `MemManage_Handler()`。异常处理程序通过轮询 USART1 寄存器打印数据访问违规、目标地址和 `SCB->CFSR`。若 `MMARVALID` 置位，还会打印 `SCB->MMFAR` 中的故障地址。诊断只输出一次，CM7 随后停留在异常处理循环中。

[实验与诊断代码](CM7/Core/Src/main.c) · [异常入口](CM7/Core/Src/stm32h7xx_it.c)

## 构建固件

将 GNU Make 和 Arm GNU Toolchain 加入 `PATH`，在实验目录运行以下命令，构建 CM4 和 CM7 固件。

```sh
make -C Makefile
```

单独构建时使用 `make -C Makefile/CM4` 或 `make -C Makefile/CM7`。各核心产出 `.elf`、`.hex` 和 `.bin` 文件。烧录 `.bin` 时使用下表中的 Flash 起始地址。

| 核心 | 输出文件名（不含扩展名） | Flash 起始地址 |
|---|---|---|
| CM7 | `Makefile/CM7/build/STM32H757XIH6_CM7` | `0x08000000` |
| CM4 | `Makefile/CM4/build/STM32H757XIH6_CM4` | `0x08100000` |

VS Code 提供 `Build All`、`Build CM4` 和 `Build CM7` 任务。用 J-Link 和 Cortex-Debug 调试 CM7 时，在 `launch.json` 中设置本机 J-Link GDB Server 路径，并确保 `arm-none-eabi-gdb` 可用。[调试配置](.vscode/launch.json)

## 烧录与观察

1. 确认板卡提供 HSE 旁路所需的 25 MHz 外部时钟，并启用双核启动。通过 SWD 将两份固件写入各自的 Flash 区域。分开烧录时，保留先前写入的区域。
2. 将 USART1 TX（PA9）接到 3.3 V USB 转串口模块的 RX，并共地。终端设为 115200 波特率、8 数据位、无校验、1 停止位、无硬件流控。
3. 打开终端，复位板卡并运行。先观察写入与读回的值，再确认约 2 秒后出现 `MemManage fault captured`。故障地址有效时，它应与目标地址一致。复位可重新运行实验。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
