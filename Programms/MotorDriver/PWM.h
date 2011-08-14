#ifndef _PWM_H
#define _PWM_H 1

#include "avrtypedef.h"

typedef void (*out_pin_f)(u08, u08);
typedef u08 (*in_pin_f)(u08);

/**
* �������������� ��� ����� �������.
*/
void initPWM(out_pin_f, in_pin_f);

/**
* ��������� ���.
*/
void closePWM();

/**
* ���������� �������� ��� ������������.
*/
u16 get_out_value(u08 servo_number);

/**
* ������� ������������� ����� � ������ ���������.
*/ 
void set_out_value(u08 servo_number, u08 value);

/**
* ���������� �������� �������� �������� ���.
*/
u16 get_in_value(u08 servo_number);

void checkPins();

extern u16 loop_counter;

#endif	// PWM.h
