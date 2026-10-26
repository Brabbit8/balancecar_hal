#ifndef __TASK_H
#define __TASK_H

#include "main.h"
#include <stdint.h>

/*
 * 周期执行宏，单位为毫秒。
 *
 * 使用方法：
 *
 * void App_MPU6050_Proc(void)
 * {
 *     PERIODIC(5);
 *
 *     App_MPU6050_Update();
 * }
 */
#define PERIODIC(T)                                      \
    do                                                  \
    {                                                   \
        static uint32_t periodic_next_time = 0;        \
        uint32_t periodic_now = HAL_GetTick();         \
                                                        \
        if ((int32_t)(periodic_now - periodic_next_time) < 0) \
        {                                               \
            return;                                     \
        }                                               \
                                                        \
        periodic_next_time += (T);                      \
    } while (0)

#endif