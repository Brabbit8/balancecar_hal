#include "encoder_test.h"
#include "app_encoder.h"
#include <stdio.h>
#include "usart.h"
//简介:对编码器的位置信号进行测量
//
void Encoder_Test(void)
{
	while(1)
	{
		float pos_l = App_Encoder_GetPos_L();
    float pos_r = App_Encoder_GetPos_R();
		char vofa_buf[64];
		int len = snprintf(vofa_buf, sizeof(vofa_buf), "%.3f,%.3f\n", pos_l, pos_r);
		HAL_UART_Transmit(&huart2, (uint8_t *)vofa_buf, (uint16_t)len, 100);
		HAL_Delay(50);
	}
}


static float last_pos_l = 0.0f;
static float last_pos_r = 0.0f;

void Encoder_M_Method_Test(void)
{
    uint32_t last_tick = HAL_GetTick();

    last_pos_l = App_Encoder_GetPos_L();
    last_pos_r = App_Encoder_GetPos_R();

    while (1)
    {
        uint32_t now = HAL_GetTick();

        if ((uint32_t)(now - last_tick) >= 50U)
        {
            uint32_t elapsed_ms = now - last_tick;
            float pos_l = App_Encoder_GetPos_L();
            float pos_r = App_Encoder_GetPos_R();

            float omega_l = (pos_l - last_pos_l) * 1000.0f
                            / (float)elapsed_ms;
            float omega_r = (pos_r - last_pos_r) * 1000.0f
                            / (float)elapsed_ms;

            char vofa_buf[64];
            int len = snprintf(
                vofa_buf,
                sizeof(vofa_buf),
                "%.3f,%.3f\n",
                omega_l,
                omega_r
            );

            if ((len > 0) && ((unsigned int)len < sizeof(vofa_buf)))
            {
                HAL_UART_Transmit(
                    &huart2,
                    (uint8_t *)vofa_buf,
                    (uint16_t)len,
                    100U
                );
            }

            last_pos_l = pos_l;
            last_pos_r = pos_r;
            last_tick = now;
        }
    }
}
void Encoder_T_Method_Test(void)
{
	while (1)
	{
		/* 速度接口返回rad/s；测试显示转换为deg/s，数值更便于和视频对照。 */
		float speed_l_deg_s = App_Encoder_GetSpeed_L() * 57.2957795f;
		float speed_r_deg_s = App_Encoder_GetSpeed_R() * 57.2957795f;
		char vofa_buf[64];
		int len = snprintf(
			vofa_buf,
			sizeof(vofa_buf),
			"%.3f,%.3f\n",
			speed_l_deg_s,
			speed_r_deg_s
		);

		if ((len > 0) && ((unsigned int)len < sizeof(vofa_buf)))
		{
			HAL_UART_Transmit(
				&huart2,
				(uint8_t *)vofa_buf,
				(uint16_t)len,
				100U
			);
		}

		HAL_Delay(20U);
	}
}
