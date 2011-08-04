#include <stdio.h>
#include <stdlib.h>

#include "I2C.h"
 
 
void DoNothing(void);
 
u08 i2c_Do; // ���������� ��������� ����������� IIC
u08 i2c_InBuff[i2c_MasterBytesRX]; // ����� ����� ��� ������ ��� Slave
u08 i2c_OutBuff[i2c_MasterBytesTX]; // ����� �������� ��� ������ ��� Slave
u08 i2c_SlaveIndex; // ������ ������ Slave
 
 
u08 i2c_index; // ������ ����� ������
u08 i2c_ByteCount; // ����� ���� ������������
 
u08 i2c_SlaveAddress; // ����� ������������
 
// ��������� ������ �� ��������:
IIC_F MasterOutFunc = &DoNothing; // � Master ������
IIC_F SlaveOutFunc = &DoNothing; // � ������ Slave
IIC_F ErrorOutFunc = &DoNothing; // � ���������� ������ � ������ Master
 
 
ISR(TWI_vect) // ���������� TWI ��� ���� ���.
{
	printf("i2c -> %d\n", (TWSR & 0xF8));

    switch(TWSR & 0xF8) // �������� ���� ����������
    {
    	case 0x00: // Bus Fail (������� ��������)
    	{
		    i2c_Do |= i2c_ERR_BF;
 
		    TWCR = 0<<TWSTA|
		    1<<TWSTO|
		    1<<TWINT|
		    1<<TWEA|
		    1<<TWEN|
		    1<<TWIE; // Go!
 
 			printf("i2c error!");
		    break;
		}
     
// IIC Slave ============================================================================
 
	    case 0x68: // RCV SLA+W Low Priority // ������� ���� ����� �� ����� �������� ��������
	    case 0x78: // RCV SLA+W Low Priority (Broadcast) // ��� ��� ��� ����������������� �����. �� �����
	    {
		    i2c_Do |= i2c_ERR_LP | i2c_Interrupted; // ������ ���� ������ Low Priority, � ����� ���� ����, ��� ������� ��������
 
 		    // Restore Trans after.
		    i2c_index = 0; // ����������� ��������� �������� ������
		    i2c_PageAddrIndex = 0;
	    } // � ����� ������. ��������!!! break ��� ���, � ������ ���� � "case 60"
 
	    case 0x60: // RCV SLA+W Incoming? // ��� ������ �������� ���� �����
	    case 0x70: // RCV SLA+W Incoming? (Broascast) // ��� ����������������� �����
	    {
 
		    i2c_Do |= i2c_Busy; // �������� ����. ����� ������ �� ��������
		    i2c_SlaveIndex = 0; // ��������� �� ������ ������ ������, ������� ����� �����. �� ��������
 
		    if (i2c_MasterBytesRX == 1) // ���� ��� ������� ������� ����� ���� ����, �� ��������� ������� ���
		    {
			    TWCR = 0<<TWSTA|
					    0<<TWSTO|
					    1<<TWINT|
					    0<<TWEA|
					    1<<TWEN|
					    1<<TWIE; // ������� � ������� ����� ��� �... NACK!
		    }
		    else
		    {
			    TWCR = 0<<TWSTA|
					    0<<TWSTO|
					    1<<TWINT|
					    1<<TWEA|
					    1<<TWEN|
					    1<<TWIE; // � ���� ���� ���� ��� ���� ����, �� ������ � ��������� ��� ACK!
		    }
		    break;
	    }
 
	    case 0x80: // RCV Data Byte // � ��� �� ������� ���� ����. ��� ��� �����������������. �� �����
	    case 0x90: // RCV Data Byte (Broadcast)
	    {
		    i2c_InBuff[i2c_SlaveIndex] = TWDR; // ������� ��� � �����.
 
		    i2c_SlaveIndex++; // �������� ���������
 
		    if (i2c_SlaveIndex == i2c_MasterBytesRX-1) // �������� ����� ����� ��� ���� ����?
		    {
			    TWCR = 0<<TWSTA|
					    0<<TWSTO|
					    1<<TWINT|
					    0<<TWEA|
					    1<<TWEN|
					    1<<TWIE; // ������� ��� � ������� NACK!
		    }
		    else
		    {
		    	TWCR = 0<<TWSTA|
					    0<<TWSTO|
					    1<<TWINT|
					    1<<TWEA|
					    1<<TWEN|
					    1<<TWIE; // ����� ��� ������? ������� � ACK!
		    }
		    break;
	    }
 
	    case 0x88: // RCV Last Byte // ������� ��������� ����
	    case 0x98: // RCV Last Byte (Broadcast)
	    {
		    i2c_InBuff[i2c_SlaveIndex] = TWDR; // ������� ��� � �����
 
		    if (i2c_Do & i2c_Interrupted) // ���� � ��� ��� ���������� ����� �� ����� �������
		    {
			    TWCR = 1<<TWSTA|
					    0<<TWSTO|
					    1<<TWINT|
					    1<<TWEA|
					    1<<TWEN|
					    1<<TWIE; // ������ � ���� ���� Start �������� � ������� ��� ���� �������
		    }
		    else
		    {
			    TWCR = 0<<TWSTA|
					    0<<TWSTO|
					    1<<TWINT|
					    1<<TWEA|
					    1<<TWEN|
					    1<<TWIE; // ���� �� ���� ������ �����, �� ������ ��������� � ����� �����
		    }
 
		    MACRO_i2c_WhatDo_SlaveOut // � ������ ���������� ��� �������� ���� ��� ������
		    break;
	    }
 
 
	    case 0xA0: // ��, �� �������� ��������� �����. �� �� ��� � ��� ������?
	    {
 
		    // �����, �������, ������� ��������������� �������, ����� ������������ ��� � ������ ���������� �������, ������� ��������.
		    // �� � �� ���� ��������������. � ���� ������ �������� ��� ���.
 
		    TWCR = 0<<TWSTA|
				    0<<TWSTO|
				    1<<TWINT|
				    1<<TWEA|
				    1<<TWEN|
				    1<<TWIE; // ������ �������������, �������������� ���� �����
		    break;
	    }
 
 
 
	    case 0xB0: // ������� ���� ����� �� ������ �� ����� �������� ��������
	    {
		    i2c_Do |= i2c_ERR_LP | i2c_Interrupted; // �� ��, ���� ������ � ���� ��������� ��������.
 
 
		    // ��������������� �������
		    i2c_index = 0;
		    i2c_PageAddrIndex = 0;
 
	    } // Break ���! ���� ������
 
	    case 0xA8: // // ���� ������ ������� ���� ����� �� ������
	    {
		    i2c_SlaveIndex = 0; // ������� ��������� �������� �� 0
 
		    TWDR = i2c_OutBuff[i2c_SlaveIndex]; // ����, ������� ���� �� ��� ��� ����.
 
		    if(i2c_MasterBytesTX == 1)
		    {
			    TWCR = 0<<TWSTA|
					    0<<TWSTO|
					    1<<TWINT|
					    0<<TWEA|
					    1<<TWEN|
					    1<<TWIE; // ���� �� ���������, �� ��� �� NACK � ����� ��������
		    }
		    else
		    {
			    TWCR = 0<<TWSTA|
					    0<<TWSTO|
					    1<<TWINT|
					    1<<TWEA|
					    1<<TWEN|
					    1<<TWIE; // � ���� ���, �� ACK ����
		    }
 
		    break;
	    }
 
 
	    case 0xB8: // ������� ����, �������� ACK
	    {
 
		    i2c_SlaveIndex++; // ������ ���������� ���������. ����� ��������� ����
		    TWDR = i2c_OutBuff[i2c_SlaveIndex]; // ���� ��� �������
 
		    if (i2c_SlaveIndex == i2c_MasterBytesTX-1) // ���� �� ��������� ���, ��
		    {
			    TWCR = 0<<TWSTA|
					    0<<TWSTO|
					    1<<TWINT|
					    0<<TWEA|
					    1<<TWEN|
					    1<<TWIE; // ���� ��� � ���� NACK
		    }
		    else
		    {
			    TWCR = 0<<TWSTA|
					    0<<TWSTO|
					    1<<TWINT|
					    1<<TWEA|
					    1<<TWEN|
					    1<<TWIE; // ���� ���, �� ���� � ���� ACK
		    }
 
		    break;
	    }
 
	    case 0xC0: // �� ������� ��������� ����, ������ � ��� ���, �������� NACK
	    case 0xC8: // ��� ACK. � ������ ������ ��� ���. �.�. ������ ������ � ��� ���.
	    {
		    if (i2c_Do & i2c_Interrupted) // ���� ��� ���� ��������� �������� �������
		    { // �� �� ��� �� ������
			    i2c_Do &= i2c_NoInterrupted;
			    // ������ ���� �����������
			    TWCR = 1<<TWSTA|
					    0<<TWSTO|
					    1<<TWINT|
					    1<<TWEA|
					    1<<TWEN|
					    1<<TWIE; // �������� ����� ����� �� ��� ������� ����.
		    }
		    else
		    {
			    TWCR = 0<<TWSTA|
					    0<<TWSTO|
					    1<<TWINT|
					    1<<TWEA|
					    1<<TWEN|
					    1<<TWIE; // ���� �� ��� ����, �� ������ ������� ����
		    }
 
		    MACRO_i2c_WhatDo_SlaveOut // � ���������� ����� ������. �������, �� ���
		    // �� ����� �� �����. ����� ��� ��� ������, ��� ������
		    break; // ��� ������ ����� �������.
	    }
 
	    default: break;
    }
}

void DoNothing(void) // ������� ��������, �������� �������������� ������
{
}
 
 
void Init_Slave_i2c() // ��������� ������ ������ (���� �����)
{
	TWAR = 0x32; // ������ � ������� ���� �����, �� ������� ����� ����������.
	// 1 � ������� ���� ��������, ��� �� ���������� �� ����������������� ������
//	SlaveOutFunc = Addr; // �������� ��������� ������ �� ������ ������� ������
 
	TWCR = 0<<TWSTA|
		0<<TWSTO|
		0<<TWINT|
		1<<TWEA|
		1<<TWEN|
		1<<TWIE; // �������� ������� � �������� ������� ����.
}
