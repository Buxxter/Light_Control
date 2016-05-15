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

//#include "cmd_shell.h"

#define baudrate 115200L
#define bauddivider ((F_CPU + baudrate * 8) / (baudrate * 16) -1)

//Maximum input text length
#define MAX_TXT_LEN			64

//Control characers
#define CHR_BS				0x08
#define CHR_ENTER			0x0D
#define CHR_LF				0x0A

//UART initialization
void TIO_Init(void);


/*

//Char I/O
uint8_t TIO_CharInput(void);
void TIO_CharOutput(uint8_t ch);
//Text I/O. Input supports backspace.
void TIO_TextOutput(uint8_t *outtxt);
void TIO_TextInput(uint8_t *intxt);
//Sends string from FLASH.
void TIO_PrintPgmMsg(const uint8_t* pgm_msg);

*/

#endif
