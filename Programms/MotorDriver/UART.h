#ifndef _UART_H
#define _UART_H 1

#define BAUD 9600					// ������ UART.
#define MYUBRR F_CPU/16/BAUD-1

struct _Element;
typedef struct _Element Element;

struct _Element
{
	unsigned char data;
	Element *next;
};

/**
* ������������� UART.
*/
void initUART();

/**
* �������� UART.
*/
void closeUART();

/**
*  ����������� ������ ���������� � UART.
*/
void executed_comand(char *command, int len);

#endif	// UART.h
