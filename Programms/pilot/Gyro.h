#ifndef _PILOT_GYRO_H
#define _PILOT_GYRO_H

class I2CBus;

class Gyro
{
    private:
        I2CBus *i2cBus;
        int angularVelocityX;      // Угловая скорость вокруг оси X.
        int angularVelocityY;      // Угловая скорость вокруг оси Y.

        struct AngularVelocityBuffer
        {
            unsigned short value1;
            unsigned short value2;
        } angularVelocityBuffer;

    public:
        Gyro(I2CBus * i2cBus);

        void refresh();     // Метод обновляет заначения скоростей считывая их из
                            // контроллера.
        int getAngularVelocityX();  // Угловая скорость вокруг оси X.
        int getAngularVelocityY();  // Угловая скорость вокруг оси Y.
};

#endif // _PILOT_GYRO_H
