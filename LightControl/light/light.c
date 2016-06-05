
#include "light.h"
#include "../libraries/fifo/fifo.h"

#include <inttypes.h>
#include <stdlib.h>

//#define LIGHT_DEBUG



//uint8_t		light_cur_state[2];

//#define		light_cur_state_l	(light_cur_state[0])
//#define		light_cur_state_h	(light_cur_state[1])

FIFO(64)	light_lstate_queue;
FIFO(64)	light_hstate_queue;
#define		LIGHT_QUEUE_IS_EMPTY	(FIFO_IS_EMPTY(light_lstate_queue))

uint16_t zerocross_counter = 0;

uint8_t		light_dimmer_current_value;
uint8_t		light_dimmer_point_value;

#ifndef _LOCAL_FUNCTIONS

void light_dimmer_init(void);
void light_update_state(void);
void light_switch_to_next_state(void);
void light_dimmer_signal_off(void);


#endif


void light_init(void)
{
	//light_cur_state_l = 0;
	//light_cur_state_h = 0;
	light_cur_state.all = 0;
	
	FIFO_FLUSH(light_lstate_queue);
	FIFO_FLUSH(light_hstate_queue);
	
	light_dimmer_current_value	= 0;
	light_dimmer_point_value	= 0;
	light_update_state();
	
	
	#ifdef _DIMMER_ENABLED
	light_dimmer_init();
	#endif // _DIMMER_ENABLED
	
}

void light_dimmer_init(void)
{
	// Control port as output
	LIGHT_DIMMER_CONTROL_DDR |= (1<<LIGHT_DIMMER_CONTROL_PIN);
	
	
	// INT1 Enable
	DDRD	&= ~(1<<PIND3); // PD3(INT1) as input
	PORTD	|= (0<<PIND3);	// PD3 Disable internal pull-up
	
	MCUCR |= 
		(0<<ISC11)|(0<<ISC10);
	
	/* 	
	| ISC11 | ISC10 | Description
	|   0	|   0	| The low level of INT1 generates an interrupt request.
	|   0	|   1	| Any logical change on INT1 generates an interrupt request.
	|   1	|   0	| The falling edge of INT1 generates an interrupt request.
	|   1	|   1	| The rising edge of INT1 generates an interrupt request.
	*/
	GICR |= (1<<INT1);	//	INT1: External Interrupt Request 1 Enable
}

void light_update_state(void)
{
	
	spi_transmit_sync(light_cur_state.byte, 2);
	
	//AddTimerTask(light_move, SWITCH_INTERVAL, false);	
	
}

void light_switch_to_next_state(void)
{
	if (LIGHT_QUEUE_IS_EMPTY)
	{
		return;
	}
	
	light_cur_state.byte_l = FIFO_GET(light_lstate_queue);
	light_cur_state.byte_h = FIFO_GET(light_hstate_queue);
	
	light_update_state();
	
	if (!LIGHT_QUEUE_IS_EMPTY)
	{
		AddTimerTask(light_switch_to_next_state, LIGHT_SWITCH_INTERVAL_ms, true);
	}
	
}

void light_add_state_to_queue(uint8_t lamp_number, bool on)
{
	uint8_t val_L = LIGHT_QUEUE_IS_EMPTY ? light_cur_state.byte_l : FIFO_PEEK_LAST(light_lstate_queue);
	uint8_t val_H = LIGHT_QUEUE_IS_EMPTY ? light_cur_state.byte_h : FIFO_PEEK_LAST(light_hstate_queue);
	
	if (on)
	{
		if (lamp_number < 8)
		{
			sbit(val_L, lamp_number);
		} else if (lamp_number < 16)
		{
			sbit(val_H, (lamp_number - 8));
		}
	} else {
		if (lamp_number < 8)
		{
			cbit(val_L, lamp_number);
		} else if (lamp_number < 16)
		{
			cbit(val_H, (lamp_number - 8));
		}
	}
	
	FIFO_PUT(light_lstate_queue, val_L);
	FIFO_PUT(light_hstate_queue, val_H);
		
}

void light_turn_interval(uint8_t start_bit, uint8_t stop_bit, bool on)
{
	if (start_bit <= stop_bit)
	{
		for (uint8_t i = 0 ; i <= stop_bit - start_bit; i++)
		{
			light_add_state_to_queue(start_bit + i, on);
		}
	} else {
		for (uint8_t i = 0 ; i <= start_bit - stop_bit; i++)
		{
			light_add_state_to_queue(start_bit - i, on);
		}
	}
	
	//light_switch_to_next_state();
	AddTimerTask(light_switch_to_next_state, LIGHT_SWITCH_INTERVAL_ms, true);
	
}

void light_get_current_state(uint8_t * output)
{
	//spi_transfer_sync(light_cur_state.byte, output, 2);
	//spi_transmit_sync(output, 2);
	output = light_cur_state.byte;
	
}

void light_update_dimmer_state(void)
{
	if (light_dimmer_current_value == light_dimmer_point_value)
	{
		return;
	} else
	if (light_dimmer_current_value < light_dimmer_point_value)
	{
		light_dimmer_current_value++;
	} else
	{
		light_dimmer_current_value--;
	}
	#ifdef notDEBUG
	char tmp_string[4];
	itoa(light_dimmer_current_value, tmp_string, 10);
	usart_send_string(tmp_string);
	usart_pgm_send_string(pgm_newline);
	#endif // DEBUG
	AddTimerTask(light_update_dimmer_state, LIGHT_DIMMER_SWITCH_INTERVAL_ms, true);
}

void light_dimmer_change_value(uint8_t new_value)
{
	if (new_value > 100) 
	{ 
		new_value = 100;
	}
	
	light_dimmer_point_value = new_value;
	light_update_dimmer_state();
}

void light_dimmer_signal_blink(void)
{
	sbit(LIGHT_DIMMER_CONTROL_PORT, LIGHT_DIMMER_CONTROL_PIN);
	AddTimerTask(light_dimmer_signal_off, 1, true);
}

void light_dimmer_signal_off(void)
{
	cbit(LIGHT_DIMMER_CONTROL_PORT, LIGHT_DIMMER_CONTROL_PIN);
}

void light_dimmer_isr(void)
{
	
	/* 
	50 full waves per S
	100 half-waves per 1000mS
	1 half-wave per 10mS
	1000 / (LIGHT_AC_frequency_Hz * 2)
	*/
	AddTimerTask(light_dimmer_signal_blink, 
				(LIGHT_DIMMER_MAX_VALUE - light_dimmer_current_value) / (1000 / (LIGHT_AC_frequency_Hz * 2)),
				false);
	zerocross_counter++;
	//AddTimerTask(light_dimmer_signal_blink,
	//(LIGHT_DIMMER_MAX_VALUE - light_dimmer_current_value - 1) / (1000 / (LIGHT_AC_frequency_Hz * 2) + 10),
	//false);
}

#ifdef LIGHT_DEBUG
void reset_zerocross_counter(void)
{
	char tmp_string[6];
	itoa(zerocross_counter, tmp_string, 10);
	usart_send_string(tmp_string);
	usart_send_string("\r\n");
	zerocross_counter = 0;
	AddTimerTask(reset_zerocross_counter, 10000, true);
}
#endif

/*
	10%
	
	(LIGHT_DIMMER_MAX_VALUE - light_dimmer_current_value) / (1000 / (LIGHT_AC_frequency_Hz * 2))
	
	(100 - 10) / (1000 / ( 50 * 2 ))
	(100 - 10) / (1000 / 100)
	(100 - 10) / (10)
	(90) / (10)
	
	(100 - 0) / 10 ))
	
*/