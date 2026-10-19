/**
  ******************************************************************************
  * @file    button.h
  * @author  铁头山羊
  * @version V 1.0.0
  * @date    2022年9月7日
  * @brief   按钮驱动程序
  ******************************************************************************
  */
	
#ifndef BUTTON_H
#define BUTTON_H

#include "stm32f1xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 使用前由 CubeMX 将目标 GPIO 配置为上拉输入，按下接地（低电平有效）。
 * MX_GPIO_Init() 之后调用 My_Button_Init()，再注册回调。
 * My_Button_Proc() 应在主循环中频繁调用，建议每 1~5 ms 扫描一次。
 * 所有回调在调用 My_Button_Proc() 的上下文执行，不是硬件中断回调。
 */

typedef struct
{
	GPIO_TypeDef *GPIOx;   /* GPIO port configured by CubeMX. */
	
	uint16_t GPIO_Pin;     /* HAL GPIO_PIN_x mask. */
} Button_InitTypeDef;

typedef struct 
{
	/* 初始化参数 */
	GPIO_TypeDef *GPIOx;
	uint16_t GPIO_Pin;
	void (*button_pressed_cb)(void);
	void (*button_released_cb)(void);
	void (*button_clicked_cb)(uint8_t clicks);
	void (*button_long_pressed_cb)(uint8_t ticks);
	uint32_t LongPressThreshold;
	uint32_t LongPressTickInterval;
	uint32_t ClickInterval; 
	
	uint8_t  LastState;     // 按钮上次的状态，0 - 松开，1 - 按下
	uint8_t  ChangePending; // 按钮的状态是否正在发生改变
	uint32_t PendingTime;   // 按钮状态开始变化的时间
	
	uint32_t LastPressedTime;  // 按钮上次按下的时间
	uint32_t LastReleasedTime; // 按钮上次松开的时间
	
	uint8_t LongPressTicks;
	uint32_t LastLongPressTickTime; 
	
	uint8_t ClickCnt;
	
} Button_TypeDef;

void My_Button_Init(Button_TypeDef *Button, Button_InitTypeDef *Button_InistStruct);
void My_Button_Proc(Button_TypeDef *Button);
uint8_t MyButton_GetState(Button_TypeDef *Button);

void My_Button_SetLongPressCb(Button_TypeDef *Button, void (*LongPressCb)(uint8_t ticks));
void My_Button_SetPressCb(Button_TypeDef *Button, void (*button_pressed_cb)(void));
void My_Button_SetReleaseCb(Button_TypeDef *Button, void (*button_released_cb)(void));
void My_Button_SetClickCb(Button_TypeDef *Button, void (*button_clicked_cb)(uint8_t clicks));

void My_Button_ClickIntervalConfig(Button_TypeDef *Button, uint32_t Interval);
void My_Button_LongPressConfig(Button_TypeDef *Button, uint32_t Throshold, uint32_t TickInterval);


#ifdef __cplusplus
}
#endif

#endif /* BUTTON_H */
