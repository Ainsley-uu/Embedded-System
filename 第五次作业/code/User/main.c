/*******************************************************************************
* ʵ������  	: PWM������ʵ��
* ʵ��˵��  : 
* ����˵��  : 1��LEDģ��-->��Ƭ�������ܽţ�STM32���İ壩
					D2-->P16��PA6��
						
* ʵ������	: ��������D2ָʾ���ɰ��������������䰵�����ֺ�����Ч��
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
u8 smgduan[]={0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07,0x7F, 0x6F};//��ѡ
u8 smgwei[8]={0x00,0x20,0x10,0x30,0x08,0x28,0x18,0x38}; //λѡ

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
void RCCINIT() //ϵͳ��ʼ������
{
	SystemInit();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);		  //����B�˿ڵ�ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);		  //����A�˿ڵ�ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//�������������������ʾ�ͻ������û�д򿪶˿ڸ��ù��ܵ�ʱ������
}

void TIM3_Init(u16 per,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);//ʹ��TIM4ʱ��
	
	TIM_TimeBaseInitStructure.TIM_Period=per;   //�Զ�װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc; //��Ƶϵ��
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //�������ϼ���ģʽ
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //������ʱ���ж�
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;//��ʱ���ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//�����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	
	
	TIM_Cmd(TIM3,ENABLE); //ʹ�ܶ�ʱ��	
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
		key=KEY_Scan(0);   //ɨ�谴��
		switch(key)
		{
			case KEY1_VALUE: HighTime+=2;break;      //����K_UP����    ����D2ָʾ��
			case KEY2_VALUE: HighTime--;break;    //����K_DOWN����  Ϩ��D2ָʾ��
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
