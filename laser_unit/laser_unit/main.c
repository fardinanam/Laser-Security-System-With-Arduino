#include <avr/io.h>

#define F_CPU 1000000 // Clock Frequency
#include <util/delay.h>

int main(void)
{
	DDRA = 0b00000011;
	char laser = 1;
	while (1)
	{
		PORTA = laser | (laser << 1);
		_delay_ms(200);
		laser = !laser;
	}
}