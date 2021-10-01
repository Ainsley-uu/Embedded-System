#ifndef _key_H
#define _key_H


#include "system.h"
 
#define KEY1_Pin    GPIO_Pin_12    
#define KEY2_Pin    GPIO_Pin_13   
 

//#define KEY_Port (GPIOB) //定义端口
#define KEY_Port (GPIOA) //定义端口#define KEY_Port (GPIOB) //定义端口

//使用位操作定义
//#define KEY1 	PBin(10)
//#define KEY2 	PBin(9)

#define KEY1 	PAin(12)
#define KEY2 	PAin(13)


//定义各个按键值  
#define KEY1_VALUE 	1
#define KEY2_VALUE 	2
  



void KEY_Init(void);
u8 KEY_Scan(u8 mode);
#endif
