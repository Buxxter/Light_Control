#include "uart_text_io.h"

/*Code module from AVR command shell project.
See appropriate header file for detais.*/

void TIO_Init(void)
{
  UCSR0B=(1<<RXEN0) | (1<<TXEN0);
  UBRR0=51; //9600BPS
}

uint8_t TIO_CharInput(void)
{
  while (!(UCSR0A & (1<<RXC0)));

  return UDR0;
}

void TIO_CharOutput(uint8_t ch)
{
  UDR0=ch;

  while (!(UCSR0A & (1<<UDRE0)));
}

void TIO_TextOutput(uint8_t *outtxt)
{
  uint8_t i=0;

  while (outtxt[i]!=0)
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

void TIO_PrintPgmMsg(uint8_t* pgm_msg)
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
