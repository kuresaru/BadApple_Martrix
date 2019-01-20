#include <reg52.h>
#include <intrins.h>
#include "delay.h"

#define uchar unsigned char
#define writeNop writeData(0, 0)

sbit CS = P2^6;
sbit DATA = P2^7;
sbit CLK = P2^5;

void MAX7219_WriteData(uchar addr, uchar dat) {
	uchar i;
	for (i = 0; i < 8; i++) {
		CLK = 0;
		DATA = (bit) (addr & 0x80);
		addr <<= 1;
		CLK = 1;
	}
	for (i = 0; i < 8; i++) {
		CLK = 0;
		DATA = (bit) (dat & 0x80);
		dat <<= 1;
		CLK = 1;
	}
}

void MAX7219_Write(uchar addr, uchar dat0, uchar dat1, uchar dat2, uchar dat3) {
	CS = 0;
	MAX7219_WriteData(addr, dat3);
	MAX7219_WriteData(addr, dat2);
	MAX7219_WriteData(addr, dat1);
	MAX7219_WriteData(addr, dat0);
	CS = 1;
}

void MAX7219_WriteAll(uchar addr, uchar dat) {
	MAX7219_Write(addr, dat, dat, dat, dat);
}

void MAX7219_Init() {
	MAX7219_WriteAll(0x0C, 0x00); // 关显示
	_nop_();
	_nop_();
	_nop_();
	MAX7219_WriteAll(0x0A, 0x03); // 亮度
	MAX7219_WriteAll(0x09, 0x00); // 正常显示模式
	MAX7219_WriteAll(0x01, 0x00); // 清屏
	MAX7219_WriteAll(0x02, 0x00);
	MAX7219_WriteAll(0x03, 0x00);
	MAX7219_WriteAll(0x04, 0x00);
	MAX7219_WriteAll(0x05, 0x00);
	MAX7219_WriteAll(0x06, 0x00);
	MAX7219_WriteAll(0x07, 0x00);
	MAX7219_WriteAll(0x08, 0x00);
	MAX7219_WriteAll(0x0C, 0x01); // 启动
	MAX7219_WriteAll(0x0B, 0x07); // 扫描限制
}
