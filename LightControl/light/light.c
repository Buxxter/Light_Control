
#include "light.h"
#include "../libraries/fifo/fifo.h"

#include <inttypes.h>


uint8_t		light_cur_state[2];
#define		light_cur_state_l	(light_cur_state[0])
#define		light_cur_state_h	(light_cur_state[1])

FIFO(64)	light_lstate_queue;
FIFO(64)	light_hstate_queue;
#define		LIGHT_QUEUE_IS_EMPTY	(FIFO_IS_EMPTY(light_lstate_queue))


uint8_t		light_dimmer_value;

void light_init(void)
{
	light_cur_state_l = 0;
	light_cur_state_h = 0;
	light_update_state();
}

void light_update_state(void)
{
	
	spi_transmit_sync(light_cur_state, 2);
	
	//AddTimerTask(light_move, SWITCH_INTERVAL, false);	
	
}

void light_switch_to_next_state(void)
{
	if (LIGHT_QUEUE_IS_EMPTY)
	{
		return;
	}
	
	light_cur_state_l = FIFO_GET(light_lstate_queue);
	light_cur_state_h = FIFO_GET(light_hstate_queue);
	
	light_update_state();
	
	if (!LIGHT_QUEUE_IS_EMPTY)
	{
		AddTimerTask(light_switch_to_next_state, LIGHT_SWITCH_INTERVAL, true);
	}
	
}

void lamp_add_state_to_queue(uint8_t lamp_number, bool on)
{
	uint8_t val_L = LIGHT_QUEUE_IS_EMPTY ? light_cur_state_l : FIFO_PEEK_LAST(light_lstate_queue);
	uint8_t val_H = LIGHT_QUEUE_IS_EMPTY ? light_cur_state_h : FIFO_PEEK_LAST(light_hstate_queue);
	
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
			lamp_add_state_to_queue(start_bit + i, on);
		}
	} else {
		for (uint8_t i = 0 ; i <= start_bit - stop_bit; i++)
		{
			lamp_add_state_to_queue(start_bit - i, on);
		}
	}
	
	//light_switch_to_next_state();
	AddTimerTask(light_switch_to_next_state, LIGHT_SWITCH_INTERVAL, true);
	
}