#ifndef __SYSTICK_H
#define __SYSTICK_H


#include "stm32f10x.h"


void Delay_inita(u8 SYSCLK);
void SysTick_InitC(void);
void Delay_ms(u16 nms);
void Delay_us(u32 nus);

#endif /* __SYSTICK_H */
