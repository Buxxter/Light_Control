#include "cmd_func.h"

/*Code module from AVR command shell project.
See appropriate header file for detais.*/

//Command line functions and their data

#define new_line (usart_pgm_send_string(pgm_newline))

const uint8_t help_text[] PROGMEM = {
"help - display this help;\r\n\
listarg - lists its arguments;\r\n"
};

const uint8_t light_help[] PROGMEM = {
"light <on/off> [start_bit] [end_bit]\r\n\
light <hex/bin>\r\n"
};

void print_help(uint8_t* p_arg[],uint8_t num_args)
{	
	usart_pgm_send_string(help_text);
	usart_pgm_send_string(light_help);
}

const uint8_t msg_args[] PROGMEM = {"Arguments passed:\r\n"};
const uint8_t msg_noargs[] PROGMEM = {"No arguments passed.\r\n"};

void list_args(uint8_t* p_arg[],uint8_t num_args)
{
    uint8_t i;

	if (num_args)
	{
      //usart_pgm_send_string(msg_args);
	  usart_send_string("Arguments passed:");
	  new_line;

	  for (i=0; i<num_args; i++)
	  {
	    usart_send_string((char*)(p_arg[i]));
		//usart_send_string("\r\n");
		new_line;
	  }
    }
	else
	  usart_pgm_send_string(msg_noargs);

}

const uint8_t cmd_on[] PROGMEM = {"on"};
const uint8_t cmd_off[] PROGMEM = {"off"};

const uint8_t ledmsg_err_noparam[] PROGMEM = {"Error - too few parameters.\r\n"};
const uint8_t msg_err_unknown[] PROGMEM = {"Error - unknown parameters.\r\n"};
	
const uint8_t msg_hex[] PROGMEM = {"hex"};
const uint8_t msg_bin[] PROGMEM = {"bin"};


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

void light(uint8_t * p_arg[], uint8_t num_args)
{
	if (num_args == 0)
	{
		usart_pgm_send_string(light_help);
	} else
	if (num_args == 1)
	{
		if (str_equal_pgm(p_arg[0], msg_hex))
		{
			uint8_t light_state[3];
			light_get_current_state(light_state);
			light_state[2] = '\0';
			usart_send_string((char*)light_state);
			new_line;
		} else
		if (str_equal_pgm(p_arg[0], msg_bin))
		{
			char light_string[17];
			for (uint8_t i = 0; i < 16; i++)
			{
				light_string[i] = '0' + ((light_cur_state.all & (1<<i)) != 0);
			}
			light_string[16] = '\0';
			usart_send_string(light_string);
			new_line;
		} else {
			usart_pgm_send_string(msg_err_unknown);
			usart_send_string((char*)p_arg[0]);
			new_line;
		}
	} else
	if (num_args < 4)
	{
		
		if (!(str_is_number(p_arg[1])))
		{
			usart_pgm_send_string(msg_err_unknown);
			usart_send_string((char*)p_arg[1]);
			new_line;
			return;
		}
		
		if (num_args == 3)
		{
			if (!(str_is_number(p_arg[2])))
			{
				usart_pgm_send_string(msg_err_unknown);
				usart_send_string((char*)p_arg[2]);
				new_line;
				return;
			}
		}
		
		uint8_t start_bit	= str_to_uint8(p_arg[1]);
		uint8_t end_bit		= (num_args == 3) ? str_to_uint8(p_arg[2]) : start_bit;
		bool	tmp_on		= str_equal_pgm(p_arg[0], cmd_on);
		light_turn_interval(start_bit, end_bit, tmp_on);		
	}
	
}

void dimmer(uint8_t * p_arg[], uint8_t num_args)
{
	if (num_args == 0)
	{
		usart_pgm_send_string(light_help);
	} else
	if (num_args == 1)
	{
		uint8_t new_val = str_to_uint8(p_arg[0]);
		light_dimmer_change_value(new_val);
	} else
	{
		usart_pgm_send_string(msg_err_unknown);
		usart_send_string((char*)p_arg[0]);
		new_line;
	}
}


void boot(uint8_t * p_arg[], uint8_t num_args)
{
	wdt_enable(WDTO_15MS);
	while(1);
	
}

void echo(uint8_t * p_arg[], uint8_t num_args)
{
	if (num_args == 0)
	{
		usart_pgm_send_string(echo_en ? cmd_on : cmd_off);
		new_line;
	} else
	if (num_args == 1)
	{
		echo_en = str_equal_pgm(p_arg[0], cmd_on);
	} else
	{
		usart_pgm_send_string(msg_err_unknown);
		usart_send_string((char*)p_arg[0]);
		new_line;
	}
	
}

//Function table

void (*sys_func[]) (uint8_t* p_arg[],uint8_t num_args) = {

    print_help,
    list_args,
	handle_led,
	light,
	dimmer,
	boot,
	echo

};

//Command line alias table

const uint8_t funcname1[] PROGMEM = {"help"};
const uint8_t funcname2[] PROGMEM = {"listarg"};
const uint8_t funcname3[] PROGMEM = {"led"};
const uint8_t funcname4[] PROGMEM = {"light"};
const uint8_t funcname5[] PROGMEM = {"dimmer"};
const uint8_t funcname6[] PROGMEM = {"boot"};
const uint8_t funcname7[] PROGMEM = {"echo"};

const uint8_t * const sys_func_names[] PROGMEM = {

    funcname1,
    funcname2,
	funcname3,
	funcname4,
	funcname5,
	funcname6,
	funcname7
};
