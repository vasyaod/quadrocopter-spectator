#ifndef _PWM_H
#define _PWM_H 1

#include "avrtypedef.h"

typedef void (*out_pin_f)(u08, u08);
typedef u08 (*in_pin_f)(u08);

/**
* �������������� ��� ����� �������.
*/
void initPWM(u08 servo_count, out_pin_f out_pin, in_pin_f in_pin);

/**
* ��������� ���.
*/
void closePWM();

/**
* ���������� �������� ��� ������������.
*/
u16 get_out_value(u08 servo_number);

/**
* ������� ���������� ��������� �� ������ �������� ���������� .
*/ 
u16 *get_out_values();

/**
* ������� ������������� ����� � ������ ���������.
*/ 
void set_out_value(u08 servo_number, u16 value);

/**
* ���������� �������� �������� �������� ���.
*/
u16 get_in_value(u08 servo_number);

/**
* ������� ���������� ��������� �� ������ ������ ���������� .
*/ 
u16 *get_in_values();

void pwm_check_in(u16 counter_value);

void pwm_check_out(u16 counter_value);

extern u16 loop_counter;

#endif	// PWM.h
