#include <stdio.h>
#include <stdlib.h>

#include "I2C.h"
 
short address = -1; 

i2c_write_f i2c_write;
i2c_read_f i2c_read;

//			    TWCR = 0<<TWSTA|		  // 1 - �������� Start.
//					    0<<TWSTO|
//					    1<<TWINT|
//					    0<<TWEA|          // 1 - ACK, 0 - NACK
//					    1<<TWEN|
//					    1<<TWIE; // ������� � ������� ����� ��� �... NACK!
 
ISR(TWI_vect) // ���������� TWI ��� ���� ���.
{
	PORTD |= (1<<PD0);
//	return;

    switch(TWSR & 0xF8) // �������� ���� ����������
    {
    	case 0x00: // Bus Fail (������� ��������)
    	{
		    TWCR = 0<<TWSTA|
		    1<<TWSTO|
		    1<<TWINT|
		    1<<TWEA|
		    1<<TWEN|
		    1<<TWIE; // Go!
 
		    break;
		}
     
// IIC Slave ============================================================================
 
//	    case 0x68: // RCV SLA+W Low Priority // ������� ���� ����� �� ����� �������� ��������
//	    case 0x78: // RCV SLA+W Low Priority (Broadcast) // ��� ��� ��� ����������������� �����. �� �����
//	    {
//		    i2c_Do |= i2c_ERR_LP | i2c_Interrupted; // ������ ���� ������ Low Priority, � ����� ���� ����, ��� ������� ��������
//
// 		    // Restore Trans after.
//		    i2c_index = 0; // ����������� ��������� �������� ������
//		    i2c_PageAddrIndex = 0;
//	    } // � ����� ������. ��������!!! break ��� ���, � ������ ���� � "case 60"
 
	    case 0x60: // RCV SLA+W Incoming? // ��� ������ �������� ���� �����
	    case 0x70: // RCV SLA+W Incoming? (Broascast) // ��� ����������������� �����
	    {
			address = -1; 			

			TWCR = 0<<TWSTA|
					0<<TWSTO|
					1<<TWINT|
					1<<TWEA|
					1<<TWEN|
					1<<TWIE; 
		    break;
	    }
 
	    case 0x80: // RCV Data Byte // � ��� �� ������� ���� ����. ��� ��� �����������������. �� �����
	    case 0x90: // RCV Data Byte (Broadcast)
	    {
			if (address < 0)
			{
				address = TWDR;
		    	TWCR = 0<<TWSTA|
					    0<<TWSTO|
					    1<<TWINT|
					    1<<TWEA|
					    1<<TWEN|
					    1<<TWIE; // ����� ��� ������? ������� � ACK!
			}
			else
			{

				i2c_write(address, TWDR);
				address ++;

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
			if (address < 0)
				address = TWDR;
			else
			{
				i2c_write(address, TWDR);
				address ++;
			}

		    TWCR = 0<<TWSTA|
				    0<<TWSTO|
				    1<<TWINT|
				    1<<TWEA|
				    1<<TWEN|
				    1<<TWIE;
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
 
//	    case 0xB0: // ������� ���� ����� �� ������ �� ����� �������� ��������
//	    {
//		    i2c_Do |= i2c_ERR_LP | i2c_Interrupted; // �� ��, ���� ������ � ���� ��������� ��������.
//
// 
//		    // ��������������� �������
//		    i2c_index = 0;
//		    i2c_PageAddrIndex = 0;
//
//	    } // Break ���! ���� ������
 
	    case 0xA8: // // ���� ������ ������� ���� ����� �� ������
	    {
			if (address < 0 )
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
		    	TWDR = i2c_read(address);

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
 			address ++;
	    	TWDR = i2c_read(address);
 
		    TWCR = 0<<TWSTA|
				    0<<TWSTO|
				    1<<TWINT|
				    1<<TWEA|
				    1<<TWEN|
				    1<<TWIE; 
 
		    break;
	    }
 
	    case 0xC0: // �� ������� ��������� ����, ������ � ��� ���, �������� NACK
	    case 0xC8: // ��� ACK. � ������ ������ ��� ���. �.�. ������ ������ � ��� ���.
	    {
			address = -1;
			TWCR = 0<<TWSTA|
				    0<<TWSTO|
				    1<<TWINT|
				    1<<TWEA|
				    1<<TWEN|
				    1<<TWIE; // ���� �� ��� ����, �� ������ ������� ����
		    break;
	    }
 // */
	    default: break;
    }
}


void i2c_init_slave(u08 slave_address, u08 broadcast, i2c_write_f _i2c_write, i2c_read_f _i2c_read)
{
	i2c_write = _i2c_write;
	i2c_read = _i2c_read;
	// 
	TWAR = (slave_address<<1)|broadcast;
 
	TWCR = 0<<TWSTA|
		0<<TWSTO|
		0<<TWINT|
		1<<TWEA|
		1<<TWEN|
		1<<TWIE; // �������� ������� � �������� ������� ����.
}
