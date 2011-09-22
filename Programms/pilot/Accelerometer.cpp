#include <math.h>

#include "I2C.h"
#include "Accelerometer.h"

Accelerometer::Accelerometer(I2CBus * i2cBus)
{
    deviceAddress = 0x1D;
    this->i2cBus = i2cBus;
    // Для того чтобы вывести акселироме из экономичного режима
    // надо сделать запись в регистр CTRL_REG1, а именно заполнить
    // биты PD1,PD0,Zen,Yen,Xen.    11000111b = 0xC7
    unsigned char data;
    data = 0xC7;
    i2cBus->write(deviceAddress, 0x20, &data, 1);
    data = 0x01;
    i2cBus->write(deviceAddress, 0x21, &data, 1);
}

void Accelerometer::refresh()
{
    // Данные акселиромера хранятся по адресу 0x28.
//    if (i2cBus->read(device_address, 0x28, &forceBuffer, 6) < 0)
//        return;
    i2cBus->read(deviceAddress, 0x28, &forceBuffer.forceXL, 1);
    i2cBus->read(deviceAddress, 0x29, &forceBuffer.forceXH, 1);
    i2cBus->read(deviceAddress, 0x2A, &forceBuffer.forceYL, 1);
    i2cBus->read(deviceAddress, 0x2B, &forceBuffer.forceYH, 1);
    i2cBus->read(deviceAddress, 0x2C, &forceBuffer.forceZL, 1);
    i2cBus->read(deviceAddress, 0x2D, &forceBuffer.forceZH, 1);

}

short Accelerometer::getForceX()
{
    return ((short)(((forceBuffer.forceXH) << 8) | forceBuffer.forceXL))/16;
}

short Accelerometer::getForceY()
{
    return ((short)(((forceBuffer.forceYH) << 8) | forceBuffer.forceYL))/16;
}

short Accelerometer::getForceZ()
{
    return ((short)(((forceBuffer.forceZH) << 8) | forceBuffer.forceZL)/16);
}

float Accelerometer::getAngleXZ()
{
    return -atan2((double)getForceX(), (double)getForceZ()) + offsetAngleXZ;
}

float Accelerometer::getAngleYZ()
{
    return -atan2((double)getForceY(), (double)getForceZ()) + offsetAngleYZ;
}

void Accelerometer::setOffsetAngleXZ(float offsetAngleXZ)
{
    this->offsetAngleXZ = offsetAngleXZ;
}

void Accelerometer::setOffsetAngleYZ(float offsetAngleYZ)
{
    this->offsetAngleYZ = offsetAngleYZ;
}

