#ifndef __PWM_BREATHING_H
#define	__PWM_BREATHING_H

#include "stm32f10x.h"

/*PWM���еĵ���*/
extern uint16_t  POINT_NUM	;
//����������ε�Ƶ��
extern __IO uint16_t period_class ;


	#define   BRE_TIMx                      TIM3

	#define   BRE_TIM_APBxClock_FUN        RCC_APB1PeriphClockCmd
	#define   BRE_TIM_CLK                   RCC_APB1Periph_TIM3
	#define   BRE_TIM_GPIO_APBxClock_FUN   RCC_APB2PeriphClockCmd
	#define   BRE_TIM_GPIO_CLK              (RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO)

	//��Ƶ�������Ҫ��ӳ��
	#define   BRE_GPIO_REMAP_FUN()						GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); 				

	#define  BRE_TIM_LED_PORT               GPIOB
	#define  BRE_TIM_LED_PIN                GPIO_Pin_5

	#define  BRE_TIM_OCxInit                TIM_OC2Init            //ͨ��ѡ��1~4
	#define  BRE_TIM_OCxPreloadConfig       TIM_OC2PreloadConfig 
	#define  BRE_CCRx                       CCR2

	#define   BRE_TIMx_IRQn                TIM3_IRQn              //�ж�
	#define   BRE_TIMx_IRQHandler          TIM3_IRQHandler





void      TIMx_Breathing_Init          (void);



#endif /* __PWM_BREATHING_H */
