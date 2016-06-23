#include "button.h"
#include "button_functions.h"
//#include "button_local.h"


#if defined(BUTTON_DEBUG)
#include <stdlib.h>
#endif


uint16_t bt_mode_time = 2000 / 20;	// Длительность последовательности анализа
								// Сделано переменной, а не константой
								// чтобы можно было менять на лету. Снижая
								// там, где не надо ждать длительных комбинаций
								// что сильно повышает отзывчивость интерфейса



#define bt_pressed(_btn)				(!((*(_btn->pin_port)) & (1<<_btn->pin)))

//PIND	_SFR_IO8(0x10)
//DDRD	_SFR_IO8(0x11)
//PORTD	_SFR_IO8(0x12)


/* =================== LOCALS =================== */



void bt_reset(button *btn);
void init_button(button *btn, volatile uint8_t *pin_port, uint8_t pin, uint8_t max_longs, uint8_t max_shorts);
void scan_button(button *btn);


#if defined(BUTTON_DEBUG)
void dbg_msg(button *btn, char *msg);
#endif
/* ------------------- LOCALS ------------------- */
 
static button buttons[BUTTONS_COUNT];
 
/* ================================================= */
/* =================== INIT BUTTONS HERE =========== */
/* ================================================= */

void bt_init(void)
{
	button_functions_init();
	
	#if defined(BUTTON_DEBUG)
	buttons[0].char_index = 'D';
	buttons[1].char_index = '2';
	buttons[2].char_index = '3';
	buttons[3].char_index = '4';
	#endif
	
	//DDRC &= ~((1<<PINC2) | (1<<PINC3) | (1<<PINC4));
	//PORTC |= ((1<<PINC2) | (1<<PINC3) | (1<<PINC4));
	
	init_button(&buttons[0], &PINA, PINA0, 1, 3);
	init_button(&buttons[1], &PINA, PINC1, 1, 3);
	init_button(&buttons[2], &PINA, PINC2, 1, 2);
	init_button(&buttons[3], &PINA, PINA3, 1, 2);
	
	#if defined(BUTTON_DEBUG)
	dbg_msg(&buttons[0], "init");
	dbg_msg(&buttons[1], "init");
	dbg_msg(&buttons[2], "init");
	dbg_msg(&buttons[3], "init");
	#endif
	
}

uint8_t bt_run(button *btn)
{
	#if defined(BUTTON_DEBUG)
	dbg_msg(btn, "running");
	#endif
	
	uint8_t i = 0;
	
	while(!(btn == &buttons[i]))
	{
		i++;
		if (i >= BUTTONS_COUNT)
		{
			return 0;
		}
	}
	
	btn->state.processing = 0;
	(button_func[i])(btn);
	
	return 1;
}

/* =================== SUB ROUTINE ================= */

void init_button(button *btn, volatile uint8_t *pin_port, uint8_t pin, uint8_t max_longs, uint8_t max_shorts)
{
	//btn->task					= task;
	btn->pin_port				= pin_port;
	btn->pin					= pin;
	btn->max_longs	= max_longs;
	btn->max_shorts = max_shorts;
	
	(*(pin_port + 1))	&= ~(1<<pin);	// PIN_PORT_DDR to input
	(*(pin_port + 2))	|= (1<<pin);	// PIN_PORT_PORT enable internal pull-up
	
	bt_reset(btn);
}

void bt_reset(button *btn)
{
	btn->state.all	= 0;
	btn->executed	= 0;
	btn->fm_time	= 0;
	btn->timer_value = 0;
	#if defined(BUTTON_DEBUG)
	dbg_msg(btn, "reseted");
	#endif
}

void bt_scan(void)
{
	
	// Need to be called every 20ms
	for (uint8_t i = 0; i < BUTTONS_COUNT; i++)
	{
		if (buttons[i].state.completed == 1)
		{
			if (buttons[i].executed != 1)
			{
				//AddTask(buttons[i].task);
				buttons[i].executed = 1;
				buttons[i].timer_value = bt_mode_time;
				if(!bt_run(&buttons[i]))
				{
					bt_reset(&buttons[i]);
				}
				// task can reset button immediately if needed or not
				// but it must set processing to 0
			} else if (buttons[i].timer_value > 0) // timeout for reactivate
			{
				buttons[i].timer_value--;
				
			} else // reactivate
			{
				bt_reset(&buttons[i]);
			}			
		} else // activated, need to be scan
		{
			if (buttons[i].state.processing)
			{
				if (buttons[i].timer_value > 0)
				{
					buttons[i].timer_value--;
					scan_button(&buttons[i]);
				} else
				{
					buttons[i].state.completed = 1;
				}
			} else
			{
				scan_button(&buttons[i]);
			}
		}
	}
	AddTimerTask(bt_scan, BT_SCAN_INTERVAL_MS, true);
}

void scan_button(button *btn)
{
	#define UP 			0		// Дефайны состояний автомата. 0 - по дефолту.
	#define DN 			1
	#define AFTER_LONG 	2
	
		
	switch(btn->state.fm_state)
	{
		case UP:
		{
			if (bt_pressed(btn))
			{
				btn->state.fm_state = DN;
				btn->fm_time = 0;
				
				if (!btn->state.processing)
				{
					btn->state.processing = 1;
					btn->timer_value = bt_mode_time;
				}
				#if defined(BUTTON_DEBUG)
				dbg_msg(btn, "ST_UP, bt_pressed");
				#endif
			}
			break;
		}
		case DN:
		{
			
			btn->timer_value++;				// в нажатом состоянии таймер не должен идти, 
											// иначе он выщелкает раньше, чем определим длинное нажатие
			
			if(bt_pressed(btn))				// Все еще нажато?
			{				
				if (btn->fm_time < 20)		// Нажато меньше чем 20*20мс?
				{// Да
					btn->fm_time++;			// Увеличиваем счетчик итераций
				} else
				{
					btn->state.fm_state = AFTER_LONG;	// Нет, уже больше! Да у нас длинное нажатие! Переходим в АЛ
				}
				
				#if defined(BUTTON_DEBUG)
				dbg_msg(btn, "DN, still_pressed");
				#endif
				
			} else {
								
				btn->state.fm_state = UP;	// Кнопка отпущена
				btn->fm_time = 0;			// Время замера в ноль
				btn->state.shorts_count++;	// Счетчик коротких нажатий
				
				if (btn->state.shorts_count >= btn->max_shorts)
				{
					btn->state.completed	= 1;
					btn->state.processing	= 0;
					btn->timer_value = 0;
				}
				#if defined(BUTTON_DEBUG)
				dbg_msg(btn, "DN, released");
				#endif
			}

			break;							// Выход
		}
		case AFTER_LONG:					// А тут мы если было длинное нажатие
		{
			if(bt_pressed(btn))				// Отпустили?
			{								// Нет, держим таймер
				btn->timer_value++;
			} else
			{
				btn->state.fm_state = UP;	// Да! Стадию в Up
				btn->fm_time = 0;			// Сбрасываем время замера нажатия
				
				btn->state.longs_count++;	// Засчитываем одно длинное нажатие
							
				if (btn->state.longs_count >= btn->max_longs)
				{
					btn->state.completed	= 1;
					btn->state.processing	= 0;
					btn->timer_value = 0;
				}
				#if defined(BUTTON_DEBUG)
				dbg_msg(btn, "AL, released");
				#endif
			}

			break;
		}
	}
}

#if defined(BUTTON_DEBUG)
void dbg_msg(button *btn, char *msg)
{
	usart_send_string("btn(");
	usart_send_char(btn->char_index);
	usart_send_string("): |");
	
	char tmp_string_bin[16];
	itoa(btn->state.all, tmp_string_bin, 2); 
	usart_send_string(tmp_string_bin);
	usart_send_string("| ");
	usart_send_string(msg);
	usart_send_string("\r\n");
}
#endif