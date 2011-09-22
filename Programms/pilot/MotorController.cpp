
#include "I2C.h"
#include "MotorController.h"

MotorController::MotorController(I2CBus * i2cBus)
{
    this->i2cBus = i2cBus;
}

void MotorController::update()
{
    if (off == 0)
    {
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
    motorStateBuffer.value0 = motorValue2;
    motorStateBuffer.value1 = motorValue1;
    motorStateBuffer.value3 = motorValue4;
    motorStateBuffer.value4 = motorValue3;
    if (i2cBus->write(0x19, 0, &motorStateBuffer, 12) < 0)
      return;
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
