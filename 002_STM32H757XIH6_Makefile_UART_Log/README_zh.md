**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32H757XIH6 Makefile 串口日志实验

## 工程概述

本工程在 STM32H757XIH6 的双核 Makefile 工程上实现了 Cortex-M7（CM7）的串口日志输出。CM7 初始化 USART1 后，在主循环中调用 `printf` 输出 `[CM7] System Running...`，每次发送后延时 1000 ms。Cortex-M4（CM4）参与双核启动同步，完成 HAL 初始化后进入空的主循环。串口日志可用来确认 CM7 已完成初始化并进入应用循环。

## 双核启动与日志路径

1. CM4 使能硬件信号量 0（HSEM 0）的通知，令 D2 域进入停止模式。CM7 等待 CM4 进入该状态，再初始化 HAL 和系统时钟。
2. CM7 获取并释放 HSEM 0 以唤醒 CM4，确认 D2 域恢复后，初始化 GPIO 和 USART1。等待停止或唤醒超时都会进入 `Error_Handler()`。
3. 在 GNU Arm 构建中，CM7 的 [`syscalls.c`](CM7/Core/Src/syscalls.c) 通过 `_write()` 逐字节调用 [`usart.c`](CM7/Core/Src/usart.c) 中的 `__io_putchar()`。该函数调用阻塞式 `HAL_UART_Transmit()` 发送单个字节，日志以 `\r\n` 换行。

## 构建固件

将 GNU Make 和 Arm GNU Toolchain（`arm-none-eabi-gcc`、`arm-none-eabi-objcopy`、`arm-none-eabi-size`）加入 `PATH`。在本实验目录运行以下命令，构建 CM4 和 CM7 两份固件。

```sh
make -C Makefile
```

需要单独构建某个核心时，运行 `make -C Makefile/CM4` 或 `make -C Makefile/CM7`。各核心的 `build/` 目录位于相应 Makefile 旁，产出同名的 `.elf`、`.hex` 和 `.bin` 文件。`.elf` 可用于调试，`.hex` 包含烧录地址，使用 `.bin` 则需在烧录工具中指定对应的起始地址。

| 核心 | 输出文件名（不含扩展名） | Flash 起始地址 |
|---|---|---|
| CM7 | `Makefile/CM7/build/STM32H757XIH6_CM7` | `0x08000000` |
| CM4 | `Makefile/CM4/build/STM32H757XIH6_CM4` | `0x08100000` |

在 VS Code 中打开本实验目录，可以运行 `Build All`、`Build CM4` 或 `Build CM7` 任务。随工程提供的 Cortex-Debug 启动配置使用 J-Link 和 SWD 调试 CM7。使用前请先构建固件，并将 [`launch.json`](.vscode/launch.json) 中的 `serverpath` 改为本机 J-Link GDB Server 的安装路径，确保 `arm-none-eabi-gdb` 可用。

## 烧录与观察

1. 确认板卡为 STM32H757XIH6 提供与 HSE 旁路配置匹配的 25 MHz 外部时钟，并启用两个核心的启动。使用 SWD 将两份固件烧录到各自的 Flash 区域。分开烧录时，避免第二次擦除第一份固件所在的区域。
2. 将板上的 USART1 TX（PA9）接到 3.3 V USB 转串口模块的 RX，并共地。PA10 为 USART1 RX，仅观察日志时可省去该引脚的连接。串口终端设为 `115200` 波特率、8 数据位、无校验、1 停止位、无硬件流控。
3. 打开串口终端，复位板卡并让程序运行。终端应约每秒显示一行 `[CM7] System Running...`。若没有输出，先检查 TX/RX 接线和共地，再确认两个镜像均已烧录、外部时钟正常、CM7 已通过启动同步，以及调试器已从 `main` 继续运行。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
