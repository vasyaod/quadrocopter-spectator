
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/interrupt.h>

#include "UART.h"

static int uart_putchar(char c, FILE *stream);

static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL,
                                             _FDEV_SETUP_WRITE);

// Набор переменных исходящего буфера.
int output_buffer_index = 0;
Element *first;
Element *last;

// Набор переменных входящего буффера.
#define MAX_INPUT_BUFFER 128

int input_buffer_index = 0;
char *input_buffer;

/**
* Вывод символа в UART.
*/
static int uart_putchar(char c, FILE *stream)
{
	
	if (output_buffer_index+1 >= MAX_INPUT_BUFFER)
		return 0; // буффер переполнен, сваливаем....

	if (c == '\n')
    	uart_putchar('\r', stream);

	Element *newElement = (Element*)malloc(sizeof(Element));
	newElement->data = c;

	if (output_buffer_index == 0)
	{
		first = newElement;
		last = newElement;
	}
	else
	{
		last->next = newElement;
		last = newElement;
	}
	
	output_buffer_index ++;
	UCSRB |= 1 << UDRIE;

//	loop_until_bit_is_set(UCSRA, UDRE);
//	UDR = c;
	return 0;
}

ISR (USART_UDRE_vect)		
{
	if(output_buffer_index == 0)
	{
		UCSRB &=~(1<<UDRIE);		// Запрещаем прерывание по опустошению - передача закончена
	}
	else 
	{
		UDR = first->data;			// Берем данные из буффера.
		Element *_tmp = first;		
		first = _tmp->next;
		output_buffer_index--;
		free(_tmp);
	}
	
}

ISR (USART_RXC_vect)		
{
	char data = UDR;
	
	printf("%c", data);
	if ((data == '\n' || data == '\r'))
	{
		if (input_buffer_index != 0)
		{
			input_buffer[input_buffer_index] = 0;
			printf("Executed comand: %s \n",input_buffer);
			executed_comand(input_buffer, input_buffer_index);
			// Сбрасываем буфер.
			input_buffer_index = 0;
		}
	}	
	else if (input_buffer_index < MAX_INPUT_BUFFER)
	{
		input_buffer[input_buffer_index] = data;
		input_buffer_index++;
	}
}

/**
* Инициализация UART.
*/
void initUART()
{
	input_buffer = malloc(MAX_INPUT_BUFFER);

	UCSRA=0x00;
	UCSRB=0xD8;
	UCSRC=0x06;
	
	UCSRB |= (1 << RXCIE);

	int myubrr = MYUBRR;
	UBRRH = (unsigned char)(myubrr>>8);
	UBRRL = (unsigned char)myubrr; 
	stdout = &mystdout;

}

/**
* Закрывает UART.
*/
void closeUART()
{
	free(input_buffer);
}
