#ifndef _led_H
#define _led_H

#include "stm32f10x.h"

/*  LEDʱ�Ӷ˿ڡ����Ŷ��� */
#define LED_PORT 			GPIOA   
#define LED_PIN 			GPIO_Pin_12
#define LED_PORT_RCC		RCC_APB2Periph_GPIOA

//#define led1 PAout(11)  	//D1ָʾ�����ӵ���PA11�ܽ�
#define led2 PAout(12)  	


void LED_Init(void);


#endif
