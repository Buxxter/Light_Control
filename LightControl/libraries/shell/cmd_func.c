#include "cmd_func.h"

/*Code module from AVR command shell project.
See appropriate header file for detais.*/

//Command line functions and their data

const uint8_t help_text[] PROGMEM = {

  "AVR command shell v1.0\r\n\
  Currently supports three commands:\r\n\
  help - display this help;\r\n\
  listarg - lists its arguments;\r\n\
  led <color> <state> - control LEDs.\r\n\
  \t color: green/blue;\r\n\
  \t state: on/off/blink/noblink\r\n"
};

void print_help(uint8_t* p_arg[],uint8_t num_args)
{
	usart_send_string("cmdhelp\r\n");
	
	usart_pgm_send_string(help_text);
}

const uint8_t msg_args[] PROGMEM = {"Arguments passed:\r\n"};
const uint8_t msg_noargs[] PROGMEM = {"No arguments passed.\r\n"};

void list_args(uint8_t* p_arg[],uint8_t num_args)
{
    uint8_t i;

	if (num_args)
	{
      //usart_pgm_send_string(msg_args);
	  usart_send_string("Arguments passed:\r\n");

	  for (i=0; i<num_args; i++)
	  {
	    usart_send_string((char*)(p_arg[i]));
		usart_send_string("\r\n");
	  }
    }
	else
	  usart_pgm_send_string(msg_noargs);

}

const uint8_t ledcmd_on[] PROGMEM = {"on"};
const uint8_t ledcmd_off[] PROGMEM = {"off"};
const uint8_t ledcmd_blink[] PROGMEM = {"blink"};
const uint8_t ledcmd_noblink[] PROGMEM = {"noblink"};

const uint8_t ledsel_green[] PROGMEM = {"green"};
const uint8_t ledsel_blue[] PROGMEM = {"blue"};

const uint8_t ledmsg_ok[] PROGMEM = {"LED OK\r\n"};
const uint8_t ledmsg_err_noparam[] PROGMEM = {"Error - too few parameters.\r\n"};
const uint8_t ledmsg_err_unknown[] PROGMEM = {"Error - unknown parameter.\r\n"};
const uint8_t ledmsg_err_unsupp_led[] PROGMEM = {"Unsupported LED, using green as default.\r\n"};

volatile uint8_t ledtype;

void handle_led(uint8_t* p_arg[],uint8_t num_args)
{
	usart_send_string("handle_led\r\n");
	#ifdef hide
	if (num_args>1)
	{
		if (str_equal_pgm(p_arg[0],ledsel_green))
		ledtype=PB1;
		else
		if (str_equal_pgm(p_arg[0],ledsel_blue))
		ledtype=PB2;
		else
		{
			ledtype=PB1;
			usart_pgm_send_string(ledmsg_err_unsupp_led);
		}

		if (str_equal_pgm(p_arg[1],ledcmd_on))
		{
			
			usart_pgm_send_string(ledmsg_ok);

			return;
		}

		if (str_equal_pgm(p_arg[1],ledcmd_off))
		{
			
			usart_pgm_send_string(ledmsg_ok);

			return;
		}

		if (str_equal_pgm(p_arg[1],ledcmd_blink))
		{
			
			usart_pgm_send_string(ledmsg_ok);

			return;
		}

		if (str_equal_pgm(p_arg[1],ledcmd_noblink))
		{
			
			usart_pgm_send_string(ledmsg_ok);

			return;
		}

		usart_pgm_send_string(ledmsg_err_unknown);
	}
	else
	usart_pgm_send_string(ledmsg_err_noparam);
	
	#endif
}

//Function table

void (*sys_func[]) (uint8_t* p_arg[],uint8_t num_args) = {

    print_help,
    list_args,
	handle_led

};

//Command line alias table

const uint8_t funcname1[] PROGMEM = {"help"};
const uint8_t funcname2[] PROGMEM = {"listarg"};
const uint8_t funcname3[] PROGMEM = {"led"};

const uint8_t * const sys_func_names[] PROGMEM = {

    funcname1,
    funcname2,
	funcname3

};
