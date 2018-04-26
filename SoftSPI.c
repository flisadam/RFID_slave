/*
 * SoftSPI.c
 *
 * Created: 2018-04-08 00:12:34
 *  Author: Adam
 */ 

#include "SoftSPI.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>




void InitSoftSPI(void)
{
	DDRB &=~(1<<PORTB2); 
	DDRA &=~(1<<PORTA0);
	DDRA |= (1<<SMOSI)|(1<<SSCK);
	PORTA |= (1<<SMISO);
	
}

void SendSoftSPI(uint8_t byte)
{
	
	uint8_t cnt = 0x80;//0b10000000
	SSCK_0;
	while (cnt)//
	{
		if(byte & cnt) SMOSI_1;
		else SMOSI_0;
		SSCK_1;
		SSCK_0;
		cnt >>=1;
		
	}
	
}

void InitHardSPI (void)
{
		/* Set MISO output, all others input */
		DDRA |= (1<<MISO);
		/* Enable SPI */
		SPCR |= (1<<SPE); //enable SPI
		//SPCR |= (1<< SPIE); // causes SPI interrupt to be executed if SPIF is set
		//SPDR ='x';
}

uint8_t SPI_SlaveTransmit(uint8_t bajt)
{
	
	SPDR=bajt;

	while(!(SPSR & (1<<SPIF)));
	return SPDR;
}

uint8_t SPI_BytesTransmit(uint8_t *tab)
{
	for (int i=0; i<sizeof(tab);i++)
	{
		SPDR= *(tab+i);
			while(!(SPSR & (1<<SPIF)));
			return SPDR;
		
	}
}

//void Timer_init(void)
//{
	//TCCR0A |= (1<<WGM01); //tryb CTC
	//TCCR0B &=~ (1<<CS01);
	//TCCR0B |= (1<<CS02)|(1<<CS00);//1024 preskaler
	//OCR0B =38;//rej. porównania
	//TIMSK0 |= (1<<TOIE0); //zezwolenie na przerwania
	
//}

void softspi_write_bit(uint8_t x, uint8_t m)
{
	SSCK_1;
	if (x & m ) SMOSI_1;
	else SMOSI_0;
	SSCK_0;
	
}

void softspi_write_byte(uint8_t x)
{
	  softspi_write_bit(x, (1 << 7));
	  softspi_write_bit(x, (1 << 6));
	  softspi_write_bit(x, (1 << 5));
	  softspi_write_bit(x, (1 << 4));
	  softspi_write_bit(x, (1 << 3));
	  softspi_write_bit(x, (1 << 2));
	  softspi_write_bit(x, (1 << 1));
	  softspi_write_bit(x, (1 << 0));
}

void softspi_read_bit(uint8_t*x , uint8_t i)
{
	SSCK_1;
	SSCK_0;
	
	if(PINA & SMISO) *x |= 1<< i; // jesli stan wysoki na pinie 
	
	
}

uint8_t softspi_read_byte(void)
{
	uint8_t x=0;
 softspi_read_bit(&x, 7);
 softspi_read_bit(&x, 6);
 softspi_read_bit(&x, 5);
 softspi_read_bit(&x, 4);
 softspi_read_bit(&x, 3);
 softspi_read_bit(&x, 2);
 softspi_read_bit(&x, 1);
 softspi_read_bit(&x, 0);
 
 return x;
	
}

void rfid_start_read_tag (void)
{
	SSCK_0;
_delay_us(1000);
	SMOSI_0;
	_delay_us(1000);
	SSCK_1;
	_delay_us(1000);
	SMOSI_1;
	_delay_us(1000);
	

	//BIT 7
SSCK_0;
_delay_us(1000);
SMOSI_1;
SSCK_1;
_delay_us(1000);

	//BIT 6
SSCK_0;
_delay_us(1000);
SMOSI_1;
SSCK_1;
_delay_us(1000);

	//BIT 5
SSCK_0;
_delay_us(1000);
SMOSI_1;
SSCK_1;
_delay_us(1000);

SSCK_0;
SMOSI_0;
	
	//THIS HTRC110 NOW IMMEDIATELY SWITCHES TO TRANSPARENT MODE.  THE TRASNPONDER DATA IS DIRECTLY PRESENTED AT DOUT.
	//Delay from RX to DOUT is dependant on demodulator settings.
	
}

uint16_t convertFrom8To16(uint8_t dataFirst, uint8_t dataSecond) {
	uint16_t dataBoth = 0x0000;

	dataBoth = dataFirst;
	dataBoth = dataBoth << 8;
	dataBoth |= dataSecond;
	return dataBoth;
}


uint8_t *convertFrom16To8(uint16_t dataAll) {
	static uint8_t arrayData[2] = { 0x00, 0x00 };

	*(arrayData) = (dataAll >> 8) & 0x00FF;// bajt starszy
	arrayData[1] = dataAll & 0x00FF;//bajt mlodszy
	return arrayData;
}

