#ifndef _UART_TIO_H_
#define _UART_TIO_H_

/*
Text I/O module. Moves data through UART @ 9600BPS/8N1.

Written by YS from radiokot.ru and we.easyelectronics.ru

You may freely use and redistribute this code any way you want,
but only at your own risk. Author gives no warranty and takes no 
responsibility on anything that could happen while using this code.
*/

#include <avr/io.h>
#include <stdint.h>
#include <avr/pgmspace.h>


#include "../hardware.h"
#include "../macros.h"

#include "pgm_strings.h"

#include "../fifo/fifo.h"
#define _USART_FIFO_ENABLED
#define _USART_ECHO_ENABLED


//Maximum input text length
#define MAX_TXT_LEN			128

#if defined(_USART_FIFO_ENABLED)
FIFO(MAX_TXT_LEN) Tx_buffer;
FIFO(MAX_TXT_LEN) Rx_buffer;
#endif // _USART_FIFO_ENABLED

#define BAUD 115200UL
#define USE_2X 1




//Control characers
#define CHR_BS				0x08
#define CHR_ENTER			0x0D
#define CHR_LF				0x0A

//UART initialization
void usart_init(void);

unsigned char usart_rx_interrupt(void)	; // USART_RX_vect
void usart_tx_interrupt		(void)	; // USART_UDRE_vect
void usart_check_tx_buffer	(void)	;

void usart_send_string		(char *outtxt)			;
void usart_send_char		(char ch)				;
void usart_pgm_send_string	(const uint8_t* pgm_msg);


#define usart_send_bin(bin_value) { \
	char tmp_string_bin[16]; \
	itoa(bin_value, tmp_string_bin, 2); \
	usart_send_string(tmp_string_bin); \
	usart_send_string("\r\n"); \
}

/*

//Char I/O
uint8_t TIO_CharInput(void);
void TIO_CharOutput(uint8_t ch);
//Text I/O. Input supports backspace.
void TIO_TextOutput(uint8_t *outtxt);
void TIO_TextInput(uint8_t *intxt);
//Sends string from FLASH.

*/


#endif
