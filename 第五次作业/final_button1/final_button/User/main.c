/*******************************************************************************
* ʵ������  	: ��������ʵ��
* ʵ��˵��  : 
* ����˵��  : 
				1����������ģ��-->��Ƭ�������ܽţ�STM32���İ壩
					K1-->P00��PB8��
					K2-->P01��PB9��
			  2��LEDģ��-->��Ƭ�������ܽţ�STM32���İ壩
					D2-->P34��PA12��
					
					P26(PB1) ���PWM�������Ƶ��ת��/��������
					P22(PB5) --> A
					P23(PB4) --> B
					P24(PB3) --> C
						
* ʵ������	: ����K1����D1����ռ�ձ����ӣ�����K2����D1��ռ�ձȼ�С
*******************************************************************************/

#include "system.h"
#include "SysTick.h"
#include "led.h"
#include "key.h"

#define LED GPIO_Pin_All

u16 smgduan[10] = {0x003F, 0x0006, 0x005B, 0x004F, 0x0066, 0x006D, 0x007D, 0x0007,0x007F, 0x006F};//��ѡ
u8 smgwei[8] = {0x00,0x20,0x10,0x30,0x08,0x28,0x18,0x38}; //λѡ
u16 tempa[10] = {0};

void GPIIOINIT()
{
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;//�����������
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}

void GPIOINIT()	  //�˿ڳ�ʼ������
{
/*********����һ��GPIO_InitTypeDef ���͵Ľṹ��**********/
	GPIO_InitTypeDef GPIO_InitStructure;

/*********ѡ��Ҫ���Ƶ�GPIOx������**********/
	GPIO_InitStructure.GPIO_Pin=LED;

/*********��������ģʽΪͨ���������**********/
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;

/*********������������Ϊ50MHZ**********/
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;

/*��ʼ����Ӧ��GPIOB������*/
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable,ENABLE);//�ѵ���������ͨIO��

/*********ѡ��Ҫ���Ƶ�GPIOx������**********/
	GPIO_InitStructure.GPIO_Pin=LED;

/*********��������ģʽΪͨ���������**********/
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;

/*********������������Ϊ50MHZ**********/
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;

/*��ʼ����Ӧ��GPIOA������*/
	GPIO_Init(GPIOA,&GPIO_InitStructure); 	
}

void RCCINIT()
{
	SystemInit();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);		  //����B�˿ڵ�ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);		  //����A�˿ڵ�ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);				//�������������������ʾ�ͻ������û�д򿪶˿ڸ��ù��ܵ�ʱ������
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);			
}

void TIMINIT()
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;

	TIM_OCInitTypeDef TIM_OCInitStructure;//����TIM_OCInitStruct��ָ���Ĳ�����ʼ������TIMx

	//TIM3��ʱ����ʼ��
	TIM_TimeBaseInitStructure.TIM_Period = 500-1;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72-1;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, & TIM_TimeBaseInitStructure);

	GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3,ENABLE);	//�ı�ָ���ܽŵ�ӳ��

	//PWM��ʼ��	  //����TIM_OCInitStruct��ָ���Ĳ�����ʼ������TIMx
//	TIM_OCInitTypeDef TIM_OCInitStructure;//�ṹ�嶨��Ҳ�൱�ڶ������������Ͷ���u32��һ���ģ�Ҫ������ǰ�棬����ᱨ��
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;//PWM���ʹ��
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_High;

	TIM_OC4Init(TIM3,&TIM_OCInitStructure);
	//ע��˴���ʼ��ʱTIM_OC4Init������TIM_OCInit������������Ϊ�̼���İ汾��һ����

	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);//ʹ�ܻ���ʧ��TIMx��CCR4�ϵ�Ԥװ�ؼĴ���
	
	TIM_Cmd(TIM3,ENABLE);//ʹ�ܻ���ʧ��TIMx����


}

int main()
{
	u8 flag = 0;	//����
	u8 key = 0;
	u16 i = 400;	//i/500Ϊռ�ձ�
	u16 j = 0;		//�����λ����ʾ
	u16 k = 0;		//Ϊ��ֹGPIO_Write���ǣ�����tempa�ı�smgduan
	u16 n = 0;		//ռ�ձȰٷֱ�
  u16 number[8] = {0};
	u16 time_count = 0;
	u16 loop_time = 10;
  u16 step = 10; //iÿ�ε��ڵķ���	
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
		key = KEY_Scan(0);   //ɨ�谴��
		switch(key)
		{
			case KEY1_VALUE: led2 = 0;		//����K_UP����    ����D1ָʾ�ƣ�ռ�ձȣ�
												for(k=0; k<10; k++)
												{
													tempa[k] = smgduan[k];
												}
												i += step;
											break;    
			case KEY2_VALUE: led2 = 1;  	//����K_DOWN����  Ϩ��D1ָʾ�ƣ�ռ�ձ�-
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
		
		TIM_SetCompare4(TIM3, i);		//����TIMx����Ƚ�4�Ĵ���ֵ	
		//delay_ms(10);
		
		for(time_count=0; time_count<loop_time; time_count++)
		{
			for(j=0; j<4; j++)			
			{
				if(j==2)
				{
					GPIO_Write(GPIOA,(tempa[(number[j])])+0x80);	//��3λ��ʾС����
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

