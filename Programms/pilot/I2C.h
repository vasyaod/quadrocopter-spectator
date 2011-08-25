#ifndef _PILOT_I2C_H
#define _PILOT_I2C_H
#include <linux/types.h>

class I2CBus
{
    private:
        char *deviceName;
        int file;

    public:
        int open();
        int close();

        int read(int device_address, int address, void *data, int len);
        int write(int device_address, int address, void *data, int len);

        I2CBus(char * deviceName);
};

#endif // _PILOT_I2C_H
