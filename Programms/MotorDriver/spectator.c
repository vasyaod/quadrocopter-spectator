#ifndef F_CPU
#define F_CPU 8000000UL  // указываем частоту в герцах
#endif

#include <avr/io.h>
//#include <stdio.h>
//#include <stdlib.h>
#include <avr/interrupt.h>
//#include <string.h>

#include "UART.h"
#include "PWM.h"
#include "I2C.h"
#include "SPISlave.h"

#define SERVO_COUNT 2

ISR(__vector_default)
{

}

void out_pin(u08 servo, u08 value)
{
	if (servo == 0)
	{
		if (value == 0)
			PORTD &=~(1<<PD4);
		else
			PORTD |= (1<<PD4);
	}	
}

u08 in_pin(u08 servo)
{
    switch(servo) 
    {
  		case 0:
 			return (PINB>>PB6)&1;
  		case 1:
			return (~(PINB>>PB7))&1;
	}
	return 0;
}

void i2c_write(u08 address, u08 data)
{
	if (address < 0)
		return;
	if (address >= SERVO_COUNT*sizeof(u16))
		return;
	
	u08 *_out_values = (u08*)get_out_values();
    _out_values[address] = data;
}

u08 i2c_read(u08 address)
{

	if (address < 0)
		return 0;
	if (address >= SERVO_COUNT*sizeof(u16))
		return 0;

	u08 *_in_values = (u08*)get_in_values();
	return _in_values[address];
}

int main(void)
{
	int f = 1;
	// Врубаем прерывания.
	sei();

	// Инициализируем UART.
//	initUART();
	// Через некоторое время инициализируем ШИМ сервоприводов.
	
	// Установка выходных портов ШИМ.
	DDRD |= (1<<PD4);

	// Установка входных портов ШИМ.
	DDRB &=~(1<<PB6);
	DDRB &=~(1<<PB7);
	PORTB |= (1<<PB7);

	initPWM(SERVO_COUNT, &out_pin, &in_pin);

	//_delay_ms(100);
	//printf("Starting SPI slave.\n");
	//SPISlaveInit();
	
	// Запустим на 0x19 адресе, не будем воспрнимать широковещательные пакеты.
	i2c_init_slave(0x19, 0, &i2c_write, &i2c_read);

//	printf("Started.\n");
	// 
	DDRC |= (1<<PC2);
	PORTC |= (1<<PC2);
	while (1)
	{
		cli();
		int counter_value = (TCNT1L) | (TCNT1H<<8);
		if (counter_value >= 20000)
		{
			TCNT1H = 0;
			TCNT1L = 0;
			counter_value = 0;
		}
		sei();


		pwm_check_in(counter_value);
		pwm_check_out(counter_value);

		if (loop_counter > 50)
		{
			loop_counter = 0;
//			printf("Led on.\n");
//			printf("S0: %d\n", get_in_value(0));
			if (f == 1)
			{
				PORTC &=~(1<<PC2);
				f = 0;
			}
			else
			{
				PORTC |= (1<<PC2);
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

//void executed_comand(char *command, int len)
//{
//	if (strcmp(command, "+") == 0)
//	{
//		inc_servo(0, 100);
//		printf("Increment servo. Value: %d \n", get_servo(0));
//	}

//	if (strcmp(command, "-") == 0)
//	{
//		dec_servo(0, 100);
//		printf("Decremrnt servo. Value: %d \n", get_servo(0));
//	}

//}
