
#include <avr/io.h>
//#include <stdlib.h>
#include <avr/interrupt.h>

#include "PWM.h"

#define SERVO_FREQUENCY 50
#define MAX_COUNT F_CPU/(SERVO_FREQUENCY*8)
#define FILTER 7

//#define USE_INTERRUPT 0

u16 out_values[SERVO_COUNT];			// Массив выходных значений для серво.
u16 in_values[SERVO_COUNT];				// Массив входных значений.
u16 in_values_tmp[SERVO_COUNT][FILTER];	// Массив входных значений.

u08 pin_state[SERVO_COUNT];			// Хранит состояние входных ножек.
u08 pin_state_tmp[SERVO_COUNT];		// 
u16 pin_couter_value[SERVO_COUNT];	// Хрант значение счетчика вмомент когда ножка 
									// переключилась из 0 в 1.
out_pin_f out_pin;
in_pin_f in_pin;

#ifdef USE_INTERRUPT
u08 interrupt = 0;
#endif

u16 loop_counter;
u08 out_off = 0;

/**
* Функция инициялизирует ШИМ для серво приводов.
*/
void initPWM(out_pin_f _out_pin, in_pin_f _in_pin)
{
	out_pin = _out_pin;
	in_pin = _in_pin;

	//////////////////////
	// Настройки выходного ШИМ.

	//out_values = malloc(SERVO_COUNT*sizeof(u16));
   
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

	//pin_state = malloc(SERVO_COUNT*sizeof(u08));
	//pin_state_tmp = malloc(SERVO_COUNT*sizeof(u08));
	//pin_couter_value = malloc(SERVO_COUNT*sizeof(u16));
	//in_values = malloc(SERVO_COUNT*sizeof(u16));

	// Инициализация.
	for (u08 i=0; i<SERVO_COUNT; i++)
	{
		pin_state[i] = 0;
		pin_couter_value[i] = 0;
		in_values[i] = 0;
	}

#ifdef USE_INTERRUPT	
	// Разрещаем внешние прерывания.
	MCUCR |= (1<<ISC10);
	MCUCR &=~(1<<ISC11);
	GICR = (1<<INT1);
#endif
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
#ifdef USE_INTERRUPT	
	GICR = 0;
	MCUCR = 0;
#endif


//	free(pin_state);
//	free(pin_state_tmp);
//	free(pin_couter_value);
//	free(in_values);
//	free(out_values);

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
* Функция возвращает указатель на массив выходных параметров .
*/ 
u16 *get_out_values()
{
	return out_values;
}

/**
* Функция устанавливает серво в нужное положение.
*/ 
void set_out_value(u08 servo_number, u16 value)
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
* Функция возвращает указатель на массив входых параметров .
*/ 
u16 *get_in_values()
{
	return in_values;
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

#ifdef USE_INTERRUPT	
ISR(INT1_vect)
{
	interrupt = 1;
}
#endif

void pwm_check_in(u16 counter_value)
{
#ifdef USE_INTERRUPT	
	if (interrupt == 0)
		return;
	interrupt = 0;
#endif

	//out_pin(0, in_pin(0));
	// Времено сохраним значения в память.
    for (u08 i=0; i<SERVO_COUNT; i++)
		pin_state_tmp[i] = in_pin(i);

	// Проверяем на какой ножке сработало прерывание.
    for (u08 i=0; i<SERVO_COUNT; i++)
	{
	//	u08 _in_pin = in_pin(i);
		u08 _in_pin = pin_state_tmp[i];

		if (pin_state[i] == 0 && _in_pin == 1)
		{
			pin_state[i] = 1;
			pin_couter_value[i] = counter_value;
		}
		else if (pin_state[i] == 1 && _in_pin == 0)
		{

			pin_state[i] = 0;
			int _counter_value = counter_value;
			int _pin_couter_value = pin_couter_value[i];

			int in_value = _counter_value - _pin_couter_value;
			if (in_value < 0)
				in_value = in_value + MAX_COUNT;

		    for (u08 j=FILTER-1; j>0; j--)
				in_values_tmp[i][j] = in_values_tmp[i][j-1];
			
			in_values_tmp[i][0] = in_value; 
			
			in_values[i] = 0;
		    for (u08 j=0; j<FILTER; j++)
				in_values[i] += in_values_tmp[i][j];
			
			in_values[i] /= FILTER;

//			if (i == 0)
//				set_out_value(0, in_values[i]);
		}

	}

}

/**
* Перехват прерываний на совпадение.
*/
ISR (TIMER1_COMPA_vect)		
{

	//u16 counter = (TCNT1H<<8) | (TCNT1L);
	//int next_ocr = MAX_COUNT;

	//if (counter >= MAX_COUNT)
	//{
	//	loop_counter++;
	//	TCNT1H = 0;
	//	TCNT1L = 0;
	//	counter = 0;
	//}
	/*
	if (counter < 2100)
	{
		next_ocr = 2100;
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
	}
	else
	{
		for (u08 i=0; i<SERVO_COUNT; i++)
		{
			out_pin(i, 0);	
		}
	}
	OCR1AH = (u08)(next_ocr>>8);
	OCR1AL = (u08)next_ocr; 
	*/

}

void pwm_check_out(u16 counter_value)
{
	if (counter_value < 2100)
	{
		for (u08 i=0; i<SERVO_COUNT; i++)
		{
			if (out_values[i] > 0 && out_values[i] > counter_value)
				out_pin(i, 1);	
			else
				out_pin(i, 0);	
		}
		out_off = 1;
	}
	else
	{
		for (u08 i=0; i<SERVO_COUNT; i++)
			out_pin(i, 0);
		out_off = 0; 
	}

}
