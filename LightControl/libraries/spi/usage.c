// Allocate buffer array
uint8_t data_buf[1];
// Initialize SPI
spi_init();
// Send 0xFF to spi slave and read 1 byte back to the same array
data_buf[0] = 0xFF;
spi_transfer_sync(data_buf,data_buf,1);
// Show received byte on LEDs
PORTC = ~data_buf[0];