 
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

// N = 2^32/365/24/60/60 = 136 年

/*时间结构体，默认时间2000-01-01 00:00:00*/
struct rtc_time systmtime=
{
0,0,0,1,1,2022,0
};

extern __IO uint32_t TimeDisplay ;


#define TASK_ENABLE 0
extern unsigned int Task_Delay[NumOfTask];


/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{
	unsigned short timeCount = 0;	//发送间隔变量
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
		
	//系统默认选择
	int sys = 0;
	SystemInit();//配置系统时钟为72M	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//中断控制器分组设置						
	USART_Config();        //初始化串口1
	Delay_inita(72);

  ILI9341_Init();
	//SysTick_InitC();	//初始化systick
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;
	
	Key_GPIO_Config();     //按键的初始化
	RTC_NVIC_Config();     //RTC初始化
	RTC_CheckAndConfig(&systmtime);//配置RTC秒中断优先级 
	//Usart3_Init(115200);							//串口3，驱动ESP8266用
	
	//I2C初始化
	i2c_GPIO_Config();
  //MPU6050初始化
	MPU6050_Init();
	Beep_Config();
	ILI9341_GramScan(6);//6模式为大部分液晶显示屏默认显示方向
	ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);
	DHT11_Init();
	TIMx_Breathing_Init();
	/*while(DHT11_Init())
	{
		printf("DHT11 Error \r\n");
		Delay_ms(1000);
	}*/
	
	
	
	
	
	//程序主体，while死循环
	while(1)
	{	
		//简易选择的系统
		if(sys == 1)			  //时间系统那一套内容
		{			
			sys = 0;          //将系统选择置0
			while(1)
			{
			  /* 每过1s 更新一次时间*/
				if (TimeDisplay == 1)
				{
					/* 当前时间 */
					Time_Display( RTC_GetCounter(),&systmtime); 
					TimeDisplay = 0;
				}
				
				//按下按键，通过串口修改时间f
				if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON  )
				{
					struct rtc_time set_time;

					/*使用串口接收设置的时间，输入数字时注意末尾要加回车*/
					Time_Regulate_Get(&set_time);
					/*用接收到的时间设置RTC*/
					Time_Adjust(&set_time);
					
					//向备份寄存器写入标志
					BKP_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
				} 			
				DHT11_Read_Data(&temp,&humi);//读取温度和湿度
				
				ILI9341_DispStringLine_EN(LINE(1),"         TIME MODE");
				sprintf(TIM_buff ,"    Time:     %0.2d:%0.2d:%0.2d",systmtime.tm_hour,systmtime.tm_min,systmtime.tm_sec);
				LCD_ClearLine(LINE(2));
				ILI9341_DispStringLine_EN(LINE(2),TIM_buff);
				
				
				//通过按键K2去跳出循环
			  if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON  )
				{
					ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);
					break;
				}	
			
			}
		}
		
		
		if(sys == 2)        //温湿度系统那一套内容
		{
			sys = 0;          //将系统选择置0
			while(1)
			{
				DHT11_Read_Data(&temp,&humi);//读取温度和湿度
		    printf("当前温度：%d 当前湿度：%d\r\n",temp,humi);
				printf("温度阈值  %d 湿度阈值  %d\r\n",warn_temp,warn_humi);
			  printf("---------------------------------------------\n");
				if(temp < warn_temp && humi < warn_humi)
				{
					printf("DHT11正常运行");
				}
				if(temp >=warn_temp)
				{
					TIM3->PSC = 0x0048; //灯闪烁
					BEEP_ON;
					printf("环境温度过高  ");
				}
				if( humi>=warn_humi)
				{
					TIM3->PSC = 0x0048; //灯闪烁
					BEEP_ON;
					printf("环境湿度过高  \n  ");
				}

								
				if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON  )
				{
					printf("修改当前温度阈值   \n");
					printf("\r请输入修改值\n");
					scanf("%d", &warn_temp);
					printf("修改当前湿度阈值   \n");
					printf("\r请输入修改值\n");
					scanf("%d", &warn_humi);
					printf("修改成功!!!\n");
					printf("---------------------------------------------\n");
				}
			
				Delay_ms(500);//每间隔1s打印一次数据
				ILI9341_DispStringLine_EN(LINE(1),"         DHT11 MODE");
				sprintf(DHT_buff ,"temp:%6d     humi:%6d",temp,humi);
				LCD_ClearLine(LINE(2));
				ILI9341_DispStringLine_EN(LINE(2),DHT_buff);
				//通过按键K2去跳出循环
			  if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON  )
				{
					ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);
					break;
				}
			}
		}
		if(sys == 3)        //温湿度系统那一套内容
		{
			sys = 0; 
			//将系统选择置0
			step=0;
			if (MPU6050ReadID() == 1)
		  {
		  	while(1)
			  {		
			
					  MPU6050ReadAcc(Accel);			
						printf("\r\n加速度： %8d%8d%8d    \n",Accel[0],Accel[1],Accel[2]);
						MPU6050ReadGyro(Gyro);
						printf("陀螺仪： %8d%8d%8d    \n",Gyro[0],Gyro[1],Gyro[2]);
						
						MPU6050_ReturnTemp(&Temp); 
						printf("温度： %8.2f\n",Temp);
					  printf("---------------------------------------------\n");
						Delay_ms(500);
//						Task_Delay[1]=500;//更新一次数据，可根据自己的需求，提高采样频率，如100ms采样一次

						Delay_ms(500);//每间隔1s打印一次数据
									
					if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON  )
				{
					printf("      您已经走了%d步\n",step);
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
						
					//通过按键K2去跳出循环
					if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON  )
					{
						ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);
						break;
					}
			 }
		}
	}
		
		
			
		
		if(sys == 0)     //选择系统
		{

			BEEP_OFF;
			printf("下面是选择界面，请输入要选择的显示的内容\n");
			printf("1,显示当前的时间\n");
			printf("2,显示当前的温湿度\n");
			printf("3,显示当前的陀螺仪的各种数据\n");
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
