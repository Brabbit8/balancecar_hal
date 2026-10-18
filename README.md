# BalanceCar HAL

基于 STM32F103C8、STM32CubeMX 和 HAL 库的平衡车学习项目，使用 Keil MDK-ARM 开发。

当前完成：电池电压采样模块，以及通过 USART2 向 VOFA+ 输出电压的测试流程。

## 2026-10-18：完成电池模块

- 配置 ADC1 注入组，使用 PB0 / ADC1_IN8 采集分压后的电池电压。
- 使用 TIM2 更新事件 TRGO 触发 ADC。在当前 72 MHz 定时器时钟、PSC = 7、ARR = 999 的配置下，采样频率为 9 kHz。
- 实现 `app_bat.c/.h`，封装 ADC 校准、采样启动、转换完成回调，以及原始值、毫伏和伏特读取接口。
- 针对本工程 STM32F1 HAL 首次回调后关闭 JEOC 中断的行为，在回调中按需重新启动注入组中断，继续等待外部触发。
- 实现 `test.c/.h`，每约 100 ms 通过 USART2 发送最新电池电压，支持 VOFA+ FireWater。
- 移除临时固定数值和启动诊断输出；电压文本使用整数格式拼接，保留三位小数。

## 工程结构

```text
balancecar_hal.ioc           CubeMX 外设、时钟及引脚配置
Core/Inc/app_bat.h           电池采样接口、满量程换算参数
Core/Src/app_bat.c           电池采样及转换完成回调
Core/Inc/test.h              模块测试接口
Core/Src/test.c              电压串口输出测试
Core/Src/main.c              外设初始化、Test_Init、Test_Loop
Core/Src/stm32f1xx_it.c      CubeMX 生成的硬件中断入口
Drivers/                    STM32 HAL 和 CMSIS 驱动
MDK-ARM/                    Keil 工程与启动文件
```

## 电池模块接口

| 接口 | 用途 |
| --- | --- |
| `App_Bat_Init()` | 校准 ADC，启动注入组中断及 TIM2 外部触发；初始化时调用一次 |
| `App_Bat_GetRaw()` | 读取最近一次 ADC 原始结果，范围 0～4095 |
| `App_Bat_GetMillivolts()` | 读取换算后的电池电压，单位 mV |
| `App_Bat_GetVoltage()` | 读取换算后的电池电压，单位 V |

## 电压换算参数

`Core/Inc/app_bat.h` 当前设置：

```c
#define APP_BAT_FULL_SCALE_MV 8400U
```

该参数假设 ADC 满量程对应电池电压 8.4 V；实际测量前须根据参考电压及分压电阻核实。若 ADC 参考电压为 3.3 V，上方电阻为 R1、下方电阻为 R2，则电池满量程为 `3.3 × (R1 + R2) / R2` V。

电池需经分压接入 PB0；不能将 8.4 V 电池直接连接到 ADC 引脚。

## 串口与 VOFA+

- USART2：115200，8 数据位，1 停止位，无校验，无硬件流控。
- STM32 PA2 / TX 接 USB 转串口模块 RX；GND 共地。
- 若需要接收，STM32 PA3 / RX 接 USB 转串口模块 TX。
- VOFA+ 选择转串口模块实际的 COM 号及 FireWater 数据引擎。
- 输出示例为 `6.697\n`，表示按当前参数换算的 6.697 V。
- 串口输出约 10 Hz，独立于 ADC 的 9 kHz 采样频率。
- 看不到文本时检查接收区的“显示接收数据”和“采样数据包”；图表需要添加波形控件并绑定通道。

## 构建与下载

1. 使用 Keil 打开 `MDK-ARM/balancecar_hal.uvprojx`。
2. 当前使用 ARM Compiler 5.06 update 7 编译；安装对应 STM32F1 器件包。
3. 编译后通过 ST-Link 下载，完成后按一次复位键，或在 Flash Download 设置中启用 Reset and Run。
4. 外设配置通过同一个 `balancecar_hal.ioc` 修改并重新生成；生成文件内手写代码放在 `USER CODE` 区域。

当前整理版本已编译通过：0 错误、0 警告。开发过程中已实测串口电压输出；本次整理后的连续采样修正仍需重新烧录并通过改变输入电压验证更新效果。

## 第三方代码

HAL、CMSIS 等第三方代码保留原有许可证和版权声明，参见 `Drivers` 内的许可证文件。本项目未另行指定整仓开源许可证。
