#ifndef _PILOT_MOTOR_CONTROLLER_H
#define _PILOT_MOTOR_CONTROLLER_H

class I2CBus;

class MotorController
{
    private:
        I2CBus *i2cBus;

        bool inited;
        bool refreshed;

        unsigned short motorValue1;
        unsigned short motorValue2;
        unsigned short motorValue3;
        unsigned short motorValue4;

        int gear;
        int xzFactor;
        int yzFactor;

        int off;

        struct MotorStateBuffer
        {
            unsigned short value0;
            unsigned short value1;
            unsigned short value2;
            unsigned short value3;
            unsigned short value4;
            unsigned short value5;
        } motorStateBuffer;

    public:

        MotorController(I2CBus * i2cBus);

        void update();      // Метод обновляет состояние двигателей.

        void emergencyShutdown();       // Функция экстренного отключения.

        void init();        // Метод инициализирует двигатели, но не запускает из.
        bool isInited();    // Метод возвращает true если контроллер двигателей
                            // инициализирован.
        void setGear(int value);    // Метод устанавливает значение газа.
        int getGear();              // Метод возвращает.

        void setXZFactor(int value);  //
        void setYZFactor(int value);  //

        int getMotor1State();
        int getMotor2State();
        int getMotor3State();
        int getMotor4State();
};

#endif // _PILOT_MOTOR_CONTROLLER_H
