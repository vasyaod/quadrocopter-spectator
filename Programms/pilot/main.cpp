#include <iostream>
#include <fcntl.h>
#include <sys/time.h>

#include <termios.h>
#include <unistd.h>

#include "I2C.h"
#include "Gyro.h"
#include "MotorController.h"
#include "Accelerometer.h"
#include "AlfaBetaFilter.h"
#include "RadioControl.h"

I2CBus *i2cBus;
Gyro *gyro;
Accelerometer *accelerometer;
MotorController *motorController;
AlfaBetaFilter *alfaBetaFilter;
RadioControl *radioControl;


int kbhit(void)
{
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if(ch != EOF)
    {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}

int main(int argc, const char* argv[])
{
    i2cBus = new I2CBus("/dev/i2c-0");
    i2cBus->open();

    if (argc > 1 && strcmp(argv[1], "--stop") == 0)
    {
        motorController = new MotorController(i2cBus);
        motorController->init();
        return 0;
    }

    gyro = new Gyro(i2cBus);

    accelerometer = new Accelerometer(i2cBus);
 //   accelerometer->setOffsetAngleXZ(-0.0678582755555555);
 //   accelerometer->setOffsetAngleYZ(-0.0242676311111111);
    accelerometer->setOffsetAngleXZ(-0.073);
    accelerometer->setOffsetAngleYZ(-0.015);

    alfaBetaFilter = new AlfaBetaFilter(accelerometer, gyro);

    radioControl = new RadioControl(i2cBus, 10);

    motorController = new MotorController(i2cBus);
    motorController->init();

    gyro->calibration(100);

    int info = 0;
    int counter = 0;
    float u = 70;
    float k = 20;
    float angleXZ = 0;
    float angleYZ = 0;
    double xzFactor = 0;
    double yzFactor = 0;

    double avgAccelerometerAngleXZ = 0;
    double avgAccelerometerAngleYZ = 0;
    int accelerometerAngleCount = 0;

    double avgFilterAngleXZ = 0;
    double avgFilterAngleYZ = 0;
    int filterAngleCount = 0;

    while(1)
    {

        alfaBetaFilter->refresh();
        radioControl->refresh();

        if (counter == 5)
        {
            counter = 0;
            if (info == 0)
            {
                avgFilterAngleXZ += alfaBetaFilter->getAngleXZ();
                avgFilterAngleYZ += alfaBetaFilter->getAngleYZ();
                filterAngleCount++;

                std::cout << "Filter status: "
                << " angularVelocityXZ: " << alfaBetaFilter->getAngularVelocityXZ()
                << ", angularVelocityYZ: " << alfaBetaFilter->getAngularVelocityYZ()
                << ", angleXZ: " << alfaBetaFilter->getAngleXZ()
                << ", angleYZ: " << alfaBetaFilter->getAngleYZ()
                << ", avgAngleXZ: " << (avgFilterAngleXZ/filterAngleCount)
                << ", avgAngleYZ: " << (avgFilterAngleYZ/filterAngleCount)
                << ", newC: " << alfaBetaFilter->getDeltaT()
                << std::endl;
            }
            if (info == 1)
            {
                std::cout << "Motor status: "
                << " gear: " << motorController->getGear()
                << ", u: " << u
                << ", k: " << k
                << ", motor1: " << motorController->getMotor1State()
                << ", motor2: " << motorController->getMotor2State()
                << ", motor3: " << motorController->getMotor3State()
                << ", motor4: " << motorController->getMotor4State()
                << ", angleXZs: " << alfaBetaFilter->getGradusAngleXZ()
                << ", angleYZs: " << alfaBetaFilter->getGradusAngleYZ()
                << std::endl;
            }
            if (info == 2)
            {
                std::cout << "Control status: "
                << ", servo1: " << radioControl->getState1()
                << ", servo2: " << radioControl->getState2()
                << ", servo3: " << radioControl->getState3()
                << ", servo4: " << radioControl->getState4()
                << std::endl;
            }
            if (info == 3)
            {
                avgAccelerometerAngleXZ += accelerometer->getAngleXZ();
                avgAccelerometerAngleYZ += accelerometer->getAngleYZ();
                accelerometerAngleCount++;

                std::cout << "Accelerometer status: "
                << ", angleXZ: " << accelerometer->getAngleXZ()
                << ", angleYZ: " << accelerometer->getAngleYZ()
                << ", avgAngleXZ: " << (avgAccelerometerAngleXZ/accelerometerAngleCount)
                << ", avgAngleYZ: " << (avgAccelerometerAngleYZ/accelerometerAngleCount)
                << std::endl;
            }
            if (info == 4)
            {
                std::cout << "Gyro status: "
                << ", angularVelocityXZ: " << gyro->getAngularVelocityXZ()
                << ", angularVelocityYZ: " << gyro->getAngularVelocityYZ()
                << std::endl;
            }


            if (kbhit() != 0)
            {
                char ch = getchar();
                if (ch == 'w')
                    motorController->setGear(motorController->getGear()+15);
                else if (ch == 's')
                    motorController->setGear(motorController->getGear()-15);
                else if (ch == ' ') // По пробелу предусмотрен экстренный останов.
                    motorController->emergencyShutdown();
                else if (ch == 'z')
                {
                    info++;
                    if (info == 5)
                        info = 0;
                }
            }
        }
        counter ++;

        // Аварийное выключение по пульту.
        if (radioControl->getState2() > 1500)
            motorController->emergencyShutdown();

        if (motorController->getGear() > 1500)
        {
            xzFactor = u*alfaBetaFilter->getAngularVelocityXZ() + k*alfaBetaFilter->getAngleXZ();
            yzFactor = u*alfaBetaFilter->getAngularVelocityYZ() + k*alfaBetaFilter->getAngleYZ();
          //  xzFactor += alfaBetaFilter->getGradusAngleXZ()*engleFactor;
          //  yzFactor += alfaBetaFilter->getGradusAngleYZ()*engleFactor;
        }
        else
        {
            xzFactor = 0;
            yzFactor = 0;
        }

        motorController->setXZFactor(xzFactor);
        motorController->setYZFactor(yzFactor);
        motorController->update();
        usleep(1000);
    }

    return 0;
}
