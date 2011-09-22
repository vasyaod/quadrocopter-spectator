#include <math.h>
#include <time.h>
#include <sys/timeb.h>

#include "Accelerometer.h"
#include "Gyro.h"
#include "AlfaBetaFilter.h"

AlfaBetaFilter::AlfaBetaFilter(Accelerometer *accelerometer, Gyro *gyro)
{
    this->accelerometer = accelerometer;
    this->gyro = gyro;

    struct timeb tp;
    ftime(&tp);
    time = tp.time*1000 + tp.millitm;
}

void AlfaBetaFilter::refresh()
{
    gyro->refresh();
    accelerometer->refresh();

    long oldTime = time;

    struct timeb tp;
    ftime(&tp);
    time = tp.time*1000 + tp.millitm;

    deltaT = (time - oldTime)/1000.0;
    float teta = (1-0.2)/0.2 * deltaT;

    float oldAngleXZ = angleXZ;
    float oldAngleYZ = angleYZ;

    angleXZ = (1-teta)*(angleXZ+gyro->getAngularVelocityXZ()*deltaT) + teta*accelerometer->getAngleXZ();
    angleYZ = (1-teta)*(angleYZ+gyro->getAngularVelocityYZ()*deltaT) + teta*accelerometer->getAngleYZ();

    angularVelocityXZ = (angleXZ - oldAngleXZ)/deltaT;
    angularVelocityYZ = (angleYZ - oldAngleYZ)/deltaT;
}

float AlfaBetaFilter::getAngleXZ()
{
    return angleXZ;
}

float AlfaBetaFilter::getAngleYZ()
{
    return angleYZ;
}

float AlfaBetaFilter::getGradusAngleXZ()
{
    return angleXZ/M_PI*180;
}

float AlfaBetaFilter::getGradusAngleYZ()
{
    return angleYZ/M_PI*180;
}

float AlfaBetaFilter::getAngularVelocityXZ()
{
    return angularVelocityXZ;
}

float AlfaBetaFilter::getAngularVelocityYZ()
{
    return angularVelocityYZ;
}

float AlfaBetaFilter::getDeltaT()
{
    return deltaT;
}
