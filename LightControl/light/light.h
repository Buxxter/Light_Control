/*
 * light.h
 *
 * Created: 15.05.2016 3:49:24
 *  Author: My
 */ 


#ifndef LIGHT_H_
#define LIGHT_H_

#include "../includes.h"

#define LIGHT_SWITCH_INTERVAL 500UL


void light_init(void);
void light_update_state(void);
void light_switch_to_next_state(void);
void lamp_add_state_to_queue(uint8_t lamp_number, bool on);
void light_turn_interval(uint8_t start_bit, uint8_t stop_bit, bool on);

#endif /* LIGHT_H_ */