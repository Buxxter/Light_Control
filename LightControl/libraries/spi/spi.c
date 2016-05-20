/*
    Copyright (c) 2007 Stefan Engelke <mbox@stefanengelke.de>

    Permission is hereby granted, free of charge, to any person 
    obtaining a copy of this software and associated documentation 
    files (the "Software"), to deal in the Software without 
    restriction, including without limitation the rights to use, copy, 
    modify, merge, publish, distribute, sublicense, and/or sell copies 
    of the Software, and to permit persons to whom the Software is 
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be 
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
    DEALINGS IN THE SOFTWARE.

    $Id$
*/

#include "spi.h"
#include "../hardware.h"

#include <avr/io.h>
#include <avr/interrupt.h>




void spi_init()
// Initialize pins for spi communication
{
    SPI_DDR &= ~((1<<SPI_MOSI)|(1<<SPI_MISO)|(1<<SPI_SS)|(1<<SPI_SCK));
    // Define the following pins as output
    SPI_DDR |= ((1<<SPI_MOSI)|(1<<SPI_SS)|(1<<SPI_SCK));

    
    SPCR = ((1<<SPE)|               // SPI Enable
            (0<<SPIE)|              // SPI Interupt Enable
            (0<<DORD)|              // Data Order (0:MSB first / 1:LSB first)
            (1<<MSTR)|              // Master/Slave select   
            (0<<SPR1)|(1<<SPR0)|    // SPI Clock Rate
            (0<<CPOL)|              // Clock Polarity (0:SCK low / 1:SCK hi when idle)
            (0<<CPHA));             // Clock Phase (0:leading / 1:trailing edge sampling)

    SPSR = (1<<SPI2X);              // Double Clock Rate
    
	//| SPI2X | SPR1  | SPR0  | SCK Frequency |
	//|   0   |   0   |   0   |   fosc/4      |
	//|   0   |   0   |   1   |   fosc/16     |     
	//|   0   |   1   |   0   |   fosc/64     |
	//|   0   |   1   |   1   |   fosc/128    |
	//|   1   |   0   |   0   |   fosc/2      |
	//|   1   |   0   |   1   |   fosc/8      |
	//|   1   |   1   |   0   |   fosc/32     |
	//|   1   |   1   |   1   |   fosc/64     |
}

void spi_transfer_sync (uint8_t * data_send, uint8_t * data_receive, uint8_t len)
// Shift full array through target device
{
	#ifdef _DRIVE_SS
	cbit(SPI_PORT, SPI_SS);
	#endif
		
    uint8_t i;
    for (i = 0; i < len; i++) {
	    SPDR = data_send[i];
	    while((SPSR & (1<<SPIF))==0);
	    data_receive[i] = SPDR;
    }
	
	#ifdef _DRIVE_SS
	sbit(SPI_PORT, SPI_SS);
	#endif
	
}

void spi_transmit_sync (uint8_t * dataout, uint8_t len)
// Shift full array to target device without receiving any byte
{
	#ifdef _DRIVE_SS
	cbit(SPI_PORT, SPI_SS);
	#endif
       
	uint8_t i;      
    for (i = 0; i < len; i++) {
            SPDR = dataout[i];
            while((SPSR & (1<<SPIF))==0);
    }
	
	#ifdef _DRIVE_SS
	sbit(SPI_PORT, SPI_SS);
	#endif
}

uint8_t spi_fast_shift (uint8_t data)
// Clocks only one byte to target device and returns the received one
{
	#ifdef _DRIVE_SS
	cbit(SPI_PORT, SPI_SS);
	#endif
	
    SPDR = data;
    while((SPSR & (1<<SPIF))==0);
	
	#ifdef _DRIVE_SS
	sbit(SPI_PORT, SPI_SS);
	#endif
	
    return SPDR;
	
}

