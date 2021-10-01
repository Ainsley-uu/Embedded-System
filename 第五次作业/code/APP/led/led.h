#ifndef _led_H
#define _led_H

#include "stm32f10x.h"

/*  LED时钟端口、引脚定义 */
#define LED_PORT 			GPIOA   
#define LED_PIN 			GPIO_Pin_0|GPIO_Pin_1 | GPIO_Pin_11
#define LED_PORT_RCC		RCC_APB2Periph_GPIOA

#define led1 PAout(0)  	//D1指示灯连接的是PA0管脚
#define led2 PAout(1)  	


void LED_Init(void);


#endif

