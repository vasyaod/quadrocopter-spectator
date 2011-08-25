#include "i2C.h"
#include "Gyro.h"

Gyro::Gyro(I2CBus * i2cBus)
{
    this->i2cBus = i2cBus;
}

void Gyro::refresh()
{
    if (i2cBus->read(0x19, 12, &angularVelocityByffer, 4) < 0)
        return;
    angularVelocityX = angularVelocityByffer.value2;
    angularVelocityY = angularVelocityByffer.value1;
}

int Gyro::getAngularVelocityX()
{
    return angularVelocityX;
}

int Gyro::getAngularVelocityY()
{
    return angularVelocityY;
}
