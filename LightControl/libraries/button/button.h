#ifndef BUTTON_H
#define BUTTON_H

#include "../../includes.h"
#include "../../rtos/rtos.h"

#define BUTTON_DEBUG


extern uint16_t bt_mode_time;

#define BT_SCAN_INTERVAL_MS	20

#define bt_pressed(_btn)				(!((*(_btn->pin_port)) & (1<<_btn->pin)))



void bt_init(void);
void bt_scan(void);

void button1_task(void);

#endif
