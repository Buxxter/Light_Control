#ifndef _CMD_FUNC_H_
#define _CMD_FUNC_H_

/*
This module contains user-defined command handling functions
and command/function tables. Customize source to your needs.

Written by YS from radiokot.ru and we.easyelectronics.ru

You may freely use and redistribute this code any way you want,
but only at your own risk. Author gives no warranty and takes no 
responsibility on anything that could happen while using this code.
*/

#include <stdint.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

#include "cmd_util.h"
//#include "../usart/uart_text_io.h"
#include "../../light/light.h"

//Total functions number
#define FUNC_NUM        7

//Function and command tables
extern void (*sys_func[FUNC_NUM])(uint8_t* p_arg[],uint8_t num_args);
extern const uint8_t* const sys_func_names[];

#endif
