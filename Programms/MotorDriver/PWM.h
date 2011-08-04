#ifndef _PWM_H
#define _PWM_H 1

/**
* �������������� ��� ����� �������.
*/
void initPWM();

/**
* ��������� ���.
*/
void closePWM();

/**
* ������� ������������� ����� � ������ ���������.
*/ 
void set_servo(int servo_number, int value);

/**
* ����������� ���� ����� �� ��������� ��������.
*/
void inc_servo(int servo_number, int value);

/**
* ��������� ���� ����� �� ��������� ��������.
*/
void dec_servo(int servo_number, int value);

/**
* ���������� �������� ��� ������������.
*/
int get_servo(int servo_number);

#endif	// PWM.h
