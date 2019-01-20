#define uchar unsigned char
#define uint  unsigned int

bit LCD12864_Busy(void);
void LCD12864_WriteCmd(unsigned char cmd);
void LCD12864_WriteData(uchar dat);
void LCD12864_Init(void);
void LCD12864_Positon(unsigned char x,unsigned char y);
void LCD12864_WriteText(uchar text[]);
void LCD12864_Clear(void);
