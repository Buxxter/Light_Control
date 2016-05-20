#include "uart_text_io.h"
#include <util/atomic.h>

void usart_init(void)
{
	/* Set baud rate */
	//UBRRL = LO(bauddivider);
	//UBRRH = HI(bauddivider) & (~(1<<URSEL));
	

	UCSRA = (1<<U2X);			// Double USART Transmission speed
	#include <util/setbaud.h>
	UBRRH = UBRRH_VALUE;
	UBRRL = UBRRL_VALUE;
	
	UCSRB = (	( 1<<RXCIE	)|	// RX Complete Interrupt
				( 0<<TXCIE	)|	// TX Complete Interrupt
				( 0<<UDRIE	)|	// Data Register Empty Interrupt
				( 1<<RXEN	)|	// Receiver
				( 1<<TXEN	)|	// Transmitter
				( 0<<UCSZ2	)|	// Character Size 2
				( 0<<RXB8	)|	// Receive Data Bit 8
				( 0<<TXB8	)	// Transmit Data Bit 8
			);
			
	UCSRC = (	( 1<<URSEL	)|	// Register Select
				( 0<<UMSEL	)|	// USART Mode Select (0 - Asynchronous Operation, 1 - Synchronous Operation)
				( 0<<UPM1	)|	// Parity Mode
				( 0<<UPM0	)|	// Parity Mode
				( 0<<USBS	)|	// Stop Bit Select ( 0 = 1bit, 1 = 2bit )
				( 1<<UCSZ1	)|	// Character Size 1
				( 1<<UCSZ0	)|	// Character Size 0
				( 0<<UCPOL	)	// Clock Polarity
			);
	/*
	| UCSZ2 | UCSZ1 | UCSZ0 | Character Size	|	UPM1 | UPM0 | Parity Mode
	|   0	|   0	|   0	|   5-bit			|	 0	 |   0	|   Disabled
	|   0	|   0	|   1	|   6-bit			|	 0	 |   1	|   Reserved
	|   0	|   1	|   0	|   7-bit			|	 1	 |   0	|   Enabled, Even Parity
	|   0	|   1	|   1	|   8-bit			|	 1	 |   1	|   Enabled, Odd Parity
	|   1	|   0	|   0	|   Reserved		|
	|   1	|   0	|   1	|   Reserved		|
	|   1	|   1	|   0	|   Reserved		|
	|   1	|   1	|   1	|   9-bit			|
	*/
	

}


unsigned char usart_rx_interrupt (void) // USART_RX_vect
{
	unsigned char rx_byte = UDR;
	#if defined(_USART_FIFO_ENABLED)
		#if defined(_USART_ECHO_ENABLED)
			// echo
			//FIFO_PUT(Tx_buffer, rx_byte);
			usart_send_char(rx_byte);
		#endif
		if (rx_byte == CHR_BS)
		{
			FIFO_REMOVE_LAST(Rx_buffer);
		} else {
			FIFO_PUT(Rx_buffer, rx_byte);
		}
		
	#endif // _USART_FIFO_ENABLED
	return rx_byte;
}

void usart_tx_interrupt(void) // USART_UDRE_vect
{
	#if defined(_USART_FIFO_ENABLED)
	if (FIFO_IS_EMPTY(Tx_buffer))
	{
		UCSRB &= ~(1<<UDRIE);
	} else {
		UDR = FIFO_GET(Tx_buffer);
	}
	#endif
	
}

void usart_check_tx_buffer(void)
{
	#if defined(_USART_FIFO_ENABLED)
		if (!FIFO_IS_EMPTY(Tx_buffer))
		{
			UCSRB |= (1<<UDRIE);
		}
	#endif		
}

void usart_send_string(char *outtxt)
{
	uint8_t i=0;

	while (outtxt[i] != 0)
	{
		usart_send_char(outtxt[i]);
		i++;
	}
	usart_check_tx_buffer();
}

void usart_send_char(char ch)
{
	#if defined(_USART_FIFO_ENABLED)
		if (FIFO_IS_FULL(Tx_buffer))
		{
			return;
		}
		FIFO_PUT(Tx_buffer, ch);
		UCSRB |= (1<<UDRIE);
	#else
		usart_send_char_hard(ch);
	#endif
}

void usart_send_char_hard(char ch)
{
	// Wait for empty transmit buffer
	while ( !( UCSRA & (1<<UDRE)) );
	// Put data into buffer, sends the data
	UDR = ch;
}

void usart_pgm_send_string(const uint8_t* pgm_msg)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		uint16_t	i=0;
		uint8_t	ch;

		do
		{
			ch=pgm_read_byte(&(pgm_msg[i]));
			i++;

			if (ch)
			{
				//usart_send_char(ch);
				//usart_check_tx_buffer();
				usart_send_char(ch);
			}
		
			//if (FIFO_IS_FULL(Tx_buffer))
			//{
				//usart_check_tx_buffer();
			//}
		

		} while (ch!=0);
	}
}


/*

uint8_t TIO_CharInput(void)
{
	while (!(UCSRA & (1<<RXC)));

	return UDR;
}

void TIO_TextInput(uint8_t *intxt)
{
	uint8_t ch,i=0;

	intxt[i]=0;

	do
	{
		ch=TIO_CharInput();
		
		if (i<MAX_TXT_LEN)
		{
			if ((i) || (ch!=CHR_BS))
			TIO_CharOutput(ch); //Echo

			switch (ch)
			{
				case CHR_BS:

					if (i>0)
					i--;

					intxt[i]=0;

					TIO_CharOutput(' ');
					TIO_CharOutput(CHR_BS);

					break;

				case CHR_ENTER:
					intxt[i]=0;
					break;

				default:
					intxt[i]=ch;
					i++;
					break;
			}
		}
		else
		{
			if (ch==CHR_ENTER)
			{
				intxt[i-1]=0;
			}
		}
	}while (ch!=CHR_ENTER);
}

*/
