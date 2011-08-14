#ifndef I2C_H
#define I2C_H
 
#include <avr/io.h>
#include <avr/interrupt.h>
#include "avrtypedef.h"

#define i2c_MasterBytesRX 2 // Величина принимающего буфера режима Slave, т.е. сколько байт жрем.
#define i2c_MasterBytesTX 1 // Величина Передающего буфера режима Slave , т.е. сколько байт отдаем за сессию.
 
#define i2c_Err_msk 0b00110011 // Маска кода ошибок
#define i2c_Err_NO 0b00000000 // All Right! -- Все окей, передача успешна.
#define i2c_ERR_NA 0b00010000 // Device No Answer -- Слейв не отвечает. Т.к. либо занят, либо его нет на линии.
#define i2c_ERR_LP 0b00100000 // Low Priority -- нас перехватили собственным адресом, либо мы проиграли арбитраж
#define i2c_ERR_NK 0b00000010 // Received NACK. End Transmittion. -- Был получен NACK. Бывает и так.
#define i2c_ERR_BF 0b00000001 // BUS FAIL -- Автобус сломался. И этим все сказано. Можно попробовать сделать переинициализацию шины
 
#define i2c_Interrupted 0b10000000 // Transmiting Interrupted Битмаска установки флага занятости
#define i2c_NoInterrupted 0b01111111 // Transmiting No Interrupted Битмаска снятия флага занятости
 
#define i2c_Busy 0b01000000 // Trans is Busy Битмаска флага "Передатчик занят, руками не трогать".
#define i2c_Free 0b10111111 // Trans is Free Битмаска снятия флага занятости.
 
 
#define MACRO_i2c_WhatDo_MasterOut (MasterOutFunc)(); // Макросы для режимо выхода. Пока тут функция, но может быть что угодно
#define MACRO_i2c_WhatDo_SlaveOut (SlaveOutFunc)();
#define MACRO_i2c_WhatDo_ErrorOut (ErrorOutFunc)();
 

typedef void (*IIC_F)(void); // Тип указателя на функцию
 
IIC_F MasterOutFunc; // Подрбрости в сишнике.
IIC_F SlaveOutFunc;
IIC_F ErrorOutFunc;
 
 
u08 i2c_Do;
u08 i2c_InBuff[i2c_MasterBytesRX];
u08 i2c_OutBuff[i2c_MasterBytesTX];
 
u08 i2c_SlaveIndex;
 
 
u08 i2c_index;
u08 i2c_ByteCount;
 
u08 i2c_PageAddrIndex;
u08 i2c_PageAddrCount;
 
 
void Init_Slave_i2c();
 
#endif
