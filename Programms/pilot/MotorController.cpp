
#include "I2C.h"
#include "MotorController.h"

MotorController::MotorController(I2CBus * i2cBus)
{
    this->i2cBus = i2cBus;
}

void MotorController::update()
{
    motorValue1 = gear;
    motorValue2 = gear;
    motorValue3 = gear;
    motorValue4 = gear;

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

