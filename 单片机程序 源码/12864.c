#include <reg52.h>
//#include <intrins.h>	
//#include "delay.h"

#define uchar unsigned char
#define uint  unsigned int
	
#define LCD_data  P0     

sbit LCD_RS  =  P3^5;
sbit LCD_RW  =  P3^6;
sbit LCD_EN  =  P3^4;
sbit LCD_PSB =  P3^7;

bit LCD12864_Busy()
 {                          
    bit result;
    LCD_RS = 0;
    LCD_RW = 1;
    LCD_EN = 1;
    //delay(1);
    result = (bit)(P0&0x80);
    LCD_EN = 0;
    return(result); 
 }

void LCD12864_WriteCmd(uchar cmd)
{                          
   while(LCD12864_Busy());
    LCD_RS = 0;
    LCD_RW = 0;
    LCD_EN = 0;
    //delay(1);
    P0 = cmd;
    //delay(1);
    LCD_EN = 1;
    //delay(1);
    LCD_EN = 0;  
}

void LCD12864_WriteData(uchar dat)
{                          
   while(LCD12864_Busy());
    LCD_RS = 1;
    LCD_RW = 0;
    LCD_EN = 0;
    P0 = dat;
    //delay(1);
    LCD_EN = 1;
    //delay(1);
    LCD_EN = 0; 
}

void LCD12864_Positon(uchar x,uchar y)	//确定显示位置
{                          
   uchar  pos;
   if (x==0)
     {x=0x80;}
   else if (x==1)
     {x=0x90;}
   else if (x==2)
     {x=0x88;}
   else if (x==3)
     {x=0x98;}
   pos = x+y ;  
   LCD12864_WriteCmd(pos);     //显示地址
}

void LCD12864_Clear() {
	LCD12864_WriteCmd(0x01);      //清除LCD的显示内容
	//delay(5); 
	LCD12864_Positon(0, 0);
}

void LCD12864_Init()
{ 
//
    LCD_PSB = 1;         //并口方式
		LCD12864_WriteCmd(0x34);      //扩充指令操作
    //delay(5);       
    LCD12864_WriteCmd(0x30);      //基本指令操作
    //delay(5);	
    LCD12864_WriteCmd(0x0C);      //显示开，关光标
    //delay(5);
    LCD12864_Clear();
}

void LCD12864_WriteText(uchar text[]) {
	uchar i;
	for (i = 0; text[i] != '\0'; i++) {
		LCD12864_WriteData(text[i]);
		//delay(1);
	}
}
