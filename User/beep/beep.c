#include "beep.h"


 void Beep_Config(void)
{
	
	
		//����һ��GPIO_InitTypeDef���͵Ľṹ��
		GPIO_InitTypeDef GPIO_InitStructure;
		
		//����LED��ص�GPIO����ʱ��
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
		
		//ѡ��Ҫ���Ƶ�GPIO����
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	
		//��������ģʽΪͨ������ģʽ
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		
		//��������������Ϊ50MHz
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		
		//���ÿ⺯������ʼ��GPIO
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	
		GPIO_ResetBits(GPIOA, GPIO_Pin_8);
}

