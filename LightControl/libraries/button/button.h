#ifndef BUTTON_H
#define BUTTON_H

//#include <HAL.h>
//#include <EERTOS.h>
//#include <avrlibtypes.h>
//#include <avrlibdefs.h>


#include "../../includes.h"
#include "../../rtos/rtos.h"

#define BUTTON_DEBUG


//#define BTN_PIN 1
//#define BTN 1

//#define Open	(BTN_PIN & 1<<BTN)
//#define Close	(!Open)


extern uint16_t bt_mode_time;

#define BT_SCAN_INTERVAL_MS	2000

#define bt_pressed(_btn)				(!(PIND & (1<<_btn->pin)))

typedef void (*TPTR)(void);

typedef struct {
	#ifdef BUTTON_DEBUG
	char		char_index;
	#endif
	union {
		uint8_t all;
		struct {
			uint8_t fm_state:2;
			uint8_t shorts_count:2;
			uint8_t longs_count:2;
			
			uint8_t processing:1;
			uint8_t completed:1;
			
		};
	}	state;	//		[long_h][long_L][short_count_h][short_count_l][competed][fm_state_h][fm_state_l]
	
	uint8_t		fm_time;
	uint16_t	timer_value;
	
	struct {
		uint8_t max_shorts:2;
		uint8_t max_longs:2;
	}	max_counters;
	//uint8_t *port;
	uint8_t pin;
	TPTR	task;
} button;

void bt_init(void);
void bt_scan(void);
void bt_ok(void);
void button1_task(void);
void reset_button(button *btn);
void init_button(button *btn, TPTR task, uint8_t pin, uint8_t max_longs, uint8_t max_shorts);
void scan_button(button *btn);

#endif
