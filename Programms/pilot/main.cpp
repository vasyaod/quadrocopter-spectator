#include <iostream>
#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include <termios.h>
#include <unistd.h>

#include "I2C.h"
#include "Gyro.h"
#include "MotorController.h"
#include "Accelerometer.h"

I2CBus *i2cBus;
Gyro *gyro;
Accelerometer *accelerometer;
MotorController *motorController;

int kbhit(void)
{
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if(ch != EOF)
    {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}

int main()
{
    i2cBus = new I2CBus("/dev/i2c-0");
    i2cBus->open();
    gyro = new Gyro(i2cBus);

    accelerometer = new Accelerometer(i2cBus);
    accelerometer->setOffsetAngleXZ(-0.08145);
    accelerometer->setOffsetAngleYZ(-0.027);

    motorController = new MotorController(i2cBus);
    motorController->init();

    while(1)
    {
        gyro->refresh();
        accelerometer->refresh();
        std::cout << "Status: "
        << ", gear: " << motorController->getGear()
        << ", angularVelocityY: " << gyro->getAngularVelocityY()
        << ", angularVelocityY: " << gyro->getAngularVelocityY()
        << ", fX: " << accelerometer->getForceX()
        << ", fY: " << accelerometer->getForceY()
        << ", fZ: " << accelerometer->getForceZ()
        << ", fXZ: " << accelerometer->getAngleXZ()
        << ", fYZ: " << accelerometer->getAngleYZ()
        << std::endl;

        if (kbhit() != 0)
        {
            char ch = getchar();
            if (ch == 'w')
                motorController->setGear(motorController->getGear()+10);
            else if (ch == 's')
                motorController->setGear(motorController->getGear()-10);
            else if (ch == ' ') // По пробелу предусмотрен экстренный останов.
                motorController->setGear(1100);
        }

        usleep(100000);
    }

    return 0;
}
