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
    std::cout << "I2C bus inited." << std::endl;

    if (argc > 1 && strcmp(argv[1], "--stop") == 0)
    {
        motorController = new MotorController(i2cBus);
        motorController->init();
        return 0;
    }

    gyro = new Gyro(i2cBus);
    std::cout << "Gyro inited." << std::endl;

    accelerometer = new Accelerometer(i2cBus);
 //   accelerometer->setOffsetAngleXZ(-0.0678582755555555);
 //   accelerometer->setOffsetAngleYZ(-0.0242676311111111);
    accelerometer->setOffsetAngleXZ(-0.073);
    accelerometer->setOffsetAngleYZ(-0.015);
    std::cout << "Accelerometer inited." << std::endl;

    alfaBetaFilter = new AlfaBetaFilter(accelerometer, gyro);
    std::cout << "AlfaBetaFilter inited." << std::endl;

    radioControl = new RadioControl(i2cBus, 10);
    std::cout << "RadioControl inited." << std::endl;

    motorController = new MotorController(i2cBus);
    motorController->init();
    std::cout << "MotorController inited." << std::endl;

    std::cout << "MotorController collebration start." << std::endl;
    gyro->calibration(100);
    std::cout << "MotorController collebration started." << std::endl;

    int info = 0;
    int counter = 0;
    float u = 0.5;
    float k = 60;
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

    bool radioControlEnabled = true;
    bool radioControlInited = false;
    double dx = 0;
    double dy = 0;


    while(1)
    {

        alfaBetaFilter->refresh();
        radioControl->refresh();

        if (counter == 2)
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
                << ", dx: " << dx
                << ", dy: " << dy
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
                {
//                    if (motorController->getGear() < 1250)
//                        motorController->setGear(1250);
//                    else
                        motorController->setGear(motorController->getGear()+15);
                }
                else if (ch == 's')
                {
//                  if (motorController->getGear() < 1250)
//                      motorController->setGear(1100);
//                    else
                        motorController->setGear(motorController->getGear()-15);

                }
//                else if (ch == 'q')
//                   motorController->setGear(motorController->getGear()+5);
//                else if (ch == 'a')
//                    motorController->setGear(motorController->getGear()-5);
                else if (ch == ' ') // По пробелу предусмотрен экстренный останов.
                    motorController->emergencyShutdown();
                else if (ch == 'r')
                    u += 0.1;
                else if (ch == 'f')
                    u -= 0.1;
                else if (ch == 't')
                    k += 0.1;
                else if (ch == 'g')
                    k -= 0.1;
                else if (ch == 'c')
                {
                    if (radioControlEnabled == true)
                    {
                        std::cout << "Radio control OFF!" << std::endl;
                        radioControlEnabled = false;
                    }
                    else
                    {
                        std::cout << "Radio control ON!" << std::endl;
                        radioControlEnabled = true;
                    }
                }
                else if (ch == 'z')
                {
                    info++;
                    if (info == 5)
                        info = 0;
                }
            }
        }

        counter ++;

        dx = 0;
        dy = 0;

        // Радио управление инициализируется после того как заначение газа
        // будет мимимальным.
        if (radioControlInited == false &&
            radioControl->getState2() < 1120 &&
            radioControl->getState2() > 1090)
            radioControlInited = true;

        // Если включено радио управление испорльзуем данные с него
        if (radioControlEnabled == true && radioControlInited == true )
        {
            int gear = radioControl->getState2();
            if (gear < 1130)
                motorController->setGear(1100);
            else
                motorController->setGear(gear);

            if (radioControl->getState3() < 1450)
                dx = (1450 - radioControl->getState3()) / 50 * (3.14/180);
            else if (radioControl->getState3() > 1550)
                dx = (radioControl->getState3() - 1550) / 50 * (3.14/180);
            else
                dx = 0;

            if (radioControl->getState4() < 1450)
                dy = (1450 - radioControl->getState4()) / 50 * (3.14/180);
            else if (radioControl->getState4() > 1550)
                dy = (radioControl->getState4() - 1550) / 50 * (3.14/180);
            else
                dy = 0;
        }
        else // если не используем, то оставим только аварийное отключение.
        {
            // Аварийное выключение по пульту. Если газ на пульте будет больше
            // 1500, то происходит отключение.
            if (radioControl->getState2() > 1500)
                motorController->emergencyShutdown();

        }


        if (motorController->getGear() > 1140)
        {
            xzFactor = u*alfaBetaFilter->getAngularVelocityXZ() + k*(alfaBetaFilter->getAngleXZ()-dy);
            yzFactor = u*alfaBetaFilter->getAngularVelocityYZ() + k*(alfaBetaFilter->getAngleYZ()-dx);
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
