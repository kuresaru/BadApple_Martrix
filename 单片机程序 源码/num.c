#include "num.h"

#define uchar unsigned char
#define uint unsigned int
#define ulong unsigned long int

// 把ulong转换成8个Hex ASCII码
uchar * ul2hstr(ulong ul) {
	uchar str[8];
	uchar i, j;
	for (i = 0; i < 8; i++) {
		j = ((ul >> (28 - i * 4)) & 0xF) + 0x30;
		if (j > 0x39)
			j += 7;
		str[i] = j;
	}
	return str;
}

// 把uchar转换成3个Dec ASCII码
uchar * uc2dstr(uchar uc) {
	uchar str[3];
	str[0] = uc / 100 + 0x30;
	str[1] = uc % 100 / 10 + 0x30;
	str[2] = uc % 10 + 0x30;
	return str;
}

// 把uint转换成5个Dec ASCII码
uchar * ui2dstr(uint ui) {
	uchar str[5];
	str[0] = ui / 10000 + 0x30;
	str[1] = ui % 10000 / 1000 + 0x30;
	str[2] = ui % 1000 / 100 + 0x30;
	str[3] = ui % 100 / 10 + 0x30;
	str[4] = ui % 10 + 0x30;
	return str;
}