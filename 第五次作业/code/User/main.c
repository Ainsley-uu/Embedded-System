/*******************************************************************************
* 实验名称  	: PWM呼吸灯实验
* 实验说明  : 
* 接线说明  : 1，LED模块-->单片机底座管脚（STM32核心板）
					D2-->P16（PA6）
						
* 实验现象	: 开发板上D2指示灯由暗变亮，再由亮变暗，呈现呼吸灯效果
*******************************************************************************/

#include "system.h"
#include "SysTick.h"
#include "led.h"
#include "pwm.h"
#include "public.h"
#include "key.h"

#define motor PAout(11)

u8 HighTime = 40;


//#define LED (GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15)
#define LED GPIO_Pin_All
u8 smgduan[]={0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07,0x7F, 0x6F};//段选
u8 smgwei[8]={0x00,0x20,0x10,0x30,0x08,0x28,0x18,0x38}; //位选

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
void RCCINIT() //系统初始化函数
{
	SystemInit();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);		  //开启B端口的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);		  //开启A端口的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//如果不加这条语句程序显示就会出错，即没有打开端口复用功能的时钟配置
}

void TIM3_Init(u16 per,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);//使能TIM4时钟
	
	TIM_TimeBaseInitStructure.TIM_Period=per;   //自动装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc; //分频系数
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //设置向上计数模式
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //开启定时器中断
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;//定时器中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	
	
	TIM_Cmd(TIM3,ENABLE); //使能定时器	
}

void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update))
	{	
		static u8 cnt = 0;
		
		if( cnt >= 100 ){
			cnt = cnt % 100;
		}
		if( cnt <= HighTime ){
			motor = 1;
		}else{
			motor = 0;
		}
		// elecCnt++;
		// if(elecCnt <= gap) motor = 1;
		// else if(elecCnt <= 100) motor = 0;
		// else
		// {
		// 	motor = 0;
		// 	elecCnt = 0;
		// }
		cnt++;
	}

	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);	
}

int main()
{
	
	u8 i, key, ge, shi;
	
	// LED_Init();
	SysTick_Init(72);
	KEY_Init();
	RCCINIT();
	GPIOINIT();
	TIM3_Init(500,72-1);
		  
	while(1)
	{
		GPIOINIT();
		key=KEY_Scan(0);   //扫描按键
		switch(key)
		{
			case KEY1_VALUE: HighTime+=2;break;      //按下K_UP按键    点亮D2指示灯
			case KEY2_VALUE: HighTime--;break;    //按下K_DOWN按键  熄灭D2指示灯
		}
		
			ge = HighTime % 10;
			shi = HighTime / 10;
			
			GPIO_Write(GPIOA,(u16)(smgduan[shi]));
			GPIO_Write(GPIOB,(u16)(smgwei[0]));
			delay_ms(1);
		
			GPIO_Write(GPIOA,(u16)(smgduan[ge]));
			GPIO_Write(GPIOB,(u16)(smgwei[1]));
		delay_ms(1);	
	}
	

}
