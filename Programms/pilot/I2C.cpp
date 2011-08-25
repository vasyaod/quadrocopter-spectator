#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include "I2C.h"

I2CBus::I2CBus(char * deviceName)
{
    this->deviceName = deviceName;
}

int I2CBus::open()
{
    file = ::open(deviceName, O_RDWR);
    if (file < 0) {
        return file;
    }
    return 0;

}

int I2CBus::close()
{
//    ::close(file);
    return 0;
}

int I2CBus::read(int device_address, int address, void *data, int len)
{
    struct i2c_msg             msgs[2];
    struct i2c_rdwr_ioctl_data msgset;
    int rc;

    /* Write Message */
    msgs[0].addr  = device_address;
    msgs[0].flags = 0;
    msgs[0].buf   = (__u8 *)&address;
    msgs[0].len   = 1;

    /* Read Message */
    msgs[1].addr  = device_address;
    msgs[1].flags = I2C_M_RD;
    msgs[1].buf   = (__u8 *)data;
    msgs[1].len   = len;

    msgset.msgs  = msgs;
    msgset.nmsgs = 2;

    rc = ::ioctl( file, I2C_RDWR , &msgset );
    return rc;
}

int I2CBus::write(int device_address, int address, void *data, int len)
{
    struct i2c_msg             msgs[2];
    struct i2c_rdwr_ioctl_data msgset;
    int rc;

    /* Write Message */
    msgs[0].addr  = device_address;
    msgs[0].flags = 0;
    msgs[0].buf   = (__u8 *)&address;
    msgs[0].len   = 1;

    /* Read Message */
    msgs[1].addr  = device_address;
    msgs[1].flags = 0;
    msgs[1].buf   = (__u8 *)data;
    msgs[1].len   = len;

    msgset.msgs  = msgs;
    msgset.nmsgs = 2;

    rc = ::ioctl( file, I2C_RDWR , &msgset );
    return rc;
}
