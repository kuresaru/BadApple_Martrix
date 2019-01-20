#include <STC89C5xRC.H>
#include "serial.h"
#include "sdcard.h"
#include "12864.h"
#include "delay.h"
#include "num.h"
#include "fs.h"
#include "max7219.h"
#include "player.h"

#define uchar unsigned char
#define uint unsigned int
#define ulong unsigned long int
	
sbit BTN_B = P1^7;
sbit BTN_A = P1^5;

extern struct SD_INFO SD_Info; 		// SD����Ϣ
extern ulong FILE_BadApple_StartClust;	

// �ȴ�A������
void waitA() {
	while (!BTN_A);
	delay(100);
}

void waitB() {
	while (!BTN_B);
	delay(100);
}

void Init_CheckSD() {
	while (1) {
		SD_Init();
		if (SD_Info.SD_Type == SD_1) {
			LCD12864_Positon(0, 0);
			LCD12864_WriteText("    SD������    ");
			break;
		} else if (SD_Info.SD_Type == UNINITIALIZED) {
			LCD12864_Positon(0, 0);
			LCD12864_WriteText("SD��������      ");
		} else {
			LCD12864_Positon(0, 0);
			LCD12864_WriteText("SD����֧��      ");
		}
		Delay100us();
		LCD12864_Positon(3, 0);
		LCD12864_WriteText("����������...");
		waitA();
		LCD12864_Positon(3, 0);
		LCD12864_WriteText("                ");
	}
}

void main() {
	delay(200);
	Serial_InitWithT2();
	MAX7219_Init();
	LCD12864_Init();
	LCD12864_WriteText("Loading...");
	delay(700);
	Init_CheckSD();
	FS_Init();
	FS_FindBadAppleBin();
	if (!FILE_BadApple_StartClust)
		while (1); // ���û���ҵ�badapple.bin ֹͣ����
	LCD12864_Positon(3, 0);
	LCD12864_WriteText("����������...");
	waitA();
	Player_Test();
	LCD12864_Clear();
	LCD12864_WriteText("  Bad Apple!!");
	LCD12864_Positon(1, 0);
	LCD12864_WriteText("ĺ��СԳwzt ����");
	LCD12864_Positon(2, 0);
	LCD12864_WriteText("��������    �ɱ�");
	LCD12864_Positon(3, 0);
	LCD12864_WriteText("���¼�����");
	waitB();
	LCD12864_Positon(3, 0);
	LCD12864_WriteText("������    ������");
	Player_Play();
	while (1);
}
