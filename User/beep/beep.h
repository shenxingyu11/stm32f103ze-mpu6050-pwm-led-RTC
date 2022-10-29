#ifndef __BEEF_H
#define __BEEF_H


#include "stm32f10x.h"


#define BEEP_OFF GPIO_ResetBits(GPIOA, GPIO_Pin_8);
#define BEEP_ON GPIO_SetBits(GPIOA, GPIO_Pin_8);


//º¯ÊýÉùÃ÷
void Beep_Config(void);


#endif
