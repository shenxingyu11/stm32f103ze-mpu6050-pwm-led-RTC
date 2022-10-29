 
#include "stm32f10x.h"
#include "stm32f10x_it.h"

#include "bsp_SysTick.h"
#include "bsp_dht11.h"
#include "bsp_breathing.h"

#include "bsp_usart.h"
#include "bsp_rtc.h"
#include "bsp_key.h"
#include "bsp_i2c.h"
#include "mpu6050.h"
#include "beep.h"
#include "9341_lcd.h"
#include "fonts.h"
#include <string.h>


u8 temp;
u8 humi;
uint32_t warn_temp=35;
uint32_t warn_humi=80;

// N = 2^32/365/24/60/60 = 136 ��

/*ʱ��ṹ�壬Ĭ��ʱ��2000-01-01 00:00:00*/
struct rtc_time systmtime=
{
0,0,0,1,1,2022,0
};

extern __IO uint32_t TimeDisplay ;


#define TASK_ENABLE 0
extern unsigned int Task_Delay[NumOfTask];


/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{
	unsigned short timeCount = 0;	//���ͼ������
	int step=0;
	int a=0;
	int b=0;
	int c=0;
	short Accel[3];
	short Gyro[3];
	float Temp;
	char DHT_buff[20];
	char MPU_1_buff[20];
	char MPU_2_buff[20];
	char MPU_3_buff[20];
	char MPU_4_buff[20];
	char TIM_buff[20];
		
	//ϵͳĬ��ѡ��
	int sys = 0;
	SystemInit();//����ϵͳʱ��Ϊ72M	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//�жϿ�������������						
	USART_Config();        //��ʼ������1
	Delay_inita(72);

  ILI9341_Init();
	//SysTick_InitC();	//��ʼ��systick
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;
	
	Key_GPIO_Config();     //�����ĳ�ʼ��
	RTC_NVIC_Config();     //RTC��ʼ��
	RTC_CheckAndConfig(&systmtime);//����RTC���ж����ȼ� 
	//Usart3_Init(115200);							//����3������ESP8266��
	
	//I2C��ʼ��
	i2c_GPIO_Config();
  //MPU6050��ʼ��
	MPU6050_Init();
	Beep_Config();
	ILI9341_GramScan(6);//6ģʽΪ�󲿷�Һ����ʾ��Ĭ����ʾ����
	ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);
	DHT11_Init();
	TIMx_Breathing_Init();
	/*while(DHT11_Init())
	{
		printf("DHT11 Error \r\n");
		Delay_ms(1000);
	}*/
	
	
	
	
	
	//�������壬while��ѭ��
	while(1)
	{	
		//����ѡ���ϵͳ
		if(sys == 1)			  //ʱ��ϵͳ��һ������
		{			
			sys = 0;          //��ϵͳѡ����0
			while(1)
			{
			  /* ÿ��1s ����һ��ʱ��*/
				if (TimeDisplay == 1)
				{
					/* ��ǰʱ�� */
					Time_Display( RTC_GetCounter(),&systmtime); 
					TimeDisplay = 0;
				}
				
				//���°�����ͨ�������޸�ʱ��f
				if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON  )
				{
					struct rtc_time set_time;

					/*ʹ�ô��ڽ������õ�ʱ�䣬��������ʱע��ĩβҪ�ӻس�*/
					Time_Regulate_Get(&set_time);
					/*�ý��յ���ʱ������RTC*/
					Time_Adjust(&set_time);
					
					//�򱸷ݼĴ���д���־
					BKP_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
				} 			
				DHT11_Read_Data(&temp,&humi);//��ȡ�¶Ⱥ�ʪ��
				
				ILI9341_DispStringLine_EN(LINE(1),"         TIME MODE");
				sprintf(TIM_buff ,"    Time:     %0.2d:%0.2d:%0.2d",systmtime.tm_hour,systmtime.tm_min,systmtime.tm_sec);
				LCD_ClearLine(LINE(2));
				ILI9341_DispStringLine_EN(LINE(2),TIM_buff);
				
				
				//ͨ������K2ȥ����ѭ��
			  if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON  )
				{
					ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);
					break;
				}	
			
			}
		}
		
		
		if(sys == 2)        //��ʪ��ϵͳ��һ������
		{
			sys = 0;          //��ϵͳѡ����0
			while(1)
			{
				DHT11_Read_Data(&temp,&humi);//��ȡ�¶Ⱥ�ʪ��
		    printf("��ǰ�¶ȣ�%d ��ǰʪ�ȣ�%d�\\r\n",temp,humi);
				printf("�¶���ֵ  %d ʪ����ֵ  %d�\\r\n",warn_temp,warn_humi);
			  printf("---------------------------------------------\n");
				if(temp < warn_temp && humi < warn_humi)
				{
					printf("DHT11��������");
				}
				if(temp >=warn_temp)
				{
					TIM3->PSC = 0x0048; //����˸
					BEEP_ON;
					printf("�����¶ȹ���  ");
				}
				if( humi>=warn_humi)
				{
					TIM3->PSC = 0x0048; //����˸
					BEEP_ON;
					printf("����ʪ�ȹ���  \n  ");
				}

								
				if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON  )
				{
					printf("�޸ĵ�ǰ�¶���ֵ   \n");
					printf("\r�������޸�ֵ\n");
					scanf("%d", &warn_temp);
					printf("�޸ĵ�ǰʪ����ֵ   \n");
					printf("\r�������޸�ֵ\n");
					scanf("%d", &warn_humi);
					printf("�޸ĳɹ�!!!\n");
					printf("---------------------------------------------\n");
				}
			
				Delay_ms(500);//ÿ���1s��ӡһ������
				ILI9341_DispStringLine_EN(LINE(1),"         DHT11 MODE");
				sprintf(DHT_buff ,"temp:%6d     humi:%6d",temp,humi);
				LCD_ClearLine(LINE(2));
				ILI9341_DispStringLine_EN(LINE(2),DHT_buff);
				//ͨ������K2ȥ����ѭ��
			  if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON  )
				{
					ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);
					break;
				}
			}
		}
		if(sys == 3)        //��ʪ��ϵͳ��һ������
		{
			sys = 0; 
			//��ϵͳѡ����0
			step=0;
			if (MPU6050ReadID() == 1)
		  {
		  	while(1)
			  {		
			
					  MPU6050ReadAcc(Accel);			
						printf("\r\n���ٶȣ� %8d%8d%8d    \n",Accel[0],Accel[1],Accel[2]);
						MPU6050ReadGyro(Gyro);
						printf("�����ǣ� %8d%8d%8d    \n",Gyro[0],Gyro[1],Gyro[2]);
						
						MPU6050_ReturnTemp(&Temp); 
						printf("�¶ȣ� %8.2f\n",Temp);
					  printf("---------------------------------------------\n");
						Delay_ms(500);
//						Task_Delay[1]=500;//����һ�����ݣ��ɸ����Լ���������߲���Ƶ�ʣ���100ms����һ��

						Delay_ms(500);//ÿ���1s��ӡһ������
									
					if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON  )
				{
					printf("      ���Ѿ�����%d��\n",step);
					Delay_ms(1000);
					continue;
				}
				    if(((Gyro[0]-a)>0)&&((Gyro[1]-b)>0)&&((Gyro[2]-c)>0))
						{
							step++;
						}
						a=Gyro[0];
						b=Gyro[1];
						c=Gyro[2];
				    sprintf(MPU_4_buff ,"Step:%7d",step);
				    LCD_ClearLine(LINE(4));
				    ILI9341_DispStringLine_EN(LINE(4),MPU_4_buff);
				    
				    ILI9341_DispStringLine_EN(LINE(0),"         MPU6050 MODE");
				    sprintf(MPU_1_buff ,"ACC:%8d%8d%8d",Accel[0],Accel[1],Accel[2]);
				    LCD_ClearLine(LINE(1));
				    ILI9341_DispStringLine_EN(LINE(1),MPU_1_buff);
					
						sprintf(MPU_2_buff ,"Gyro:%7d%7d%7d",Gyro[0],Gyro[1],Gyro[2]);
				    LCD_ClearLine(LINE(2));
				    ILI9341_DispStringLine_EN(LINE(2),MPU_2_buff);
						
						sprintf(MPU_3_buff ,"Temp:%7.2f",Temp);
				    LCD_ClearLine(LINE(3));
				    ILI9341_DispStringLine_EN(LINE(3),MPU_3_buff);
						
					//ͨ������K2ȥ����ѭ��
					if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON  )
					{
						ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);
						break;
					}
			 }
		}
	}
		
		
			
		
		if(sys == 0)     //ѡ��ϵͳ
		{

			BEEP_OFF;
			printf("������ѡ����棬������Ҫѡ�����ʾ������\n");
			printf("1,��ʾ��ǰ��ʱ��\n");
			printf("2,��ʾ��ǰ����ʪ��\n");
			printf("3,��ʾ��ǰ�������ǵĸ�������\n");
			printf("---------------------------------------------\n");
			ILI9341_DispStringLine_EN(LINE(6),"        1,TIME MODE");
			ILI9341_DispStringLine_EN(LINE(7),"       2,DHT11 MODE");
			ILI9341_DispStringLine_EN(LINE(8),"      3,MPU6050 MODE");
			ILI9341_DispStringLine_EN(LINE(9),"------------------------------");
			LCD_SetTextColor(RED);
			ILI9341_DispStringLine_EN(LINE(5),"            MODE");
			LCD_SetTextColor(BLACK);
			scanf("%d",&sys);
			printf("%d\n",sys);

		}
				
			
	}

	test(sys);
}
/*********************************************END OF FILE**********************/
