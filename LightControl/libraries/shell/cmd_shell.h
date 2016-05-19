#ifndef _CMD_SHELL_H_
#define _CMD_SHELL_H_

/*
User interface module.

Written by YS from radiokot.ru and we.easyelectronics.ru

You may freely use and redistribute this code any way you want,
but only at your own risk. Author gives no warranty and takes no 
responsibility on anything that could happen while using this code.
*/

//These settings affect usage of SRAM.

//Maximum user input length in characters.
#define SHELL_INPUT_LEN		20
//Maximum number of arguments that can be passed with a command.
#define SHELL_ARGS_MAX		5

//Main user interface routine.
//Place it in your main() instead of while (1).
void cmd_process_loop(void);

#endif
