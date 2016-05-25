#ifndef _MACROS_H_
#define _MACROS_H_

#define MAX(a,b) 					((a)>(b)?(a):(b))
#define MIN(a,b) 					((a)<(b)?(a):(b))

//#define bit_is_clear(port, bit)		(!(port & (1<<bit)))
//#define bit_is_set(port, bit)		(port & (1<<bit))

#define sbit(port, bit)				(port |= (1<<bit))
#define cbit(port, bit)				(port &= ~(1<<bit))
#define ibit(port, bit)				(port ^= (1<<bit))

#define HI(x)						((x)>>8)
#define LO(x)						((x) & 0xFF)



#endif /* _MACROS_H_ */