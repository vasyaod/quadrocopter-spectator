#include <math.h>
#include <fcntl.h>
#include <sys/time.h>

#include <termios.h>
#include <unistd.h>

#include "I2C.h"
#include "Gyro.h"

Gyro::Gyro(I2CBus * i2cBus)
{
    deviceAddress = 0x19;
    this->i2cBus = i2cBus;
}

void Gyro::refresh()
{
    if (i2cBus->read(deviceAddress, 12, &angularVelocityBuffer, 4) < 0)
        return;
    angularVelocityX = angularVelocityBuffer.value2;
    angularVelocityY = angularVelocityBuffer.value1;
}

void Gyro::calibration(int sample)
{
    int i = 0;
    double sumAngularVelocityX = 0;
    double sumAngularVelocityY = 0;
    while(i < sample)
    {
        refresh();
        sumAngularVelocityX += angularVelocityX;
        sumAngularVelocityY += angularVelocityY;
        usleep(20000);
        i++;
    }
    offsetAngularVelocityX = sumAngularVelocityX / i;
    offsetAngularVelocityY = sumAngularVelocityY / i;
}

float Gyro::getAngularVelocityXZ()
{
    return getGradusAngularVelocityXZ()/180*M_PI;
}

float Gyro::getAngularVelocityYZ()
{
    return getGradusAngularVelocityYZ()/180*M_PI;
}

float Gyro::getGradusAngularVelocityXZ()
{
    float v = (float)(angularVelocityY - offsetAngularVelocityY)*5/1024;
    return v/0.0033;
}

float Gyro::getGradusAngularVelocityYZ()
{
    float v = (float)(angularVelocityX - offsetAngularVelocityX)*5/1024;
    return v/0.0033;
}
