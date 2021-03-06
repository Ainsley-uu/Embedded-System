#include "reg52.h"

typedef unsigned int u16;
typedef unsigned char u8;

#define GPIO_DIG P0
#define GPIO_KEY P1

u8 KeyValue;	

u8 code smgduan[17] = {0x3f, 0x06, 0x5b, 0x4f, 0x66,
	0x6d, 0x7d, 0x07, 0x7f, 0x6f,
	0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71};

void delay(u16 i)
{
	while(i--);
}

void KeyDown(void)
{
	char a = 0;
	GPIO_KEY = 0x0f;
	if( GPIO_KEY != 0x0f )
	{
		delay(1000);
		if( GPIO_KEY != 0x0f )
		{
			GPIO_KEY = 0X0F;
			switch(GPIO_KEY)
			{
				case(0X07): KeyValue = 0 ; break;
				case(0X0b): KeyValue = 1 ; break;
				case(0X0d): KeyValue = 2 ; break;
				case(0X0e):	KeyValue = 3 ; break;
			}
			GPIO_KEY =0XF0;
			switch(GPIO_KEY)
			{
				case(0X70): KeyValue = KeyValue ; break;
				case(0Xb0): KeyValue = KeyValue+4 ; break;
				case(0Xd0): KeyValue = KeyValue+8 ; break;
				case(0Xe0):	KeyValue = KeyValue+12 ; break;
			}
		}
	}
	while((a<50)&&(GPIO_KEY != 0xf0))
	{
		delay(100);
		a++;
	}
}


void main()
{
	while(1)
	{
		KeyDown();
		GPIO_DIG = ~smgduan[KeyValue];
	}
}
