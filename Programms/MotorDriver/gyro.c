#include <avr/io.h>
#include <avr/interrupt.h>
#include "gyro.h"

u08 gyro_values[4];
u08 current_gyro = 0;

void initADC()
{
	// Настройка АЦП.
	ADMUX = 0;
	ADMUX |= (01<<6); // В качестве опорного источника питания выбран AVCC.
	ADMUX |= (0<<5);  // Выраынивание по правому краю.
	ADMUX |= current_gyro;  // Мультиплексирование на 0 входе.


	ADCSRA = (1<<ADEN) |		// Разрещаем АЦП.	
			 (1<<ADIE) |		// Разрещаем прерывание.
			 (1<<ADSC) |		// Запускаем преобразование.
			 (7<<ADPS0);		// Делитель частоты 128.
	
	// Настройка 8-битного таймера который будет руковадить завуском.
	TCCR0 = 0x4;		// делитель частоты 256.
	//TCCR0 = 0x3;		// делитель частоты 64.
	// Включаем прерывание таймера на переполнение.
	TIMSK |= 1<<TOIE0;
}

u08 *get_gyro_values()
{
	return gyro_values;
}

ISR (TIMER0_OVF_vect)		
{
	// Каждый раз когда срабатывает прерывание на переполнение таймера,
	// запускаем работу АЦП.
	ADCSRA |= (1<<ADSC);
}


ISR (ADC_vect)		
{
	gyro_values[current_gyro*2] = ADCL;
	gyro_values[current_gyro*2+1] = ADCH;

	if (current_gyro == 0)
		current_gyro = 1;

	// Переключим вход АЦП на другую ногу.
	ADMUX &= 0xF8;
	ADMUX |= current_gyro;
}
