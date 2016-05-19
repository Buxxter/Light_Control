
#include <avr/io.h>
#include <inttypes.h>

#include "includes.h"
#include "libraries/usart/pgm_strings.h"

#include <util/delay.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>

#define STATUS_PORT PORTD
#define STATUS_DDR	DDRD
#define STATUS_PIN	PIND4

void tmp(void);

void status(void);
void execute_command(void);

int main(void)
{
	
	STATUS_DDR |= (1<<STATUS_PIN);
	
	//clock_prescale_set()
	
	spi_init();
	spi_transmit_sync(0, 1);
	spi_transmit_sync(0, 1);
	light_init();
	usart_init();
	
	rtos_init();
	
	sei();
	
	light_turn_interval(11, 2, true);
	
	//_delay_ms(2000);
	
	light_turn_interval(2, 11, false);
	
	light_turn_interval(0, 15, true);
	
	light_turn_interval(0, 15, false);
	
	//_delay_ms(2000);
	
	//light_turn_interval(5, 5, true);
	
	
	
	AddTimerTask(tmp, 10000, true);
	
	
    /* Replace with your application code */
    while (1) 
    {
		wdt_reset();
		TaskManager();
		usart_check_tx_buffer();
    }
	
	return 0;
}

void execute_command(void)
{
	unsigned char command[FIFO_SIZE(Rx_buffer) + 1];
	uint8_t index = 0;
	while(!FIFO_IS_EMPTY(Rx_buffer))
	{
		command[index] = FIFO_GET(Rx_buffer);
		if (command[index] != CHR_LF && command[index] != CHR_ENTER)
		{
			index++;
		}
	}
	
	command[index] = '\0';
	
	wdt_reset();
	usart_send_string("\r\n");
	usart_send_string("|");
	usart_send_string(command);
	usart_send_string("|");
	usart_send_string("\r\n");
	
	wdt_reset();
	uint8_t result = cmd_exec(command);
	status();
	
	
	usart_pgm_send_string(result ? pgm_ok : pgm_error);
	
}

void status(void)
{
	STATUS_PORT ^= (1<<STATUS_PIN);
}

void tmp(void)
{
	usart_send_string("Another 1 second\r\n");
	//status();
	//AddTimerTask(tmp, 1000, true);
}

ISR(RTOS_ISR)
{
	sei();
	TimerService();
}

ISR(USART_RXC_vect)
{
	//unsigned char rx_byte = UDR;
	uint8_t rx_byte = usart_rx_interrupt();
	
	if (rx_byte == CHR_ENTER)
	{
		AddTask(execute_command);
	}
	
}

ISR(USART_UDRE_vect)
{
	usart_tx_interrupt();
}
