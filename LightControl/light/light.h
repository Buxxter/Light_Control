/*
 * light.h
 *
 * Created: 15.05.2016 3:49:24
 *  Author: My
 */ 


#ifndef LIGHT_H_
#define LIGHT_H_

#include "../includes.h"

#define LIGHT_MAIN_LAMPS_COUNT				16
#define LIGHT_SWITCH_INTERVAL_ms			62

#define LIGHT_DIMMER_MAX_VALUE				100
#define LIGHT_DIMMER_SWITCH_INTERVAL_ms		2000 / LIGHT_DIMMER_MAX_VALUE	
#define LIGHT_AC_frequency_Hz				49
#define LIGHT_DIMMER_CONTROL_PORT			PORTA
#define LIGHT_DIMMER_CONTROL_DDR			DDRA
#define LIGHT_DIMMER_CONTROL_PIN			PINA0

#define _DIMMER_ENABLED						1

typedef union
{
	uint16_t all;
	uint8_t byte[2];
	struct
	{
		uint8_t byte_l;
		uint8_t byte_h;
	};
} light_state;

light_state light_cur_state;



void light_init(void);


void light_add_state_to_queue_by_lamp_number(uint8_t lamp_number, bool on);
void light_turn_interval(uint8_t start_bit, uint8_t stop_bit, bool on);
void light_turn_interval_hard(uint8_t start_bit, uint8_t stop_bit, bool on);
void light_get_current_state_uint8(uint8_t * output);
uint16_t light_get_last_state_uint16(void);
void light_set_state_uint16(uint16_t new_value);

void light_turn_all(uint8_t mode, bool on);

void light_dimmer_change_value(uint8_t new_value);
void light_dimmer_isr(void);

#ifdef DEBUG
void reset_zerocross_counter(void);
#endif

#endif /* LIGHT_H_ */