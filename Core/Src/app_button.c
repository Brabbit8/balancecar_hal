#include "app_button.h"
#include "button.h"
#include "main.h"
#include "app_pwm.h"
#include "app_motor.h"
static Button_TypeDef userKey;//用户按钮
static void onuserkey_clicked(uint8_t clicks);
//按钮初始化
void App_Button_Init(void)
{
    Button_InitTypeDef config = {0};

    config.GPIOx = USER_KEY_GPIO_Port;
    config.GPIO_Pin = USER_KEY_Pin;

    My_Button_Init(&userKey, &config);
		
	My_Button_SetClickCb(&userKey, onuserkey_clicked);
}

void App_Button_proc(void)
{
    My_Button_Proc(&userKey);
}

static uint8_t pwm_on=0;

//按钮的回调函数
static void onuserkey_clicked(uint8_t clicks)
{
	
	if(clicks==1)
	{
		if(pwm_on==0)
		{
			pwm_on=1;
		}
		else
		{
			pwm_on=0;
		}
		App_Motor_Cmd(pwm_on);
	}
}