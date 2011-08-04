#ifndef _PWM_H
#define _PWM_H 1

/**
* Инициализирует ШИМ серво машинок.
*/
void initPWM();

/**
* Закрывает ШИМ.
*/
void closePWM();

/**
* Функция устанавливает серво в нужное положение.
*/ 
void set_servo(int servo_number, int value);

/**
* Увеличивает угол сервы на указанное значение.
*/
void inc_servo(int servo_number, int value);

/**
* Уменьшает угол сервы на указанное значение.
*/
void dec_servo(int servo_number, int value);

/**
* Возвращает значение для сервомашинки.
*/
int get_servo(int servo_number);

#endif	// PWM.h
