#include <stdio.h>
#include <stdlib.h>

#include "I2C.h"
 
 
void DoNothing(void);
 
u08 i2c_Do; // Переменная состояния передатчика IIC
u08 i2c_InBuff[i2c_MasterBytesRX]; // Буфер прием при работе как Slave
u08 i2c_OutBuff[i2c_MasterBytesTX]; // Буфер передачи при работе как Slave
u08 i2c_SlaveIndex; // Индекс буфера Slave
 
 
u08 i2c_index; // Индекс этого буфера
u08 i2c_ByteCount; // Число байт передаваемых
 
u08 i2c_SlaveAddress; // Адрес подчиненного
 
// Указатели выхода из автомата:
IIC_F MasterOutFunc = &DoNothing; // в Master режиме
IIC_F SlaveOutFunc = &DoNothing; // в режиме Slave
IIC_F ErrorOutFunc = &DoNothing; // в результате ошибки в режиме Master
 
 
ISR(TWI_vect) // Прерывание TWI Тут наше все.
{
	printf("i2c -> %d\n", (TWSR & 0xF8));

    switch(TWSR & 0xF8) // Отсекаем биты прескалера
    {
    	case 0x00: // Bus Fail (автобус сломался)
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
 
	    case 0x68: // RCV SLA+W Low Priority // Словили свой адрес во время передачи мастером
	    case 0x78: // RCV SLA+W Low Priority (Broadcast) // Или это был широковещательный пакет. Не важно
	    {
		    i2c_Do |= i2c_ERR_LP | i2c_Interrupted; // Ставим флаг ошибки Low Priority, а также флаг того, что мастера прервали
 
 		    // Restore Trans after.
		    i2c_index = 0; // Подготовили прерваную передачу заново
		    i2c_PageAddrIndex = 0;
	    } // И пошли дальше. Внимание!!! break тут нет, а значит идем в "case 60"
 
	    case 0x60: // RCV SLA+W Incoming? // Или просто получили свой адрес
	    case 0x70: // RCV SLA+W Incoming? (Broascast) // Или широковещательный пакет
	    {
 
		    i2c_Do |= i2c_Busy; // Занимаем шину. Чтобы другие не совались
		    i2c_SlaveIndex = 0; // Указатель на начало буфера слейва, Неважно какой буфер. Не ошибемся
 
		    if (i2c_MasterBytesRX == 1) // Если нам суждено принять всего один байт, то готовимся принять его
		    {
			    TWCR = 0<<TWSTA|
					    0<<TWSTO|
					    1<<TWINT|
					    0<<TWEA|
					    1<<TWEN|
					    1<<TWIE; // Принять и сказать пошли все н... NACK!
		    }
		    else
		    {
			    TWCR = 0<<TWSTA|
					    0<<TWSTO|
					    1<<TWINT|
					    1<<TWEA|
					    1<<TWEN|
					    1<<TWIE; // А если душа шире чем один байт, то сожрем и потребуем еще ACK!
		    }
		    break;
	    }
 
	    case 0x80: // RCV Data Byte // И вот мы приняли этот байт. Наш или широковещательный. Не важно
	    case 0x90: // RCV Data Byte (Broadcast)
	    {
		    i2c_InBuff[i2c_SlaveIndex] = TWDR; // Сжираем его в буфер.
 
		    i2c_SlaveIndex++; // Сдвигаем указатель
 
		    if (i2c_SlaveIndex == i2c_MasterBytesRX-1) // Свободно место всего под один байт?
		    {
			    TWCR = 0<<TWSTA|
					    0<<TWSTO|
					    1<<TWINT|
					    0<<TWEA|
					    1<<TWEN|
					    1<<TWIE; // Приянть его и сказать NACK!
		    }
		    else
		    {
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
		    i2c_InBuff[i2c_SlaveIndex] = TWDR; // Сожрали его в буфер
 
		    if (i2c_Do & i2c_Interrupted) // Если у нас был прерываный сеанс от имени мастера
		    {
			    TWCR = 1<<TWSTA|
					    0<<TWSTO|
					    1<<TWINT|
					    1<<TWEA|
					    1<<TWEN|
					    1<<TWIE; // Влепим в шину свой Start поскорей и сделаем еще одну попытку
		    }
		    else
		    {
			    TWCR = 0<<TWSTA|
					    0<<TWSTO|
					    1<<TWINT|
					    1<<TWEA|
					    1<<TWEN|
					    1<<TWIE; // Если не было такого факта, то просто отвалимся и будем ждать
		    }
 
		    MACRO_i2c_WhatDo_SlaveOut // И лениво отработаем наш выходной экшн для слейва
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
 
 
 
	    case 0xB0: // Поймали свой адрес на чтение во время передачи Мастером
	    {
		    i2c_Do |= i2c_ERR_LP | i2c_Interrupted; // Ну чо, коды ошибки и флаг прерваной передачи.
 
 
		    // Восстанавливаем индексы
		    i2c_index = 0;
		    i2c_PageAddrIndex = 0;
 
	    } // Break нет! Идем дальше
 
	    case 0xA8: // // Либо просто словили свой адрес на чтение
	    {
		    i2c_SlaveIndex = 0; // Индексы слейвовых массивов на 0
 
		    TWDR = i2c_OutBuff[i2c_SlaveIndex]; // Чтож, отдадим байт из тех что есть.
 
		    if(i2c_MasterBytesTX == 1)
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
 
		    i2c_SlaveIndex++; // Значит продолжаем дискотеку. Берем следующий байт
		    TWDR = i2c_OutBuff[i2c_SlaveIndex]; // Даем его мастеру
 
		    if (i2c_SlaveIndex == i2c_MasterBytesTX-1) // Если он последний был, то
		    {
			    TWCR = 0<<TWSTA|
					    0<<TWSTO|
					    1<<TWINT|
					    0<<TWEA|
					    1<<TWEN|
					    1<<TWIE; // Шлем его и ждем NACK
		    }
		    else
		    {
			    TWCR = 0<<TWSTA|
					    0<<TWSTO|
					    1<<TWINT|
					    1<<TWEA|
					    1<<TWEN|
					    1<<TWIE; // Если нет, то шлем и ждем ACK
		    }
 
		    break;
	    }
 
	    case 0xC0: // Мы выслали последний байт, больше у нас нет, получили NACK
	    case 0xC8: // или ACK. В данном случае нам пох. Т.к. больше байтов у нас нет.
	    {
		    if (i2c_Do & i2c_Interrupted) // Если там была прерваная передача мастера
		    { // То мы ему ее вернем
			    i2c_Do &= i2c_NoInterrupted;
			    // Снимем флаг прерваности
			    TWCR = 1<<TWSTA|
					    0<<TWSTO|
					    1<<TWINT|
					    1<<TWEA|
					    1<<TWEN|
					    1<<TWIE; // Сгенерим старт сразу же как получим шину.
		    }
		    else
		    {
			    TWCR = 0<<TWSTA|
					    0<<TWSTO|
					    1<<TWINT|
					    1<<TWEA|
					    1<<TWEN|
					    1<<TWIE; // Если мы там одни, то просто отдадим шину
		    }
 
		    MACRO_i2c_WhatDo_SlaveOut // И отработаем выход слейва. Впрочем, он тут
		    // Не особо то нужен. Разве что как сигнал, что мастер
		    break; // Нас почтил своим визитом.
	    }
 
	    default: break;
    }
}

void DoNothing(void) // Функция пустышка, затыкать несуществующие ссылки
{
}
 
 
void Init_Slave_i2c() // Настройка режима слейва (если нужно)
{
	TWAR = 0x32; // Внесем в регистр свой адрес, на который будем отзываться.
	// 1 в нулевом бите означает, что мы отзываемся на широковещательные пакеты
//	SlaveOutFunc = Addr; // Присвоим указателю выхода по слейву функцию выхода
 
	TWCR = 0<<TWSTA|
		0<<TWSTO|
		0<<TWINT|
		1<<TWEA|
		1<<TWEN|
		1<<TWIE; // Включаем агрегат и начинаем слушать шину.
}
