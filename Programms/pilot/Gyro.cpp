#include "I2C.h"
#include "Gyro.h"

Gyro::Gyro(I2CBus * i2cBus)
{
    this->i2cBus = i2cBus;
}

void Gyro::refresh()
{
    if (i2cBus->read(0x19, 12, &angularVelocityBuffer, 4) < 0)
        return;
    angularVelocityX = angularVelocityBuffer.value2;
    angularVelocityY = angularVelocityBuffer.value1;
}

int Gyro::getAngularVelocityX()
{
    return angularVelocityX;
}

int Gyro::getAngularVelocityY()
{
    return angularVelocityY;
}
