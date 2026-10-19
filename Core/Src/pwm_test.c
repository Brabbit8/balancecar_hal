#include "pwm_test.h"
#include "app_pwm.h"
//
//@简介:对pwm模块进行测试
//      先使能电机
//			正转30%2秒
//			正转60%2秒
//			正转90%2秒
//			关闭电机

void PWM_Test(void)
{
    App_PWM_Cmd(1U);

    App_PWM_Set_L(30.0f);
    App_PWM_Set_R(30.0f);
    HAL_Delay(2000U);

    App_PWM_Set_L(60.0f);
    App_PWM_Set_R(60.0f);
    HAL_Delay(2000U);

    App_PWM_Set_L(90.0f);
    App_PWM_Set_R(90.0f);
    HAL_Delay(2000U);

    App_PWM_Set_L(0.0f);
    App_PWM_Set_R(0.0f);
    App_PWM_Cmd(0U);

    return;
}