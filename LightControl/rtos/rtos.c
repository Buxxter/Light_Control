#include "rtos.h"

// Queues
volatile static TPTR	TaskQueue[MAX_TASKS_COUNT+1];			// Pointers Queue

// MaintTimers Queue
volatile static struct
{
	TPTR		GoToTask;					// Pointer to task
	uint16_t	TimeToStart;				// Time in TimerCycles to run task
	#ifdef _ARGS_SUPPORT
	uint8_t		args[MAX_ARGS_COUNT];
	bool		withargs;
	#endif
} MainTimer[MAX_TIMERS_COUNT+1];


// RTOS Init. Preparing the queues
inline void rtos_init(void)
{
	uint8_t	index;

	for(index = 0; index != MAX_TASKS_COUNT + 1; index++)	// Idle in all positions
	{
		TaskQueue[index] = Idle;
	}


	for(index = 0; index != MAX_TIMERS_COUNT + 1; index++) // Resetting all timers
	{
		MainTimer[index].GoToTask = Idle;
		MainTimer[index].TimeToStart = 0;
	}
	
	rtos_timer_setup();
}

//RTOS on system timer
inline void rtos_timer_setup (void)
{
	TCCR2 = 1<<WGM21|4<<CS20; 				// Freq = CK/64 - Setup mode and prescaler
	// Reset On Compare
	TCNT2 = 0;								// Counter start value
	OCR2  = LO(TimerDivider); 				// Compare Register value
	TIMSK |= 1<<OCIE2;						// RTOS Interrupt Enable - Start OS

}


// IDLE task
inline void  Idle(void)
{

}


// Add task to queue (ex. SetTask(Task1))
void AddTask(TPTR TS)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		uint8_t		index = 0;
		
		// Search for free place to new task
		while(TaskQueue[index] != Idle) 			
		{									
			index++;
		}
		
		// If found then place task into queue. Else return
		if (index != MAX_TASKS_COUNT + 1)
		{
			TaskQueue[index] = TS;
		}
	}
	
}

//void AddTimerTask(TPTR TS, uint16_t NewTime, bool update_if_exist)
//{
	//AddTimerHardTask(TS, NewTime * 2, update_if_exist);
//}

// Adds timer task. Example: SetTimerTask(Task1, Time_In_Intervals_of_TimerService
void AddTimerTask(TPTR TS, uint16_t NewTime_ms, bool update_if_exist)
{
	if (NewTime_ms == 0)
	{
		AddTask(TS);
		return;
	}
	
	uint8_t		index=0;
	
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		if (update_if_exist)
		{
			// Update TimeToStart if the task already in queue
			for(index = 0; index != MAX_TIMERS_COUNT + 1; ++index)
			{
				if(MainTimer[index].GoToTask == TS)
				{
					MainTimer[index].TimeToStart	= NewTime_ms - 1;
					return;
				}
			}
		}
		
	
		// Add timer if not in queue
		for(index = 0; index != MAX_TIMERS_COUNT + 1; ++index)	// Find empty timer
		{
			if (MainTimer[index].GoToTask == Idle)
			{
				MainTimer[index].GoToTask		= TS;
				MainTimer[index].TimeToStart	= NewTime_ms - 1;
				return;
			}
		
		}												
		
		//return; // No free space in queue
	}
		
}


/*=================================================================================
	TaskManager gets task in the head and runs it
*/

inline void TaskManager(void)
{
	uint8_t		index = 0;
	TPTR		GoToTask = Idle;
	
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		GoToTask = TaskQueue[0];				// Take first task in queue

		if (GoToTask != Idle)
		{
			// Remove current task from queue
			for(index = 0; index != MAX_TASKS_COUNT; index++)
			{
				TaskQueue[index] = TaskQueue[index + 1];
			}

			TaskQueue[MAX_TASKS_COUNT] = Idle;	// Idle in the tail
		}
	}
	
	(GoToTask)();								// Run task	
}


/*
Timers Service
If executing not from interrupt ATOMIC_BLOCK required
*/
inline void TimerService(void)
{
	uint8_t index;

	for(index=0; index != MAX_TIMERS_COUNT + 1; index++)
	{
		if(MainTimer[index].GoToTask == Idle) continue;

		if(MainTimer[index].TimeToStart != 0)	// To Do: Вычислить по тактам, что лучше !=1 или !=0.
		{
			MainTimer[index].TimeToStart--;
		}
		else
		{
			AddTask(MainTimer[index].GoToTask);
			MainTimer[index].GoToTask = Idle;
		}
	}
}
