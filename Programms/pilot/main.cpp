#include <iostream>
#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include "I2C.h"
#include "Gyro.h"

I2CBus *i2cBus;
Gyro *gyro;

int main()
{
    __u8 reg = 0x19; /* Device register to access */
    __s32 res;

    short test;

    i2cBus = new I2CBus("/dev/i2c-0");
    i2cBus->open();
    i2cBus->read(reg, 12, &test, 2);
    gyro = new Gyro(i2cBus);
    while(1)
    {
        gyro->refresh();
        std::cout << "Gyro: "
        << " angularVelocityX: " << gyro->getAngularVelocityX()
        << " angularVelocityY: " << gyro->getAngularVelocityY()
        << std::endl;

        usleep(100000);
    }
    return 0;
}
