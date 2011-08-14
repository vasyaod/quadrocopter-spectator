#ifndef F_CPU
#define F_CPU 8000000UL  // указываем частоту в герцах
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <string.h>

#include "UART.h"
#include "PWM.h"
#include "I2C.h"
#include "SPISlave.h"

ISR(__vector_default)
{

}

void out_pin(u08 servo, u08 value)
{
	if (servo == 0)
	{
		if (value == 0)
			PORTA &=~(1<<0);
		else
			PORTA |= (1<<0);
	}	
}

u08 in_pin(u08 servo)
{
	if (servo == 0)
		if ((PIND&(1<<7)) > 0 )
			return 1;
	
	return 0;
}

int main(void)
{
	int f = 1;
	// Врубаем прерывания.
	sei();

	// Инициализируем UART.
	initUART();
	// Через некоторое время инициализируем ШИМ сервоприводов.
	_delay_ms(100);
	printf("Starting pwm.\n");
	
	DDRA |= 1;
	DDRD &=~(1<<7);
//	PORTD |= (1<<7);

	initPWM(&out_pin, &in_pin);

	//_delay_ms(100);
	//printf("Starting SPI slave.\n");
	//SPISlaveInit();
	
//	_delay_ms(100);
//	printf("Starting i2c.\n");
//	i2c_InBuff[0] = 5;
//	i2c_OutBuff[0] = 6;
//	Init_Slave_i2c();

	printf("Spectator started.\n");

	// 
	DDRB |= 1;
	PORTB = 0x01;
	while (1)
	{
		checkPins();

		if (loop_counter > 50)
		{
			loop_counter = 0;
		//	set_out_value(0, get_in_value(0));
			printf("Led on.\n");
			int k = get_in_value(0);
			printf("S0: %d\n", k);
			if (f == 1)
			{
				PORTB = 0x00;
				f = 0;
			}
			else
			{
				PORTB = 0x01;
				f = 1;
			}
		}
		
/*
		if (f == 1)
		{
			printf("Led on.\n");
//			printf("S0: %d\n",get_in_value(0));
			printf("S0: %d\n",in_pin(0));
			PORTB = 0x00;
			f = 0;
		}
		else
		{
			printf("Led off.\n");
			PORTB = 0x01;
			f = 1;
		}

		_delay_ms(1000);
*/

	}
	return 0;
}

void executed_comand(char *command, int len)
{
	if (strcmp(command, "+") == 0)
	{
//		inc_servo(0, 100);
//		printf("Increment servo. Value: %d \n", get_servo(0));
	}

	if (strcmp(command, "-") == 0)
	{
//		dec_servo(0, 100);
//		printf("Decremrnt servo. Value: %d \n", get_servo(0));
	}

}
