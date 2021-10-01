#include<reg51.h>

// set initial year, month, day, hour, minute, second
// set initial time 2001.5.21 5:21:00
unsigned char data year = 0;
unsigned char data month = 2;
unsigned char data day = 28;
unsigned char data hour = 23;
unsigned char data minute = 59;
unsigned char data second = 0;
unsigned char data ms_50 = 0;

// days of month table
unsigned char xdata Date[2][12] =  {
	{31,28,31,30,31,30,31,31,30,31,30,31},
	{31,29,31,30,31,30,31,31,30,31,30,31}
};
	

void Set(void)
{
	EA = 1;
	ET0 = 1;
	TMOD = 0x01;
	TH0 = 0x3C;
	TL0 = 0xB0;
	TR0 = 1;
	TF0 = 0;
}

// Determine whether it is a leap year, 
// if it is true, return 1, otherwise return 0
bit JudgeDays(void)
{
	unsigned int Year = 2000 + year;
	bit flag = 0;
	if( (Year % 400) == 0 || (Year % 4 ==0 && Year % 100 != 0 ))
		flag = 1;
	return flag;
}

// interrupt program
void timer0(void) interrupt 1
{
	// reset 
	TH0 = 0x3C;
	TL0 = 0xB0;
	TF0 = 0;
	
	// Trigger interrupt, indicating overflow, it is 50ms
	ms_50++;
	if( ms_50 == 20 )
	{
		ms_50 = 0;
		second++;
	}
	
	// minute++;
	if( second == 60 )
	{
		second = 0;
		minute++;
	}
	
	// hour++;
	if( minute == 60 )
	{
		minute = 0;
		hour++;
	}
	
	// day++;
	if( hour == 24 )
	{
		hour = 0;
		day++;
	}
	
	// month++;
	if( day == Date[JudgeDays()][month-1]+1 )
	{
		day = 1;
		month++;
	}
	
	// year++;
	if( month == 13 )
	{
		month = 1;
		year++;
	}
}

// Display time as decimal
unsigned char Decimal(unsigned char hex)
{
	return 16*(hex/10)+hex%10;
}
	
void main(void)
{
	// Show time at 0x50
	code unsigned char* show_place = 0x50;	
	// Show the year as 2000+
	show_place[0] = Decimal(20);
	
	Set();
	
	while(1){
		show_place[1] = Decimal(year);
		show_place[2] = Decimal(month);
		show_place[3] = Decimal(day);
		show_place[4] = Decimal(hour);
		show_place[5] = Decimal(minute);
		show_place[6] = Decimal(second);
	};	
}