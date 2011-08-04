
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/interrupt.h>

#include "PWM.h"

#define SERVO_FREQUENCY 50
#define MAX_COUNT F_CPU/(50*8)
#define SERVO_COUNT 1

int *ocr_values;

/**
* Функция инициялизирует ШИМ для серво приводов.
*/
void initPWM()
{
	ocr_values = malloc(SERVO_COUNT);

	// Установим дефолтные значения, серво машинки будут находится в среднем 
	// положении.
	for (int i=0; i<SERVO_COUNT; i++)
		ocr_values[i] = (MAX_COUNT/200)*15;


	DDRA |= 1;

	// Настройки счетчика Т1.
	TCCR1A = 0;
	// Установим делитель счетчика на 8 тактов.
	TCCR1B = (0<<CS10) | (1<<CS11) | (0<<CS12);
	
	OCR1AH = (unsigned char)(MAX_COUNT>>8);
	OCR1AL = (unsigned char)MAX_COUNT; 

	// Настроки прерываний счетчиков, разрешаем прерывания.
	TIMSK |= (1<<TOIE1) | (1<<OCIE1A);
}


/**
* Закрытие управления серво двигателями.
*/
void closePWM()
{
	// Сброс всех регистров.
	TIMSK = 0;
	TCCR1B = 0;
	TCCR1B = 0;

	free(ocr_values);
}

/**
* Функция устанавливает серво в нужное положение.
*/ 
void set_servo(int servo_number, int value)
{
	if (servo_number <= SERVO_COUNT)
	{
		cli();
		ocr_values[servo_number] = value;
		sei();
	}
}

/**
* Увеличивает угол сервы на указанное значение.
*/
void inc_servo(int servo_number, int value)
{
	if (servo_number <= SERVO_COUNT)
	{
		cli();
		ocr_values[servo_number] += value;
		sei();
	}
}

/**
* Уменьшает угол сервы на указанное значение.
*/
void dec_servo(int servo_number, int value)
{
	if (servo_number <= SERVO_COUNT)
	{
		cli();
		ocr_values[servo_number] -= value;
		sei();
	}
}

/**
* Возвращает значение для сервомашинки.
*/
int get_servo(int servo_number)
{
	if (servo_number <= SERVO_COUNT)
	{
		cli();
		int res = ocr_values[servo_number];
		sei();
		return res;
	}
	else
		return 0;
}

/**
* Перехват прерываний на совпадение.
*/
ISR (TIMER1_COMPA_vect)		
{
	int counter = (TCNT1H<<8) | (TCNT1L);
	int port_value = 0;
	int next_ocr = MAX_COUNT;

	if (counter >= MAX_COUNT)
	{
		TCNT1H = 0;
		TCNT1L = 0;
		counter = 0;
	}

	for (int i=0; i<SERVO_COUNT; i++)
	{
		if (ocr_values[i] > 0 && ocr_values[i] > counter)
		{
			port_value |= (1<<i);
			if (next_ocr > ocr_values[i])
				next_ocr = ocr_values[i];
		}
	}

	OCR1AH = (unsigned char)(next_ocr>>8);
	OCR1AL = (unsigned char)next_ocr; 
	PORTA = port_value; 
}

/**
* Перехват прерываний на переполнение таймера.
*/
ISR (SIG_OVERFLOW1)		
{
	printf("*\n");
}
