#include "beep.h"


 void Beep_Config(void)
{
	
	
		//定义一个GPIO_InitTypeDef类型的结构体
		GPIO_InitTypeDef GPIO_InitStructure;
		
		//开启LED相关的GPIO外设时钟
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
		
		//选择要控制的GPIO引脚
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	
		//设置引脚模式为通用推挽模式
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		
		//设置引脚数量表为50MHz
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		
		//调用库函数，初始化GPIO
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	
		GPIO_ResetBits(GPIOA, GPIO_Pin_8);
}

