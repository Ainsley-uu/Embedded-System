#include "reg52.h"

typedef unsigned int u16;
typedef unsigned char u8;

sbit LSA = P2^2;
sbit LSB = P2^3;
sbit LSC = P2^4;
sbit k1 = P1^0;
sbit led = P0^0;

u8 code smgduan[17]={0x3f, 0x06, 0x5b, 0x4f, 0x66,
	0x6d, 0x7d, 0x07, 0x7f, 0x6f,
	0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71};

void delay(u16 i)
{
	while(i--);
}

void keypros()
{
	if( k1 == 0 )		
	{
		delay(1000);
		if(k1 == 0);
		{
			led = ~led;
		}
		while(!k1);
	}
}

void DigDisplay()
{
	u8 i;
	for( i=0 ; i<8 ; i++ )
	{
		switch(i)
		{
			case(0):
				LSA = 0; LSB = 0; LSC = 0; break;
			case(1):
				LSA = 1; LSB = 0; LSC = 0; break;
			case(2):
				LSA = 0; LSB = 1; LSC = 0; break;
			case(3):
				LSA = 1; LSB = 1; LSC = 0; break;
			case(4):
				LSA = 0; LSB = 0; LSC = 1; break;
			case(5):
				LSA = 1; LSB = 0; LSC = 1; break;
			case(6):
				LSA = 0; LSB = 1; LSC = 1; break;
			case(7):
				LSA = 1; LSB = 1; LSC = 1; break;
		}
		P0 = smgduan[i];
		delay(100);
		P0 = 0x00;
	}
}

void main()
{
	led = 1;
	while(1)
	{
		keypros();
		DigDisplay();
	}
}



