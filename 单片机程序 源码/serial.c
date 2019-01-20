#include <reg52.h>

#define uchar unsigned char

// FOSC/32*(65535-RCAP2)

uchar pos = 0, recv[10];

extern bit Player_Playing;

void Serial_InitWithT2() {
	PCON &= 0x7F; //SMOD=0
	SCON = 0x50; //方式1 允许接收				SM0=0 SM1=1 SM2=0 REN=1
	T2CON = 0x34;
	RCAP2H = 0xFF;
	RCAP2L = 0xFF - 80;
	TH2 = RCAP2H;
	TL2 = RCAP2L;
	TR2 = 1;
	EA = 1;
	ES = 1;
}

void Serial_Send(uchar dat) {
	SBUF = dat;
	while (!TI);
	TI = 0;
}

/*void Serial_Send_Str(uchar dat[]) {
	uchar i;
	for (i = 0; dat[i] != '\0'; i++) {
		Serial_Send(dat[i]);
	}
}*/

void serial() interrupt 4 {
	if (RI) {
		recv[pos++] = SBUF;
		RI = 0;
		if (pos == 10)
			pos = 0;
		if (recv[3] == 0x3D) // 播放完成
			Player_Playing = 0;
	}
}