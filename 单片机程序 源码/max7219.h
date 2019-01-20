#define uchar unsigned char

void MAX7219_WriteData(uchar addr, uchar dat);
void MAX7219_Write(uchar addr, uchar dat0, uchar dat1, uchar dat2, uchar dat3);
void MAX7219_WriteAll(uchar addr, uchar dat);
void MAX7219_Init(void);
