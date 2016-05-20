
#include <avr/io.h>
#include <inttypes.h>

#include "includes.h"

#include <util/delay.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>

#define STATUS_PORT PORTD
#define STATUS_DDR	DDRD
#define STATUS_PIN	PIND4

void hello_message(void);

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
	
	
	AddTimerTask(hello_message, 1000, true);
	
	
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
	usart_check_tx_buffer();
	
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
		
	uint8_t result = cmd_exec(command);
	status();
	
	usart_pgm_send_string(result ? pgm_ok : pgm_error);
	
}

void status(void)
{
	STATUS_PORT ^= (1<<STATUS_PIN);
}

void hello_message(void)
{
	usart_send_string("\r\nStarted\r\n");
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
		#ifdef _USART_ECHO_ENABLED
		usart_send_char('\n');
		#endif
		
		AddTask(execute_command);
	}
	
}

ISR(USART_UDRE_vect)
{
	usart_tx_interrupt();
}
