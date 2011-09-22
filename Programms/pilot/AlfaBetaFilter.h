#ifndef _PILOT_ALFA_BETA_FILTER_H
#define _PILOT_ALFA_BETA_FILTER_H

class Accelerometer;
class Gyro;

class AlfaBetaFilter
{
    private:
        Accelerometer *accelerometer;
        Gyro *gyro;

        long time;
        float deltaT;
        float angleXZ;
        float angleYZ;
        float angularVelocityXZ;
        float angularVelocityYZ;

    public:
        AlfaBetaFilter(Accelerometer *accelerometer, Gyro *gyro);

        void refresh();

        float getAngleXZ();
        float getAngleYZ();

        float getGradusAngleXZ();
        float getGradusAngleYZ();

        float getAngularVelocityXZ();
        float getAngularVelocityYZ();

        float getDeltaT();
};

#endif // _PILOT_ALFA_BETA_FILTER_H
