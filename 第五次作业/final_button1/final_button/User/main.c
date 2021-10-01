/*******************************************************************************
* 实验名称  	: 按键控制实验
* 实验说明  : 
* 接线说明  : 
				1，独立按键模块-->单片机底座管脚（STM32核心板）
					K1-->P00（PB8）
					K2-->P01（PB9）
			  2，LED模块-->单片机底座管脚（STM32核心板）
					D2-->P34（PA12）
					
					P26(PB1) 输出PWM波（控制电机转速/灯亮暗）
					P22(PB5) --> A
					P23(PB4) --> B
					P24(PB3) --> C
						
* 实验现象	: 按下K1键，D1亮，占空比增加；按下K2键，D1灭，占空比减小
*******************************************************************************/

#include "system.h"
#include "SysTick.h"
#include "led.h"
#include "key.h"

#define LED GPIO_Pin_All

u16 smgduan[10] = {0x003F, 0x0006, 0x005B, 0x004F, 0x0066, 0x006D, 0x007D, 0x0007,0x007F, 0x006F};//段选
u8 smgwei[8] = {0x00,0x20,0x10,0x30,0x08,0x28,0x18,0x38}; //位选
u16 tempa[10] = {0};

void GPIIOINIT()
{
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;//复用推挽输出
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}

void GPIOINIT()	  //端口初始化函数
{
/*********定义一个GPIO_InitTypeDef 类型的结构体**********/
	GPIO_InitTypeDef GPIO_InitStructure;

/*********选择要控制的GPIOx的引脚**********/
	GPIO_InitStructure.GPIO_Pin=LED;

/*********设置引脚模式为通用推完输出**********/
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;

/*********设置引脚速率为50MHZ**********/
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;

/*初始化相应的GPIOB的设置*/
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable,ENABLE);//把调试设置普通IO口

/*********选择要控制的GPIOx的引脚**********/
	GPIO_InitStructure.GPIO_Pin=LED;

/*********设置引脚模式为通用推完输出**********/
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;

/*********设置引脚速率为50MHZ**********/
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;

/*初始化相应的GPIOA的设置*/
	GPIO_Init(GPIOA,&GPIO_InitStructure); 	
}

void RCCINIT()
{
	SystemInit();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);		  //开启B端口的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);		  //开启A端口的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);				//如果不加这条语句程序显示就会出错，即没有打开端口复用功能的时钟配置
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);			
}

void TIMINIT()
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;

	TIM_OCInitTypeDef TIM_OCInitStructure;//根据TIM_OCInitStruct中指定的参数初始化外设TIMx

	//TIM3定时器初始化
	TIM_TimeBaseInitStructure.TIM_Period = 500-1;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72-1;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, & TIM_TimeBaseInitStructure);

	GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3,ENABLE);	//改变指定管脚的映射

	//PWM初始化	  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx
//	TIM_OCInitTypeDef TIM_OCInitStructure;//结构体定义也相当于定义变量，这个和定义u32是一样的，要放在最前面，否则会报警
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;//PWM输出使能
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_High;

	TIM_OC4Init(TIM3,&TIM_OCInitStructure);
	//注意此处初始化时TIM_OC4Init而不是TIM_OCInit，否则会出错。因为固件库的版本不一样。

	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);//使能或者失能TIMx在CCR4上的预装载寄存器
	
	TIM_Cmd(TIM3,ENABLE);//使能或者失能TIMx外设


}

int main()
{
	u8 flag = 0;	//方向
	u8 key = 0;
	u16 i = 400;	//i/500为占空比
	u16 j = 0;		//数码管位数显示
	u16 k = 0;		//为防止GPIO_Write覆盖，利用tempa改变smgduan
	u16 n = 0;		//占空比百分比
  u16 number[8] = {0};
	u16 time_count = 0;
	u16 loop_time = 10;
  u16 step = 10; //i每次调节的幅度	
	RCCINIT();
	GPIOINIT();
	GPIIOINIT();
	TIMINIT();
	SysTick_Init(72);
	
	LED_Init();
	KEY_Init();
	
  for(k=0; k<10; k++)
	{
		tempa[k] = smgduan[k];
	}
	
	while(1)
	{
		key = KEY_Scan(0);   //扫描按键
		switch(key)
		{
			case KEY1_VALUE: led2 = 0;		//按下K_UP按键    点亮D1指示灯，占空比＋
												for(k=0; k<10; k++)
												{
													tempa[k] = smgduan[k];
												}
												i += step;
											break;    
			case KEY2_VALUE: led2 = 1;  	//按下K_DOWN按键  熄灭D1指示灯，占空比-
												for(k=0; k<10; k++)
												{
													tempa[k] = smgduan[k] + 0x1000;
												}
												i -= step;
											break;   
		}
		delay_ms(10);

		n = 2*i;
		number[0] = n/1000;
		number[1] = (n%1000)/100;
		number[2] = (n%100)/10;
		number[3] = n%10;	
		
		TIM_SetCompare4(TIM3, i);		//设置TIMx捕获比较4寄存器值	
		//delay_ms(10);
		
		for(time_count=0; time_count<loop_time; time_count++)
		{
			for(j=0; j<4; j++)			
			{
				if(j==2)
				{
					GPIO_Write(GPIOA,(tempa[(number[j])])+0x80);	//第3位显示小数点
					GPIO_Write(GPIOB,(u16)(smgwei[j]));				
				}
				
				else
				{
					GPIO_Write(GPIOA,(tempa[(number[j])]));
					GPIO_Write(GPIOB,(u16)(smgwei[j]));
				}
				delay_us(100);
			}
			delay_us(100);
			
		}
		
	}	
}

