#ifndef _PILOT_GYRO_H
#define _PILOT_GYRO_H

class I2CBus;

class Gyro
{
    private:
        I2CBus *i2cBus;

        int deviceAddress;        // Адрес устройства.

        int angularVelocityX;      // Угловая скорость вокруг оси X.
        int angularVelocityY;      // Угловая скорость вокруг оси Y.

        double offsetAngularVelocityX;
        double offsetAngularVelocityY;

        struct AngularVelocityBuffer
        {
            unsigned short value1;
            unsigned short value2;
        } angularVelocityBuffer;

    public:
        Gyro(I2CBus * i2cBus);

        void calibration(int sample);    // Производит калибровку гироскопа при этом
                                         // квадрокоптер должен остоваться неподвижным.

        void refresh();     // Метод обновляет заначения скоростей считывая их из
                            // контроллера.
        float getAngularVelocityYZ();  // Угловая скорость вокруг оси X (рад/с).
        float getAngularVelocityXZ();  // Угловая скорость вокруг оси Y (рад/с).
        float getGradusAngularVelocityYZ();  // Угловая скорость вокруг оси X (градус/с).
        float getGradusAngularVelocityXZ();  // Угловая скорость вокруг оси Y (градус/с).
};

#endif // _PILOT_GYRO_H
