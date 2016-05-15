
#include <avr/io.h>
#include <inttypes.h>

#include "includes.h"

#include <util/delay.h>
#include <avr/wdt.h>

int main(void)
{
	
	spi_init();
	spi_transmit_sync(0, 1);
	spi_transmit_sync(0, 1);
	
	rtos_init();
	
	sei();
	
	light_move();
	
	
    /* Replace with your application code */
    while (1) 
    {
		wdt_reset();
		TaskManager();
    }
	
	return 0;
}


ISR(RTOS_ISR)
{
	sei();
	TimerService();
}
