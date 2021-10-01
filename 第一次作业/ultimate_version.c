#include "reg52.h"	

typedef unsigned int u16;
typedef unsigned char u8;

#define KEY1 1	// Enter setting mode
#define KEY2 2	// increase
#define KEY3 3	// decrease
#define KEY4 4	// End setting mode
#define KEY5 5 	// reset high
#define KEY6 6 	// reset low

#define FAST_SPEED 10 // 100ms
#define LOW_SPEED 20 // 200ms

#define L_Ones 0
#define L_Ten 2
#define L_Hundreds 0

#define H_Ones 0
#define H_Ten 5
#define H_Hundreds 2

sbit k1 = P1^0;		// Port button
sbit k2 = P1^1;
sbit k3 = P1^2;
sbit k4 = P1^3;	
sbit k5 = P1^4;
sbit k6 = P1^5;	

sbit ShowOnes = P2^2;
sbit ShowTen = P2^3;
sbit ShowHundreds = P2^4;

u8 code smgduan[17]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,
					0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71};
u8 ShowData[3] = { L_Hundreds, L_Ten, L_Ones };

u16 cnt = 0;
u8 Data = 20;
u8 velocity = LOW_SPEED;

// increase
void increase()
{
    u8 temp;
    if(ShowData[0] == H_Hundreds && ShowData[1] == H_Ten && ShowData[2] == H_Ones ){
        DigDisplay();
        return;
    }
    temp = ShowData[0] * 100 + ShowData[1] + ShowData[2];
    temp++;
    
    ShowData[0] = temp / 100;
    ShowData[1] = (temp / 10) % 10;
    ShowData[2] = temp % 10;
    DigDispaly();
    return ;
}

// decrease
void decrease()
{
    u8 temp;
    if(ShowData[0] == H_Hundreds && ShowData[1] == H_Ten && ShowData[2] == H_Ones ){
        DigDisplay();
        return;
    }
    temp = ShowData[0] * 100 + ShowData[1] + ShowData[2];
    temp--;
    
    ShowData[0] = temp / 100;
    ShowData[1] = (temp / 10) % 10;
    ShowData[2] = temp % 10;
    DigDispaly();
    return ;
}

// Reset to minimum
void setLow()
{
	ShowData[0] = L_Hundreds;
	ShowData[1] = L_Ten;
	ShowData[2] = L_Ones;
	DigDisplay();
}

void setHigh()
{
	ShowData[0] = H_Hundreds;
	ShowData[1] = H_Ten;
	ShowData[2] = H_Ones;
	DigDisplay();
}

void recordData()
{
	Data = ShowData[0] * 100 + ShowData[1] * 10 + ShowData[2];
}

void showData()
{
	ShowData[0] = Data / 100;
	ShowData[1] = ( Data / 10 ) % 10;
	ShowData[2] = Data % 10;
	DigDisplay();
}

void changeData()
{
	u8 key;
	key = keyScan();
	if( key == KEY1 ){
		recordData();
		TR0 = 1;
	}else if( key == KEY2 ){
		increase();
		TR0 = 1;
	}else if( key == KEY3 ){
		decrease();
		TR0 = 1;
	}else if( key == KEY4 ){
		
		