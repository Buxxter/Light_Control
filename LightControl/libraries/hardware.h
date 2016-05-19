#ifndef HARDWARE_H_
#define HARDWARE_H_

#define F_CPU 8000000UL

#include "spi/spi.h"
#include "usart/uart_text_io.h"

//#ifdef SPI_H

#define SPI_PORT    PORTB
#define SPI_DDR     DDRB
#define SPI_MISO    DDB6
#define SPI_MOSI    DDB5
#define SPI_SS      DDB4
#define SPI_SCK     DDB7

#define _DRIVE_SS
	
//#endif

#define USART_BAUDRATE 115200L

//System Timer Config
#define Prescaler	  		64
#define	TimerDivider  		(F_CPU/Prescaler/1000)		// 1 mS


#endif /* HARDWARE_H_ */