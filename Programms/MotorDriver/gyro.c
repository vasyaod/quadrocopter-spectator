#include <avr/io.h>
#include <avr/interrupt.h>
#include "gyro.h"

u08 gyro_values[4];
u08 current_gyro = 0;

void initADC()
{
	// ��������� ���.
	ADMUX = 0;
	ADMUX |= (01<<6); // � �������� �������� ��������� ������� ������ AVCC.
	ADMUX |= (0<<5);  // ������������ �� ������� ����.
	ADMUX |= current_gyro;  // ������������������� �� 0 �����.


	ADCSRA = (1<<ADEN) |		// ��������� ���.	
			 (1<<ADIE) |		// ��������� ����������.
			 (1<<ADSC) |		// ��������� ��������������.
			 (7<<ADPS0);		// �������� ������� 128.
	
	// ��������� 8-������� ������� ������� ����� ���������� ��������.
	TCCR0 = 0x4;		// �������� ������� 256.
	//TCCR0 = 0x3;		// �������� ������� 64.
	// �������� ���������� ������� �� ������������.
	TIMSK |= 1<<TOIE0;
}

u08 *get_gyro_values()
{
	return gyro_values;
}

ISR (TIMER0_OVF_vect)		
{
	// ������ ��� ����� ����������� ���������� �� ������������ �������,
	// ��������� ������ ���.
	ADCSRA |= (1<<ADSC);
}


ISR (ADC_vect)		
{
	gyro_values[current_gyro*2] = ADCL;
	gyro_values[current_gyro*2+1] = ADCH;

	if (current_gyro == 0)
		current_gyro = 1;

	// ���������� ���� ��� �� ������ ����.
	ADMUX &= 0xF8;
	ADMUX |= current_gyro;
}
