#ifndef RTOS_H_
#define RTOS_H_

#include <stdint.h>
#include <util/atomic.h>

#define MAX_TASKS_COUNT		5
#define MAX_TIMERS_COUNT	5 // 4 bytes per timer

typedef void (*TPTR)(void);

extern void rtos_init(void);
extern void Idle(void);

extern void AddTask(TPTR TS);
extern void AddTimerTask(TPTR TS, uint8_t NewTime);

extern void TaskManager(void);
extern void TimerService(void);

////RTOS Errors Пока не используются.
//#define TaskSetOk			 'A'
//#define TaskQueueOverflow	 'B'
//#define TimerUpdated		 'C'
//#define TimerSetOk			 'D'
//#define TimerOverflow		 'E'





#endif /* RTOS_H_ */