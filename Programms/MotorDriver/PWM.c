
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "PWM.h"

#define SERVO_FREQUENCY 50
#define MAX_COUNT F_CPU/(SERVO_FREQUENCY*8)
#define SERVO_COUNT 1


u16 *out_values;		// Массив выходных значений для серво.
u16 *in_values;			// Массив входных значений.

u08 *pin_state;			// Переменная хранит состояние ножек для входных ШИМ сигналов.
u16 *pin_couter_value;	// Хрант значение счетчика вмомент когда ножка 
						// переключилась из 0 в 1.

out_pin_f out_pin;
in_pin_f in_pin;

u08 interrupt = 0;
u16 loop_counter;

/**
* Функция инициялизирует ШИМ для серво приводов.
*/
void initPWM(out_pin_f _out_pin, in_pin_f _in_pin)
{
	out_pin = _out_pin;
	in_pin = _in_pin;

	//////////////////////
	// Настройки выходного ШИМ.

	out_values = malloc(SERVO_COUNT*sizeof(u16));
   
	// Установим дефолтные значения, серво машинки будут находится в среднем 
	// положении.
	for (u08 i=0; i<SERVO_COUNT; i++)
		out_values[i] = (MAX_COUNT/200)*15;

	// Настройки счетчика Т1.
	TCCR1A = 0;
	// Установим делитель счетчика на 8 тактов.
	TCCR1B = (0<<CS10) | (1<<CS11) | (0<<CS12);
	
	OCR1AH = (unsigned char)(MAX_COUNT>>8);
	OCR1AL = (unsigned char)MAX_COUNT; 

	// Настроки прерываний счетчиков, разрешаем прерывания.
	TIMSK |= (1<<TOIE1) | (1<<OCIE1A);

	//////////////////////
	// Настройки входного ШИМ.

	pin_state = malloc(SERVO_COUNT*sizeof(u08));
	pin_couter_value = malloc(SERVO_COUNT*sizeof(u16));
	in_values = malloc(SERVO_COUNT*sizeof(u16));

	// Инициализация.
	for (u08 i=0; i<SERVO_COUNT; i++)
	{
		pin_state[i] = 0;
		pin_couter_value[i] = 0;
		in_values[i] = 0;
	}

	// Разрещаем внешние прерывания.
	MCUCR |= (1<<ISC10);
	MCUCR &=~(1<<ISC11);
	GICR = (1<<INT1);
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
	GICR = 0;
	MCUCR = 0;


	free(pin_state);
	free(pin_couter_value);
	free(in_values);
	free(out_values);

}

/**
* Возвращает значение для сервомашинки.
*/
u16 get_out_value(u08 servo_number)
{
	if (servo_number <= SERVO_COUNT)
	{
		cli();
		u16 res = out_values[servo_number];
		sei();
		return res;
	}
	else
		return 0;
}

/**
* Функция устанавливает серво в нужное положение.
*/ 
void set_out_value(u08 servo_number, u08 value)
{
	if (servo_number <= SERVO_COUNT)
	{
		cli();
		out_values[servo_number] = value;
		sei();
	}
}

/**
* Функция возвращает значение входящего шигнала.
*/
u16 get_in_value(u08 servo_number)
{
	if (servo_number <= SERVO_COUNT)
	{
		cli();
		u16 res = in_values[servo_number];
		sei();
		return res;
	}
	else
		return 0;
}

/**
* Увеличивает угол сервы на указанное значение.
*/

//void inc_servo(int servo_number, int value)
//{
//	if (servo_number <= SERVO_COUNT)
//	{
//		cli();
//		out_values[servo_number] += value;
//		sei();
//	}
//}

/**
* Уменьшает угол сервы на указанное значение.
*/
//void dec_servo(int servo_number, int value)
//{
//	if (servo_number <= SERVO_COUNT)
//	{
//		cli();
//		out_values[servo_number] -= value;
//		sei();
//	}
//}

/**
* Обработка внешнего прерывания.
*/

ISR(INT1_vect)
{
	interrupt = 1;
}

void checkPins()
{
	cli();
	if (interrupt == 0)
	{
		sei();
		return;
	}
	interrupt = 0;
	int new_counter_value = 0;
	sei();


	// Проверяем на какой ножке сработало прерывание.
    for (u08 i=0; i<SERVO_COUNT; i++)
	{

		u08 _in_pin = in_pin(i);

		if (pin_state[i] == 0 && _in_pin == 1)
		{
			pin_state[i] = 1;
			pin_couter_value[i] = new_counter_value;
		}
		else if (pin_state[i] == 1 && _in_pin == 0)
		{

			pin_state[i] = 0;
			u16 in_value;
			if (new_counter_value < pin_couter_value[i])
			{
				in_value = MAX_COUNT - pin_couter_value[i] + new_counter_value;
			}
			else
				in_value = new_counter_value - pin_couter_value[i];
			
			in_values[i] = in_value;
		}
	}

}


/**
* Перехват прерываний на совпадение.
*/
ISR (TIMER1_COMPA_vect)		
{
	int counter = (TCNT1H<<8) | (TCNT1L);
	int next_ocr = MAX_COUNT;

	if (counter >= MAX_COUNT)
	{
		loop_counter++;
		TCNT1H = 0;
		TCNT1L = 0;
		counter = 0;
	}
	
	for (u08 i=0; i<SERVO_COUNT; i++)
	{
		if (out_values[i] > 0 && out_values[i] > counter)
		{
			out_pin(i, 1);	
			if (next_ocr > out_values[i])
				next_ocr = out_values[i];
		}
		else
			out_pin(i, 0);	
	}

	OCR1AH = (u08)(next_ocr>>8);
	OCR1AL = (u08)next_ocr; 
}
