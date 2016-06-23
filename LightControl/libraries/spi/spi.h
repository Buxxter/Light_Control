
#ifndef _SPI_H_
#define _SPI_H_

#include <avr/io.h>
#include "../macros.h"
#include "../hardware.h"

extern void spi_init();
extern void spi_transfer_sync (uint8_t * data_send, uint8_t * data_receive, uint8_t len);
extern void spi_transmit_sync (uint8_t * dataout, uint8_t len);
void spi_transmit_sync_inverted (uint8_t * dataout, uint8_t len);
extern uint8_t spi_fast_shift (uint8_t data);


#endif /* _SPI_H_ */
