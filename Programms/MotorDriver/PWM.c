
#include <avr/io.h>
//#include <stdlib.h>
#include <avr/interrupt.h>

#include "PWM.h"

#define SERVO_FREQUENCY 50
#define MAX_COUNT F_CPU/(SERVO_FREQUENCY*8)
#define FILTER 7

//#define USE_INTERRUPT 0

u16 out_values[SERVO_COUNT];			// Массив выходных значений для серво.
u08 out_in_pin_state[SERVO_COUNT];
u16 in_values[SERVO_COUNT];				// Массив входных значений.
u16 in_values_tmp[SERVO_COUNT][FILTER];	// Массив входных значений.


u08 in_pin_state[SERVO_COUNT];			// Хранит состояние входных ножек.
u08 in_pin_state_tmp[SERVO_COUNT];		// 
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
	{
//		out_values[i] = (MAX_COUNT/200)*15;
		out_values[i] = 0;
		out_in_pin_state[i] = 0;
	}
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

	//in_pin_state = malloc(SERVO_COUNT*sizeof(u08));
	//in_pin_state_tmp = malloc(SERVO_COUNT*sizeof(u08));
	//pin_couter_value = malloc(SERVO_COUNT*sizeof(u16));
	//in_values = malloc(SERVO_COUNT*sizeof(u16));

	// Инициализация.
	for (u08 i=0; i<SERVO_COUNT; i++)
	{
		in_pin_state[i] = 0;
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


//	free(in_pin_state);
//	free(in_pin_state_tmp);
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
	cli();
	out_values[servo_number] = value;
	sei();
}

/**
* Функция возвращает значение входящего шигнала.
*/
u16 get_in_value(u08 servo_number)
{
	cli();
	u16 res = in_values[servo_number];
	sei();
	return res;
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

int old_counter = 0;
void pwm_check_in(int counter_value)
{
//#ifdef USE_INTERRUPT	
//	if (interrupt == 0)
//		return;
//	interrupt = 0;
//#endif

	int r = counter_value - old_counter;
	if (r > 0 && r < 2)
		return;
	old_counter = counter_value;



	//out_pin(0, in_pin(0));
	// Времено сохраним значения в память.
    for (u08 i=0; i<SERVO_COUNT; i++)
	{
	     switch(i) 
    	{
 			case 0:
				in_pin_state_tmp[i] = (PINB>>PB7)&1;
				break;
  			case 1:
				in_pin_state_tmp[i] = (PIND>>PD5)&1;
				break;
  			case 2:
				in_pin_state_tmp[i] = (PIND>>PD6)&1;
				break;
  			case 3:
				in_pin_state_tmp[i] = (PIND>>PD7)&1;
				break;
  			case 4:
				in_pin_state_tmp[i] = (PINB>>PB0)&1;
				break;
		}
	}
//		in_pin_state_tmp[i] = in_pin(i);
	

	// Проверяем на какой ножке сработало прерывание.
    for (u08 i=0; i<SERVO_COUNT; i++)
	{
	//	u08 _in_pin = 0;
		u08 _in_pin = in_pin_state_tmp[i];

		if (in_pin_state[i] == 0 && _in_pin == 1)
		{
			in_pin_state[i] = 1;
			pin_couter_value[i] = counter_value;
		}
		else if (in_pin_state[i] == 1 && _in_pin == 0)
		{

			in_pin_state[i] = 0;
			int _counter_value = counter_value;
			int _pin_couter_value = pin_couter_value[i];

			int in_value = _counter_value - _pin_couter_value;
			if (in_value < 0)
				in_value = in_value + MAX_COUNT;

//		    for (u08 j=FILTER-1; j>0; j--)
//				in_values_tmp[i][j] = in_values_tmp[i][j-1];
//			
//			in_values_tmp[i][0] = in_value; 
			
//			in_values[i] = 0;
//		    for (u08 j=0; j<FILTER; j++)
//				in_values[i] += in_values_tmp[i][j];
			
//			in_values[i] /= FILTER;
//			if (i == 0)
//				set_out_value(0, in_value);
		}
	}

}

ISR (TIMER1_OVF_vect)		
{
	DDRD |= (1<<PD0);
}

/**
* Перехват прерываний на совпадение.
*/
ISR (TIMER1_COMPA_vect)		
{
	pwm_check_out2();
}

void pwm_check_out2()
{
	u16 counter = (TCNT1H<<8) | (TCNT1L);
	int next_ocr = MAX_COUNT;

	if (counter >= MAX_COUNT)
	{
		loop_counter++;
		TCNT1H = 0;
		TCNT1L = 0;
		counter = 0;
	}
	
	if (counter < 2100)
	{
		next_ocr = 2100;
		for (u08 i=0; i<SERVO_COUNT; i++)
		{
			u16 out_value = out_values[i];
			if (out_value > 2100)
				out_value = 2100;
			if (out_value > 0 && out_value > counter)
			{
				out_pin(i, 1);	
				if (next_ocr > out_value)
					next_ocr = out_value;
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

	if (next_ocr > MAX_COUNT)
		next_ocr = MAX_COUNT;
	OCR1AH = (u08)(next_ocr>>8);
	OCR1AL = (u08)next_ocr; 
}

int  old_counter1 = 0;
void pwm_check_out(int counter_value)
{
	int r = counter_value - old_counter1;
	if (r > 0 && r < 2)
		return;
	old_counter1 = counter_value;

	if(out_off == 1 && counter_value > 2100)
	{
		for (u08 i=0; i<SERVO_COUNT; i++)
		{
			out_pin(i, 0);
			out_in_pin_state[i] = 0;
		}
		out_off = 0; 
	}
	else if (out_off == 0 && counter_value <= 2100)
	{
		for (u08 i=0; i<SERVO_COUNT; i++)
		{
			if (out_values[i] > 0)
			{
				out_pin(i, 1);
				out_in_pin_state[i] = 1;
			}
		}
		out_off = 1;
	}
	else if (out_off == 1 && counter_value <= 2100)
	{
		for (u08 i=0; i<SERVO_COUNT; i++)
		{
			if (out_in_pin_state[i] == 1 && out_values[i] < counter_value)
			{
				out_pin(i, 0);
				out_in_pin_state[i] = 0;
			}
               	
		}
	}


}
