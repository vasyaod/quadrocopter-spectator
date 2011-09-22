#include <math.h>
#include <stdlib.h>

#include "I2C.h"
#include "RadioControl.h"

RadioControl::RadioControl(I2CBus * i2cBus)
{
    deviceAddress = 0x19;
    this->i2cBus = i2cBus;

    avgCount = 1;
    controlStateBufferList = (ControlStateBuffer **)malloc(sizeof(void*)*avgCount);
    for(int i = 0; i < avgCount; i++)
        controlStateBufferList[i] = (ControlStateBuffer *)malloc(sizeof(ControlStateBuffer));
}

RadioControl::RadioControl(I2CBus * i2cBus, int avgCount)
{
    deviceAddress = 0x19;
    this->i2cBus = i2cBus;

    this->avgCount = avgCount;
    controlStateBufferList = (ControlStateBuffer **)malloc(sizeof(void*)*avgCount);
    for(int i = 0; i < avgCount; i++)
        controlStateBufferList[i] = (ControlStateBuffer *)malloc(sizeof(ControlStateBuffer));
}

void RadioControl::refresh()
{
    ControlStateBuffer *first = controlStateBufferList[0];
    for(int i = 1; i < avgCount; i++)
        controlStateBufferList[i-1] = controlStateBufferList[i];

    if (i2cBus->read(deviceAddress, 0, first, 12) < 0)
        return;

    controlStateBufferList[avgCount-1] = first;
}

int RadioControl::getState1()
{
    int res = 0;
    for(int i = 0; i < avgCount; i++)
        res += controlStateBufferList[i]->value1;

    return res/avgCount;
}

int RadioControl::getState2()
{
    int res = 0;
    for(int i = 0; i < avgCount; i++)
        res += controlStateBufferList[i]->value2;

    return res/avgCount;
}

int RadioControl::getState3()
{
    int res = 0;
    for(int i = 0; i < avgCount; i++)
        res += controlStateBufferList[i]->value3;

    return res/avgCount;
}

int RadioControl::getState4()
{
    int res = 0;
    for(int i = 0; i < avgCount; i++)
        res += controlStateBufferList[i]->value4;

    return res/avgCount;
}
