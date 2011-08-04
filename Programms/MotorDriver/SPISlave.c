#include <stdio.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/io.h>


void SPISlaveInit(void)
{
	/* Set MISO output, all others input */
    // Set PB6(MISO) as output 
    DDRB    = (1<<PB6);
	/* Enable SPI */
	SPCR = (1<<SPE)|(1<<SPIE);
}

ISR(SPI_STC_vect)
{
	int data = SPDR;
	printf("SPI intrupt: %d\n", data);
}
