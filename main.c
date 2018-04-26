/*
 * RFID_system_slave.c
 *
 * Created: 2018-02-23 17:08:21
 * Author : Adam
 */ 

//fuse bits-external clock 4mHz
//#define F_CPU   4000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "SoftSPI.h"
#include "rfid.h"

#define ACK 0x7E
#define LONG_TIME 10000
#define SCK PORTA4
#define MISO PORTA5
#define MOSI PORTA6
#define CS PORTA7




//uint8_t volatile RFID_ID [5];
//volatile uint16_t PulseWidth=5;
extern volatile uint8_t test_flag;

int main(void)
{
	
	DDRA |=(1<<PORTA2);
	PORTA |= (1<<PORTA2); // led testowy poza p³ytk¹
	DDRB |= (1<<PORTB1);
	PORTB|=(1<<PORTB1);
	InitSoftSPI();
	InitHardSPI();
	rfid_start_read_tag();
	RFID_init();
	sei();
    while (1) 
    {
		//sei();
/*	if (test_flag)
	{
		//cli();
		//header_align();
					if(!(PINA & (1<<PORTA7)))//sprawdz czy CS ma niskie stan
					{
						
						for (int i=0;i<8;i++)
						{
							int x =SPI_SlaveTransmit(RFID_data[i]);
						}
		
					}
					test_flag==0;
	}
	
	
	/*	
	
	if (test_flag)
		{
			decode_ok=header_align();
		
			//cli();
			//PORTB ^= (1<<PORTB1);
			RFID_decoded_flag=0;
			if(!(PINA & (1<<PORTA7)))//sprawdz czy CS ma niskie stan
			{
			
				for (i=0;i<8;i++)
				{
					test =SPI_SlaveTransmit(RFID_data[i]);
				}
			

			}
			test_flag=0;
			//sei();
		}
		
		*/
    }
}





