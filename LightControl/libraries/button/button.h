#ifndef BUTTON_H
#define BUTTON_H

#include "../../includes.h"
#include "../../rtos/rtos.h"

#define BUTTON_DEBUG 0

extern bool buttons_override;
extern uint16_t bt_mode_time;

#define BUTTONS_COUNT 4
#define BT_SCAN_INTERVAL_MS	20
#define BT_OVERRIDE_RESET_INTERVAL_MS 10000

typedef struct {
	#if (BUTTON_DEBUG)
	char		char_index;
	#endif
	union {
		uint8_t all;
		struct {
			uint8_t fm_state:2;		// 0,1
			uint8_t shorts_count:2;	// 2,3
			uint8_t longs_count:2;	// 4,5
			uint8_t completed:1;	// 6
			uint8_t processing:1;	// 7
		};
	}	state;	//		[long_h][long_L][short_count_h][short_count_l][competed][fm_state_h][fm_state_l]
	
	uint8_t		fm_time;
	uint16_t	timer_value;
	
	struct {
		uint8_t max_shorts:2;
		uint8_t max_longs:2;
		uint8_t executed:1;
	};
	
	volatile uint8_t *pin_port;
	uint8_t pin;
	//TPTR	task;
} button;


void bt_init(void);
void bt_scan(void);
void bt_reset(button *btn);
void bt_override_reset(void);

uint8_t bt_run(button *btn);


#endif
