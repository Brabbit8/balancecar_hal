# BalanceCar HAL

基于 STM32F103C8、STM32CubeMX 和 HAL 库的平衡车学习项目，使用 Keil MDK-ARM 开发。

当前已完成电池电压采样、按键与电机 PWM、编码器 T 法测速、MPU6050 姿态读取、双轮 PID 转速控制，以及串级 PID 直立平衡控制。

## 2026-10-18：完成电池模块

- 配置 ADC1 注入组，使用 PB0 / ADC1_IN8 采集分压后的电池电压。
- 使用 TIM2 更新事件 TRGO 触发 ADC。在当前 72 MHz 定时器时钟、PSC = 7、ARR = 999 的配置下，采样频率为 9 kHz。
- 实现 `app_bat.c/.h`，封装 ADC 校准、采样启动、转换完成回调，以及原始值、毫伏和伏特读取接口。
- 针对本工程 STM32F1 HAL 首次回调后关闭 JEOC 中断的行为，在回调中按需重新启动注入组中断，继续等待外部触发。
- 实现 `test.c/.h`，每约 100 ms 通过 USART2 发送最新电池电压，支持 VOFA+ FireWater。
- 移除临时固定数值和启动诊断输出；电压文本使用整数格式拼接，保留三位小数。

## PID 双轮速度控制

- `pid.c/.h` 提供 PID 初始化、目标值设置、输出限幅、计算和复位接口。
- `app_motor.c/.h` 为左右电机分别维护 PID 控制器，每个控制周期读取编码器速度，计算控制电压，再按电池电压换算为 PWM 占空比。
- `PERIODIC(1)` 使用 HAL 毫秒节拍限制电机控制流程为约 1 ms 一次；微秒时间由 Cortex-M3 DWT 周期计数器提供。
- 单击用户按键切换电机使能状态。左右方向引脚的极性需与实际电机接线匹配；当前工程已调整左电机方向映射，使正向目标对应车辆前进方向。

## 串级 PID 直立平衡控制

- 新增 `app_control.c/.h`，实现「直立环（角度环）+ 角速度环」的串级控制：外环角度 PID 输出作为内环角速度 PID 的设定值。
- 外环反馈为 MPU6050 pitch 角，内环反馈为陀螺仪 X 轴角速度；角度均换算为弧度。
- 内环输出经平衡车动力学模型换算为轮心加速度，积分后得到目标轮速，下发左右电机。
- 直立环 Kp=4.0、输出限幅 ±12.57 rad/s；角速度环 Kp=10.0、Ki=10.0、输出限幅 ±125.7。
- 控制周期约 5 ms（`PERIODIC(5)`），微秒计时由 Cortex-M3 DWT 提供。
- 主循环由 `App_Control_Proc()` 驱动，原速度阶梯测试与 USART 调试输出已移除。

## 工程结构

```text
balancecar_hal.ioc            CubeMX 外设、时钟及引脚配置
Core/Inc/app_bat.h / .c       电池电压采样（ADC 注入组 + TIM2 触发）
Core/Inc/app_button.h / .c    用户按键（消抖、单击/多击/长按）
Core/Inc/app_led.h / .c       电量 LED 显示与低电压闪烁
Core/Inc/app_pwm.h / .c       PWM 与 TB6612 电机驱动（正反转、待机）
Core/Inc/app_encoder.h / .c   编码器计数与 T 法测速
Core/Inc/app_mpu6050.h / .c   MPU6050 I2C 驱动与互补滤波姿态解算
Core/Inc/app_motor.h / .c     双轮速度 PID 控制
Core/Inc/app_control.h / .c   串级 PID 直立平衡控制（角度环 + 角速度环）
Core/Inc/pid.h / .c           PID 控制器（初始化、限幅、计算、复位）
Core/Inc/qmath.h / .c         查表与线性插值数学函数
Core/Inc/task.h / .c          基于 HAL_GetTick 的周期任务宏
Core/Inc/delay_us.h / .c      基于 Cortex-M3 DWT 的微秒计时
Core/Src/main.c               外设初始化与主循环（调用各 App 模块）
Core/Src/stm32f1xx_it.c       CubeMX 生成的硬件中断入口
Core/Src/{adc,gpio,tim,i2c,usart}.c  CubeMX 生成的外设驱动
Tests/                        各模块测试代码
Drivers/                      STM32 HAL 和 CMSIS 驱动
MDK-ARM/                      Keil 工程与启动文件
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
- 各阶段测试通过 USART2 输出调试数据（如电池电压、左右轮转速）；当前主循环运行串级平衡控制，串口调试输出已注释。
- 看不到文本时检查接收区的“显示接收数据”和“采样数据包”；图表需要添加波形控件并绑定通道。

## 构建与下载

1. 使用 Keil 打开 `MDK-ARM/balancecar_hal.uvprojx`。
2. 当前使用 ARM Compiler 5.06 update 7 编译；安装对应 STM32F1 器件包。
3. 编译后通过 ST-Link 下载，完成后按一次复位键，或在 Flash Download 设置中启用 Reset and Run。
4. 外设配置通过同一个 `balancecar_hal.ioc` 修改并重新生成；生成文件内手写代码放在 `USER CODE` 区域。

README 和 CHANGELOG 记录各阶段的构建与上板验证情况。工程使用 Keil ARMCC 5.06 update 7；提交前请在 Keil 中重新编译并确认目标板运行表现。

## 第三方代码

HAL、CMSIS 等第三方代码保留原有许可证和版权声明，参见 `Drivers` 内的许可证文件。本项目未另行指定整仓开源许可证。
