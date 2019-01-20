#include <intrins.h>

void Delay100us()		//@24.576MHz
{
	unsigned char i;

	_nop_();
	i = 99;
	while (--i);
}

void delay(unsigned int ms)
{
	while (ms--) {
		Delay100us();
		Delay100us();
		Delay100us();
		Delay100us();
		Delay100us();
		Delay100us();
		Delay100us();
		Delay100us();
		Delay100us();
		Delay100us();
	}
}
