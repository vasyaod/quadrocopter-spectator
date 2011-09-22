#ifndef _PILOT_ACCELEROMETER_H
#define _PILOT_ACCELEROMETER_H

class I2CBus;

class Accelerometer
{
    private:
        I2CBus *i2cBus;

        int deviceAddress;      // Адрес устройства.
        float offsetAngleXZ;     // Смещение угла акселирометра в плоскости XZ.
        float offsetAngleYZ;     // Смещение угла акселирометра в плоскости YZ.

        // Буффер куда будут записываться значения акселерометра.
        struct ForceBuffer
        {
            unsigned char forceXL;
            unsigned char forceXH;
            unsigned char forceYL;
            unsigned char forceYH;
            unsigned char forceZL;
            unsigned char forceZH;
        } forceBuffer;

    public:
        Accelerometer(I2CBus * i2cBus);

        void refresh();     // Обновляет значения аксерерометра.

        short getForceX();    // Заначение акселерметра.
        short getForceY();    // Заначение акселерметра.
        short getForceZ();    // Заначение акселерметра.

        float getAngleXZ();   // Угол наклона акселирометра.
        float getAngleYZ();   // Угол наклона акселирометра.

        void setOffsetAngleXZ(float offsetAngleXZ);
        void setOffsetAngleYZ(float offsetAngleYZ);
};

#endif // _PILOT_ACCELEROMETER_H
