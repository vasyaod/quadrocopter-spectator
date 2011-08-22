#include <stdio.h>
#include <stdlib.h>

#include "I2C.h"
 
short address = -1; 

i2c_write_f i2c_write;
i2c_read_f i2c_read;

//			    TWCR = 0<<TWSTA|		  // 1 - посылает Start.
//					    0<<TWSTO|
//					    1<<TWINT|
//					    0<<TWEA|          // 1 - ACK, 0 - NACK
//					    1<<TWEN|
//					    1<<TWIE; // Принять и сказать пошли все н... NACK!
 
ISR(TWI_vect) // Прерывание TWI Тут наше все.
{
	PORTD |= (1<<PD0);
//	return;

    switch(TWSR & 0xF8) // Отсекаем биты прескалера
    {
    	case 0x00: // Bus Fail (автобус сломался)
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
 
//	    case 0x68: // RCV SLA+W Low Priority // Словили свой адрес во время передачи мастером
//	    case 0x78: // RCV SLA+W Low Priority (Broadcast) // Или это был широковещательный пакет. Не важно
//	    {
//		    i2c_Do |= i2c_ERR_LP | i2c_Interrupted; // Ставим флаг ошибки Low Priority, а также флаг того, что мастера прервали
//
// 		    // Restore Trans after.
//		    i2c_index = 0; // Подготовили прерваную передачу заново
//		    i2c_PageAddrIndex = 0;
//	    } // И пошли дальше. Внимание!!! break тут нет, а значит идем в "case 60"
 
	    case 0x60: // RCV SLA+W Incoming? // Или просто получили свой адрес
	    case 0x70: // RCV SLA+W Incoming? (Broascast) // Или широковещательный пакет
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
 
	    case 0x80: // RCV Data Byte // И вот мы приняли этот байт. Наш или широковещательный. Не важно
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
					    1<<TWIE; // Места еще дофига? Принять и ACK!
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
					    1<<TWIE; // Места еще дофига? Принять и ACK!
			}
		    break;
	    }
 
	    case 0x88: // RCV Last Byte // Приянли последний байт
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
 
 
	    case 0xA0: // Ой, мы получили Повторный старт. Но чо нам с ним делать?
	    {
 
		    // Можно, конечно, сделать вспомогательный автомат, чтобы обрабатывать еще и адреса внутренних страниц, подобно еепромке.
		    // Но я не стал заморачиваться. В этом случае делается это тут.
 
		    TWCR = 0<<TWSTA|
				    0<<TWSTO|
				    1<<TWINT|
				    1<<TWEA|
				    1<<TWEN|
				    1<<TWIE; // просто разадресуемся, проигнорировав этот посыл
		    break;
	    }
 
//	    case 0xB0: // Поймали свой адрес на чтение во время передачи Мастером
//	    {
//		    i2c_Do |= i2c_ERR_LP | i2c_Interrupted; // Ну чо, коды ошибки и флаг прерваной передачи.
//
// 
//		    // Восстанавливаем индексы
//		    i2c_index = 0;
//		    i2c_PageAddrIndex = 0;
//
//	    } // Break нет! Идем дальше
 
	    case 0xA8: // // Либо просто словили свой адрес на чтение
	    {
			if (address < 0 )
			{
			    TWCR = 0<<TWSTA|
					    0<<TWSTO|
					    1<<TWINT|
					    0<<TWEA|
					    1<<TWEN|
					    1<<TWIE; // Если он последний, мы еще на NACK в ответ надеемся
			
			}
			else
			{
		    	TWDR = i2c_read(address);

			    TWCR = 0<<TWSTA|
					    0<<TWSTO|
					    1<<TWINT|
					    1<<TWEA|
					    1<<TWEN|
					    1<<TWIE; // А если нет, то ACK ждем
			
			}
		    break;
	    }
 
 
	    case 0xB8: // Послали байт, получили ACK
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
 
	    case 0xC0: // Мы выслали последний байт, больше у нас нет, получили NACK
	    case 0xC8: // или ACK. В данном случае нам пох. Т.к. больше байтов у нас нет.
	    {
			address = -1;
			TWCR = 0<<TWSTA|
				    0<<TWSTO|
				    1<<TWINT|
				    1<<TWEA|
				    1<<TWEN|
				    1<<TWIE; // Если мы там одни, то просто отдадим шину
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
		1<<TWIE; // Включаем агрегат и начинаем слушать шину.
}
