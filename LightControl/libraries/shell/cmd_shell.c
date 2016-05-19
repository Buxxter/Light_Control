#include "cmd_shell.h"
#include "../uart/uart_text_io.h"
#include "cmd_interp.h"

/*Code module from AVR command shell project.
See appropriate header file for detais.*/

//System messages
const uint8_t msg_cmd_err[] PROGMEM = {"Input error - unknown command.\r\n"};
const uint8_t msg_con[] PROGMEM = {"cmd: "};
const uint8_t msg_start[] PROGMEM = {"AVR command shell v1.0 by YS\r\n\r\n"};
const uint8_t msg_newline[] PROGMEM = {"\r\n"};

uint8_t cmdline[SHELL_INPUT_LEN];

void cmd_process_loop(void)
{
  TIO_Init();
  usart_pgm_send_string(msg_start);

  while (1)
  {
    usart_pgm_send_string(msg_con);
	TIO_TextInput(cmdline);

	usart_pgm_send_string(msg_newline);

	if (!(cmd_exec(cmdline)))
	  usart_pgm_send_string(msg_cmd_err);

	usart_pgm_send_string(msg_newline);
  }
}
