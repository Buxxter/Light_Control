#include "button.h"
//#include "button_local.h"


#define BUTTON_DEBUG

#ifdef BUTTON_DEBUG
#include <stdlib.h>
#endif


uint16_t bt_mode_time = 2000 / 20;	// Длительность последовательности анализа
								// Сделано переменной, а не константой
								// чтобы можно было менять на лету. Снижая
								// там, где не надо ждать длительных комбинаций
								// что сильно повышает отзывчивость интерфейса


#define BUTTONS_COUNT 1

//PIND	_SFR_IO8(0x10)
//DDRD	_SFR_IO8(0x11)
//PORTD	_SFR_IO8(0x12)


/* =================== LOCALS =================== */

typedef struct {
	#ifdef BUTTON_DEBUG
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
	}	max_counters;
	volatile uint8_t *pin_port;
	uint8_t pin;
	TPTR	task;
} button;

static button buttons[BUTTONS_COUNT];

void reset_button(button *btn);
void init_button(button *btn, TPTR task, volatile uint8_t *pin_port, uint8_t pin, uint8_t max_longs, uint8_t max_shorts);
void scan_button(button *btn);


#ifdef BUTTON_DEBUG
void dbg_msg(button *btn, char *msg);
#endif
/* ------------------- LOCALS ------------------- */



void bt_init(void)
{
	
	init_button(&buttons[0], button1_task, &PIND, PIND7, 3, 3);
}

void button1_task(void)
{
	usart_send_string("btn1\r\n");
}

void init_button(button *btn, TPTR task, volatile uint8_t *pin_port, uint8_t pin, uint8_t max_longs, uint8_t max_shorts)
{
	btn->task					= task;
	btn->pin_port				= pin_port;
	btn->pin					= pin;
	btn->max_counters.max_longs	= max_longs;
	btn->max_counters.max_shorts = max_shorts;
	
	(*(pin_port + 1))	&= ~(1<<pin);	// PIN_PORT_DDR to input
	(*(pin_port + 2))	|= (1<<pin);	// PIN_PORT_PORT enable internal pull-up
	
	
	#ifdef BUTTON_DEBUG
	btn->char_index = 'A';
	dbg_msg(btn, "init");
	#endif
	
	reset_button(btn);
}

void reset_button(button *btn)
{
	btn->state.all	= 0;
	btn->fm_time		= 0;
	btn->timer_value = 0;
	#ifdef BUTTON_DEBUG
	dbg_msg(btn, "reseted");
	#endif
}

void bt_scan(void)
{
	
	// Need to be called every 20ms
	for (uint8_t i = 0; i < BUTTONS_COUNT; i++)
	{
		if (buttons[i].state.completed == 1)
		//|| ((bt_get_processing(buttons[i]) & buttons[i].timer_value) == 0)
		{
			
			//#ifdef BUTTON_DEBUG
			//usart_send_string("btn1 task add\r\n");
			//#endif
			
			AddTask(buttons[i].task);
			reset_button(&buttons[i]);
		} else if (buttons[i].state.processing)
		{
			buttons[i].timer_value--;
		}
		scan_button(&buttons[i]);
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
			//if (bt_pressed(btn))
			if (!(PIND & (1<<btn->pin)))
			{
				btn->state.fm_state = DN;
				btn->fm_time = 0;
				
				if (!btn->state.processing)
				{
					btn->state.processing = 1;
					btn->timer_value = bt_mode_time;
				}
				#ifdef BUTTON_DEBUG
				dbg_msg(btn, "ST_UP, bt_pressed");
				#endif
			}
			break;
		}
		case DN:
		{			
			if(bt_pressed(btn))				// Все еще нажато?
			{				
				if (btn->fm_time < 20)		// Нажато меньше чем 20*20мс?
				{// Да
					btn->fm_time++;			// Увеличиваем счетчик итераций
				} else
				{
					btn->state.fm_state = AFTER_LONG;	// Нет, уже больше! Да у нас длинное нажатие! Переходим в АЛ
				}
				
				#ifdef BUTTON_DEBUG
				dbg_msg(btn, "DN, still_pressed");
				#endif
				
			} else {
								
				btn->state.fm_state = UP;	// Кнопка отпущена
				btn->fm_time = 0;			// Время замера в ноль
				btn->state.shorts_count++;	// Счетчик коротких нажатий
				
				if (btn->state.shorts_count >= btn->max_counters.max_shorts)
				{
					btn->state.completed = 1;
					btn->timer_value = 0;
				}
				#ifdef BUTTON_DEBUG
				dbg_msg(btn, "DN, released");
				#endif
			}

			break;							// Выход
		}
		case AFTER_LONG:					// А тут мы если было длинное нажатие
		{			
			if(!bt_pressed(btn))			// Отпустили?
			{
				btn->state.fm_state = UP;	// Да! Стадию в Up
				btn->fm_time = 0;			// Сбрасываем время замера нажатия
				
				btn->state.longs_count++;	// Засчитываем одно длинное нажатие
							
				if (btn->state.longs_count >= btn->max_counters.max_longs)
				{
					btn->state.completed = 1;
					btn->timer_value = 0;
				}
				#ifdef BUTTON_DEBUG
				dbg_msg(btn, "AL, released");
				#endif
			}

			break;
		}
	}
}

//void bt_set_state(button _btn, uint8_t new_state)
//{
	//_btn.state &= (~BT_ST_FM_STATE_BITS);
	//_btn.state |= (new_state<<BT_ST_FM_STATE_L);
//}

//uint8_t bt_get_state(button _btn)
//{
	//return ((_btn.state & BT_ST_FM_STATE_BITS)>>BT_ST_FM_STATE_L);
//}



#ifdef BUTTON_DEBUG
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

//void bt_ok(void)					// Ловим дешифровку событий тут
//{
//switch(bt_cnt_s)					// Смотрим сколько нажатий коротких
	//{
	//case 1: bt1 = 1; break;			// Такой флажок и ставим
	//case 2: bt2 = 1; break;
	//case 3: bt3 = 1; break;
	//case 4: bt4 = 1; break;
	//case 5: bt5 = 1; break;
	//default: break;
	//}
//
//switch(bt_cnt_l)					// Смотрим сколько нажатий длинных
	//{
	//case 1: bt_l = 1; break;		// Такой флажок и ставим
	//case 2: bt_l2 = 1; break;	
	//default: break;
	//}
//
//bt_cnt = 0;							// Сбрасываем счетчики
//bt_cnt_s = 0;
//bt_cnt_l = 0;
//
//}
