
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
* ������� �������������� ��� ��� ����� ��������.
*/
void initPWM()
{
	ocr_values = malloc(SERVO_COUNT);

	// ��������� ��������� ��������, ����� ������� ����� ��������� � ������� 
	// ���������.
	for (int i=0; i<SERVO_COUNT; i++)
		ocr_values[i] = (MAX_COUNT/200)*15;


	DDRA |= 1;

	// ��������� �������� �1.
	TCCR1A = 0;
	// ��������� �������� �������� �� 8 ������.
	TCCR1B = (0<<CS10) | (1<<CS11) | (0<<CS12);
	
	OCR1AH = (unsigned char)(MAX_COUNT>>8);
	OCR1AL = (unsigned char)MAX_COUNT; 

	// �������� ���������� ���������, ��������� ����������.
	TIMSK |= (1<<TOIE1) | (1<<OCIE1A);
}


/**
* �������� ���������� ����� �����������.
*/
void closePWM()
{
	// ����� ���� ���������.
	TIMSK = 0;
	TCCR1B = 0;
	TCCR1B = 0;

	free(ocr_values);
}

/**
* ������� ������������� ����� � ������ ���������.
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
* ����������� ���� ����� �� ��������� ��������.
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
* ��������� ���� ����� �� ��������� ��������.
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
* ���������� �������� ��� ������������.
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
* �������� ���������� �� ����������.
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
* �������� ���������� �� ������������ �������.
*/
ISR (SIG_OVERFLOW1)		
{
	printf("*\n");
}
