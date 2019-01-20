#include <reg52.h>
#include <intrins.h>
#include "delay.h"
#include "serial.h"
#include "sdcard.h"

#define uchar unsigned char
#define ulong unsigned long int

sbit MOSI = P2^1;
sbit MISO = P2^3;
sbit CS = P2^0;
sbit CLK = P2^2;

#define WAIT_COUNT 			30

struct SD_INFO SD_Info;


void SD_Data_Send(uchar dat) {
	uchar i;
	for (i = 0; i < 8; i++) {
		MOSI = dat & 0x80;
		dat <<= 1;
		CLK = 1;
		CLK = 0;
	}
}

uchar SD_Data_Read() {
	uchar i, tmp = 0;
	for (i = 0; i < 8; i++) {
		tmp <<= 1;
		CLK = 1;
		if (MISO)
			tmp |= 1;
		CLK = 0;
	}
	return tmp;
}

uchar SD_Command(uchar cmd, uchar arg0, uchar arg1, uchar arg2, uchar arg3, uchar crc) {
	uchar i = WAIT_COUNT, tmp;
	SD_Data_Send(cmd);
	SD_Data_Send(arg0);
	SD_Data_Send(arg1);
	SD_Data_Send(arg2);
	SD_Data_Send(arg3);
	SD_Data_Send(crc);
	SD_Data_Read();
	do {
		tmp = SD_Data_Read();
	} while (i-- && tmp == 0xFF); // 正常情况下最高位永远是0?
	return tmp;
}

void SD_Start() {
	CS = 1;
	SD_Data_Send(0xFF);
	CS = 0;
}

void SD_Stop() {
	CS = 1;
}

bit SD_Init() {
	uchar i, tmp;
	
	CLK = 1;
	CS = 1;
	MOSI = 1;
	MISO = 1;
	SD_Info.SD_Type = UNINITIALIZED;
	
	// 初始化时钟
	for (i = 0; i < 120; i++) {
		_nop_();
		_nop_();
		CLK = 0;
		_nop_();
		_nop_();
		CLK = 1;
	}
	CLK = 0;
	
	// CMD0 让卡复位
	Delay100us();
	SD_Start();
	tmp = SD_Command(0x40, 0x00, 0x00, 0x00, 0x00, 0x95); // CMD0
	SD_Stop();
	if (tmp != 0x01) {
		return 0;
	}
	
	// CMD8 如果正常响应为SD2.0  错误为SD1.0
	Delay100us();
	SD_Start();
	tmp = SD_Command(0x48, 0x00, 0x00, 0x01, 0xAA, 0x87); // CMD8
	SD_Data_Read();
	SD_Data_Read();
	SD_Data_Read();//cmd8返回是R7 后边还有4个字节
	SD_Data_Read();
	SD_Stop();
	if(tmp == 0x01) {
		// 是2.0
		SD_Info.SD_Type = SD_2;
		// 初始化
		i = WAIT_COUNT;
		do {
			Delay100us();
			SD_Start();
			SD_Command(0x77, 0x00, 0x00, 0x00, 0x00, 0xFF); // CMD55
			tmp = SD_Command(0x69, 0x40, 0x00, 0x00, 0x00, 0xFF); // ACMD41
			SD_Stop();
		} while ((tmp & 0x01) && i--);
		if (!i)
			return 0;
		
		// 检查是不是SDHC
		Delay100us();
		SD_Start();
		SD_Command(0x7A, 0x00, 0x00, 0x00, 0x00, 0xFF); // CMD58
		tmp = SD_Data_Read();
		SD_Data_Read();
		SD_Data_Read();
		SD_Data_Read();
		SD_Stop();
		if (tmp & 0x40) {
			SD_Info.SD_Type = SD_HC;
		}
		return 0; //不知道为什么2.0卡不能用
	} else if (tmp == 0x05) {
		// 检查是1.0还是MMC
		Delay100us();
		SD_Start();
		i = WAIT_COUNT;
		do {
			SD_Command(0x77, 0x00, 0x00, 0x00, 0x00, 0xFF); // CMD55
			tmp = SD_Command(0x69, 0x00, 0x00, 0x00, 0x00, 0xFF); // ACMD41
		} while (i-- && (tmp & 0x01)); //好像最后一位是0就可以了
		if (i) { // 重试次数内完成
			SD_Stop();
			SD_Info.SD_Type = SD_1;
			return 1; //成功
		} else { // 看看是不是MMC
			tmp = SD_Command(0x41, 0x00, 0x00, 0x00, 0x00, 0xFF); // CMD1
			if (!tmp) {  // 这到底是0还是是算成功...
				SD_Info.SD_Type = SD_MMC;
				return 1;
			} else {
				return 0;
			}
		}
	} else {
		return 0;
	}
}

/**
	开始读 扇区地址 0才是成功 之后手动读512字节内容和2字节crc 
*/
bit SD_StartReadBlock(ulong address) {
	uchar tmp, i;
	if (SD_Info.SD_Type != SD_HC) {
		address <<= 9; // *512
	}
	i = WAIT_COUNT;
	do {
		tmp = SD_Command(0x51, (address >> 24) & 0xFF, (address >> 16) & 0xFF, (address >> 8) & 0xFF, address & 0xFF, 0xFF); //CMD17
	} while (i-- && tmp);
	if (tmp == 0) {
		i = WAIT_COUNT;
		do
			tmp = SD_Data_Read();
		while (i-- && tmp != 0xFE);
		if (i)
			return 1;
		return 0;
	}
	return 0;
}
