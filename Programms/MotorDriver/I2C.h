#ifndef I2C_H
#define I2C_H
 
#include <avr/io.h>
#include <avr/interrupt.h>
#include "avrtypedef.h"

#define i2c_MasterBytesRX 2 // �������� ������������ ������ ������ Slave, �.�. ������� ���� ����.
#define i2c_MasterBytesTX 1 // �������� ����������� ������ ������ Slave , �.�. ������� ���� ������ �� ������.
 
#define i2c_Err_msk 0b00110011 // ����� ���� ������
#define i2c_Err_NO 0b00000000 // All Right! -- ��� ����, �������� �������.
#define i2c_ERR_NA 0b00010000 // Device No Answer -- ����� �� ��������. �.�. ���� �����, ���� ��� ��� �� �����.
#define i2c_ERR_LP 0b00100000 // Low Priority -- ��� ����������� ����������� �������, ���� �� ��������� ��������
#define i2c_ERR_NK 0b00000010 // Received NACK. End Transmittion. -- ��� ������� NACK. ������ � ���.
#define i2c_ERR_BF 0b00000001 // BUS FAIL -- ������� ��������. � ���� ��� �������. ����� ����������� ������� ����������������� ����
 
#define i2c_Interrupted 0b10000000 // Transmiting Interrupted �������� ��������� ����� ���������
#define i2c_NoInterrupted 0b01111111 // Transmiting No Interrupted �������� ������ ����� ���������
 
#define i2c_Busy 0b01000000 // Trans is Busy �������� ����� "���������� �����, ������ �� �������".
#define i2c_Free 0b10111111 // Trans is Free �������� ������ ����� ���������.
 
 
#define MACRO_i2c_WhatDo_MasterOut (MasterOutFunc)(); // ������� ��� ������ ������. ���� ��� �������, �� ����� ���� ��� ������
#define MACRO_i2c_WhatDo_SlaveOut (SlaveOutFunc)();
#define MACRO_i2c_WhatDo_ErrorOut (ErrorOutFunc)();
 

typedef void (*IIC_F)(void); // ��� ��������� �� �������
 
IIC_F MasterOutFunc; // ���������� � �������.
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
