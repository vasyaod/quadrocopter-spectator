#ifndef I2C_H
#define I2C_H
 
#include <avr/io.h>
#include <avr/interrupt.h>
#include "avrtypedef.h"

typedef void (*i2c_write_f)(u08, u08);
typedef u08 (*i2c_read_f)(u08);
 
void i2c_init_slave(u08, u08, i2c_write_f, i2c_read_f);
 
#endif
