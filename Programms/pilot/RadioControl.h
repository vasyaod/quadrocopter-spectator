#ifndef _PILOT_RADIO_CONTROL_H
#define _PILOT_RADIO_CONTROL_H

class I2CBus;

class RadioControl
{
    private:
        I2CBus *i2cBus;

        int deviceAddress;      // Адрес устройства.

        struct ControlStateBuffer
        {
            unsigned short value0;
            unsigned short value1;
            unsigned short value2;
            unsigned short value3;
            unsigned short value4;
            unsigned short value5;
        };

        int avgCount;
        struct ControlStateBuffer **controlStateBufferList;

    public:
        RadioControl(I2CBus *i2cBus);
        RadioControl(I2CBus *i2cBus, int avgCount);

        void refresh();

        int getState1();
        int getState2();
        int getState3();
        int getState4();
};

#endif // _PILOT_RADIO_CONTROL_H
