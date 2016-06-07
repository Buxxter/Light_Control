#include "button_functions.h"
#include "../../light/light.h"
// 1 - свой
// 2 - компьютер
// 3 - эркер

struct {
	uint8_t further_start;
	uint8_t further_stop;
	
	uint8_t central_start;
	uint8_t central_stop;
	
	uint8_t comp_start;
	uint8_t comp_stop;
	
	uint8_t erker_start;
	uint8_t erker_stop;
	
	} intervals;


void button_functions_init(void)
{
	intervals.further_start	= 3;
	intervals.further_stop	= 3;
	
	intervals.central_start	= 6;
	intervals.central_stop	= 6;
	
	intervals.comp_start	= 8;
	intervals.comp_stop		= 9;
	
	intervals.erker_start	= 10;
	intervals.erker_stop	= 13;
}
	
bool buttons_override = false;

void send_button_state(button *btn, char *btn_name);
void turn(uint8_t self_start, uint8_t self_stop, bool all);

void turn(uint8_t self_start, uint8_t self_stop, bool all)
{
	if (all)
	{
		//light_turn_interval(0, 15, (light_cur_state.all == 0));
		light_turn_all(self_start, (light_cur_state.all == 0));
		return;
	}
		
	light_turn_interval(self_start, self_stop, (light_cur_state.all & (1<<self_start|1<<self_stop)) == 0);
	
}

void further_bed_switch(button *btn)
{
	send_button_state(btn, "btn_further");
		
	if (buttons_override)
	{
		bt_reset(btn);
		return;
	}
	
	
	if (btn->state.longs_count == 1)
	{
		turn(btn->state.shorts_count, 0, true);
		bt_reset(btn);
		return;
	}
	
	switch (btn->state.shorts_count)
	{
		case 1: turn(intervals.further_start,	intervals.further_stop,		btn->state.longs_count == 1); break;
		case 2: turn(intervals.comp_start,		intervals.comp_stop,		btn->state.longs_count == 1); break;
		case 3: turn(intervals.erker_start,		intervals.erker_stop,		btn->state.longs_count == 1); break;		
	}
	bt_reset(btn);
}

void central_bed_switch(button *btn)
{
	send_button_state(btn, "btn_central");
		
	if (buttons_override)
	{
		bt_reset(btn);
		return;
	}
	
	
	if (btn->state.longs_count == 1)
	{
		turn(btn->state.shorts_count, 0, true);
		bt_reset(btn);
		return;
	}
	
	switch (btn->state.shorts_count)
	{
		case 1: turn(intervals.central_start,	intervals.central_stop,		btn->state.longs_count == 1); break;
		case 2: turn(intervals.comp_start,		intervals.comp_stop,		btn->state.longs_count == 1); break;
		case 3: turn(intervals.erker_start,		intervals.erker_stop,		btn->state.longs_count == 1); break;
	}
	bt_reset(btn);
}

void comp_switch(button *btn)
{
	send_button_state(btn, "btn_comp");
		
	if (buttons_override)
	{
		bt_reset(btn);
		return;
	}
	
	
	if (btn->state.longs_count == 1)
	{
		turn(btn->state.shorts_count, 0, true);
		bt_reset(btn);
		return;
	}
	
	switch (btn->state.shorts_count)
	{
		case 1: turn(intervals.comp_start,		intervals.comp_stop,		btn->state.longs_count == 1); break;
		case 2: turn(intervals.erker_start,		intervals.erker_stop,		btn->state.longs_count == 1); break;
	}
	bt_reset(btn);
}

void erker_switch(button *btn)
{
	send_button_state(btn, "btn_erker");
		
	if (buttons_override)
	{
		bt_reset(btn);
		return;
	}
	
	
	if (btn->state.longs_count == 1)
	{
		turn(btn->state.shorts_count, 0, true);
		return;
	}
	
	switch (btn->state.shorts_count)
	{
		case 1: turn(intervals.erker_start,		intervals.erker_stop,		btn->state.longs_count == 1); break;
		case 2: turn(intervals.comp_start,		intervals.comp_stop,		btn->state.longs_count == 1); break;
	}
	bt_reset(btn);
}

void (*button_func[]) (button *btn) = {

	further_bed_switch,
	central_bed_switch,
	comp_switch,
	erker_switch

};

void send_button_state(button *btn, char *btn_name)
{
	usart_send_string(btn_name);
	usart_send_char('(');
	usart_send_char('0' + btn->state.longs_count);
	usart_send_char(',');
	usart_send_char('0' + btn->state.shorts_count);
	usart_send_string(")\r\n");
}