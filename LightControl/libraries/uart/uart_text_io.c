#include "uart_text_io.h"

/*Code module from AVR command shell project.
See appropriate header file for detais.*/

void TIO_Init(void)
{
	/* Set baud rate */
	UBRRH = (unsigned char)(bauddivider>>8);
	UBRRL = (unsigned char)(bauddivider);
	UCSRA = (1<<U2X);
	/* Enable receiver and transmitter // + Interrupt on receive byte */
	UCSRB = (1<<RXEN)|(1<<TXEN)|(1<<RXCIE);
	/* Set frame format: 8data, 2stop bit */
	UCSRC = (1<<USBS)|(3<<UCSZ0);

}
/*

uint8_t TIO_CharInput(void)
{
	while (!(UCSRA & (1<<RXC)));

	return UDR;
}

void TIO_CharOutput(uint8_t ch)
{
	// Wait for empty transmit buffer
	while ( !( UCSRA & (1<<UDRE)) )
	;
	// Put data into buffer, sends the data
	UDR = ch;
  
}

void TIO_TextOutput(uint8_t *outtxt)
{
  uint8_t i=0;

  while (outtxt[i] != 0)
  {
    TIO_CharOutput(outtxt[i]);
	i++;
  }
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

void TIO_PrintPgmMsg(const uint8_t* pgm_msg)
{
  uint16_t i=0;
  uint8_t ch;

  do
  {
    ch=pgm_read_byte(&(pgm_msg[i]));
	i++;

	if (ch)
	  TIO_CharOutput(ch);

  }while (ch!=0);
}
*/