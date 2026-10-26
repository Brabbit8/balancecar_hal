#ifndef __DELAY_US_H
#define __DELAY_US_H

#include "main.h"
#include <stdint.h>

void DelayUs_Init(void);
uint64_t GetUs(void);
void DelayUs(uint32_t us);

#endif