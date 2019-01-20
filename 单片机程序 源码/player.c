#include <STC89C5xRC.H>
#include "12864.h"
#include "delay.h"
#include "serial.h"
#include "max7219.h"
#include "sdcard.h"
#include "fs.h"

#define uchar unsigned char
#define ulong unsigned long int
	
#define TH 0xF1
#define TL 0xC8  // 调速度  F1Cx  后4位不确定

sbit BTN_D = P1^6;
sbit BTN_C = P1^4;

extern ulong FILE_BadApple_StartClust;
extern uchar FS_SectorPerClust;
extern uchar FS_SectorPerClust;
extern ulong FS_FatUseSector;
extern ulong FS_FatSector;

bit Player_Playing;
bit Player_WaitNextFrame;
uchar frame[32];
uchar timer = 0;

void waitC() {
	while (!BTN_C);
	delay(100);
}

void waitD() {
	while (!BTN_D);
	delay(100);
}

void Timer1Init(void)
{
	TMOD |= 0x10;
	TH1 = TH;
	TL1 = TL;
	ET1 = 1;
	TR1 = 1;
}

void Player_Test() {
	uchar i, j;
	LCD12864_Clear();
	LCD12864_WriteText("音频测试中...");
	Player_Playing = 1;
	Serial_Send(0x7E);
	Serial_Send(0xFF);
	Serial_Send(0x06);
	Serial_Send(0x03);
	Serial_Send(0x00);
	Serial_Send(0x00);
	Serial_Send(0x03);
	Serial_Send(0xFE);
	Serial_Send(0xF5);
	Serial_Send(0xEF);
	while (Player_Playing);
	LCD12864_Positon(1, 0);
	LCD12864_WriteText("按Ｃ键继续...");
	waitC();
	LCD12864_Positon(2, 0);
	LCD12864_WriteText("图像测试中...");
	LCD12864_Positon(3, 0);
	j = 0x80;
	for (i = 0; i < 8; i++) {
		MAX7219_WriteAll(1, j);
		MAX7219_WriteAll(2, j);
		MAX7219_WriteAll(3, j);
		MAX7219_WriteAll(4, j);
		MAX7219_WriteAll(5, j);
		MAX7219_WriteAll(6, j);
		MAX7219_WriteAll(7, j);
		MAX7219_WriteAll(8, j);
		j >>= 1;
		delay(100);
	}
	for (i = 0; i < 8; i++) {
		MAX7219_WriteAll(1, i == 0 ? 0xFF : 0x00);
		MAX7219_WriteAll(2, i == 1 ? 0xFF : 0x00);
		MAX7219_WriteAll(3, i == 2 ? 0xFF : 0x00);
		MAX7219_WriteAll(4, i == 3 ? 0xFF : 0x00);
		MAX7219_WriteAll(5, i == 4 ? 0xFF : 0x00);
		MAX7219_WriteAll(6, i == 5 ? 0xFF : 0x00);
		MAX7219_WriteAll(7, i == 6 ? 0xFF : 0x00);
		MAX7219_WriteAll(8, i == 7 ? 0xFF : 0x00);
		delay(100);
	}
	MAX7219_WriteAll(8, 0);
	LCD12864_WriteText("按Ｄ键继续...");
	waitD();
}

void Player_ReDisplay() {
	uchar i;
	for (i = 0; i < 8; i++)
		MAX7219_Write(i + 1, frame[i], frame[i + 8], frame[i + 16], frame[i + 24]);
	Player_WaitNextFrame = 1;
}

void Player_ReceiveFrame() {
	uchar i;
	for (i = 0; i < 32; i++) {
		frame[i] = SD_Data_Read();
	}
}

void Player_Play() {
	ulong nextClust = FILE_BadApple_StartClust;
	ulong rootSector = FS_FatSector + FS_FatUseSector + FS_FatUseSector;
	uchar clustSector;
	uchar i;
	
	ET1 = 1;
	Timer1Init();
	SD_Start();
	Serial_Send(0x7E);
	Serial_Send(0xFF);
	Serial_Send(0x06);
	Serial_Send(0x03);
	Serial_Send(0x00);
	Serial_Send(0x00);
	Serial_Send(0x02);
	Serial_Send(0xFE);
	Serial_Send(0xF6);
	Serial_Send(0xEF);
	for (nextClust = FILE_BadApple_StartClust; nextClust != 0x0FFFFFFF; nextClust = FS_GetNextClust(nextClust)) { // 所有簇
		for (clustSector = 0; clustSector < FS_SectorPerClust; clustSector++) { // 所有扇区
			SD_StartReadBlock(rootSector + ((nextClust - 2) * FS_SectorPerClust) + clustSector);
			for (i = 0; i < 16; i++) { // 32*16=512  一个扇区有16帧
				Player_ReceiveFrame();
				Player_ReDisplay();
				while (Player_WaitNextFrame);
			}
			SD_Data_Read();
			SD_Data_Read();
		}
	}
	TR1 = 0;
	SD_Stop();  // 不知道哪的问题  好像到不了这
	LCD12864_Positon(3, 0);
	LCD12864_WriteText("    播放完成    ");
}

void Timer1() interrupt 3 {
	TH1 = TH;
	TL1 = TL;
	if (timer++ > 20) {
		timer = 0;
		Player_WaitNextFrame = 0;
	}
}
