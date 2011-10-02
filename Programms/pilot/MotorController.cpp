#include <stdio.h>
#include <iostream>

#include "I2C.h"
#include "MotorController.h"

MotorController::MotorController(I2CBus * i2cBus)
{
    this->i2cBus = i2cBus;
}

void MotorController::update()
{
    int l12 = 1, l23 = 1, l43 = 1, l14 = 1;

    if (off == 0)
    {
//        double f = gear;
//        double mx = yzFactor;
//        double my = xzFactor;
//        double z = (l43+l12)*(l23+3*l14);
//        motorValue1 = -0.5 * (-12*l23*l43*f - 4*l23*l12*f + 4*l14*l12*f - 4*l14*l43*f + 2*l12*mx + l23*my + 3*l14*my + 4*mx*l43) / z;
//        motorValue2 = 0.5 * (4*l23*l43*f - 4*l23*l12*f + 4*l14*l12*f + 12*l14*l43*f + 2*l12*mx - l23*my - 3*l14*my) / z;
//        motorValue3 = 0.5 * (-4*l23*l43*f + 4*l23*l12*f + l23*my + 4*l14*l43*f + 12*l14+l12*f + 3*l14*my + 2*mx*l43) / z;
//        motorValue4 = 0.5 * (4*l23*l43*f + 12*l23*l12*f + 4*l14*l12*f - 4*l14*l43*f -4*l12*mx + l23*my + 3*l14*my - 2*l43*mx) / z;

        motorValue1 = gear + yzFactor + xzFactor;
        motorValue2 = gear - yzFactor + xzFactor;
        motorValue3 = gear - yzFactor - xzFactor;
        motorValue4 = gear + yzFactor - xzFactor;
    }
    else
    {
        motorValue1 = 1100;
        motorValue2 = 1100;
        motorValue3 = 1100;
        motorValue4 = 1100;
    }

    if (motorStateBuffer.value0 == motorValue2 &&
        motorStateBuffer.value1 == motorValue1 &&
        motorStateBuffer.value3 == motorValue4 &&
        motorStateBuffer.value4 == motorValue3)
        return;


/*
    if (i2cBus->write(0x19, 0, &motorStateBuffer, 12) < 0)
    {
        std::cout << "MotorController buffer write error! " << std::endl;
        printf("MotorController buffer write error!");
        return;
    }
*/
    if (i2cBus->write(0x19, 0, &motorValue2, 2) < 0)
        return;
    if (i2cBus->write(0x19, 2, &motorValue1, 2) < 0)
        return;
    if (i2cBus->write(0x19, 6, &motorValue4, 2) < 0)
        return;
    if (i2cBus->write(0x19, 8, &motorValue3, 2) < 0)
        return;

    motorStateBuffer.value0 = motorValue2;
    motorStateBuffer.value1 = motorValue1;
    motorStateBuffer.value3 = motorValue4;
    motorStateBuffer.value4 = motorValue3;

    int d = 1;
    // Что бы подтвердить транзаю запишем любое значение по адресу 0x20
    if (i2cBus->write(0x19, 0x20, &d, 2) < 0)
    {
        std::cout << "MotorController buffer write error! " << std::endl;
        printf("MotorController transaction flag write error!");
        return;
    }
}

void MotorController::init()
{
    gear = 1100;
    update();
    inited = true;
}

bool MotorController::isInited()
{
    return inited;
}

void MotorController::setGear(int value)
{
    gear = value;
    update();
}

int MotorController::getGear()
{
    return gear;
}

void MotorController::setXZFactor(int value)
{
    xzFactor = value;
}

void MotorController::setYZFactor(int value)
{
    yzFactor = value;
}

int MotorController::getMotor1State()
{
    return motorValue1;
}

int MotorController::getMotor2State()
{
    return motorValue2;
}

int MotorController::getMotor3State()
{
    return motorValue3;
}

int MotorController::getMotor4State()
{
    return motorValue4;
}

void MotorController::emergencyShutdown()
{
    off = 1;
    update();
}
