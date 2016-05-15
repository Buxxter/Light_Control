
#include "light.h"

#include <inttypes.h>


uint16_t	light_current_state;
uint8_t		light_dimmer_value;

void light_move(void)
{
	if (light_current_state == 0)
	{
		light_current_state = 1;
	} else {
		light_current_state = light_current_state << 1;
	}
	
	spi_transmit_sync((uint8_t *) &light_current_state, 2);
	
	AddTimerTask(light_move, 100);	
	
}