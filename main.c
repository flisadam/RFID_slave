/*
 * RFID_system_slave.c
 *
 * Created: 2018-02-23 17:08:21
 * Author : Adam
 */ 

//fuse bits-external clock 4mHz
//#define F_CPU   4000000UL

#include <avr/io.h>
//#include <util/delay.h>
#include <avr/interrupt.h>
#include "SoftSPI.h"
#include "rfid.h"

#define ACK 0x7E
#define LONG_TIME 10000
#define SCK PORTA4
#define MISO PORTA5
#define MOSI PORTA6
#define CS PORTA7




extern volatile uint64_t RFID_data;
extern volatile uint8_t test_flag;
extern volatile uint8_t PW_read_flag;
static uint8_t header_finder;
uint8_t k;
int main(void)
{
	
	DDRA |=(1<<PORTA2);
	PORTA |= (1<<PORTA2);
	DDRB |= (1<<PORTB1);
	PORTB|=(1<<PORTB1);
	InitSoftSPI();
	InitHardSPI();
	rfid_start_read_tag();
	RFID_init();

	sei();
	RFID_decoded_flag=0;

	
    while (1) 
    {
	if (RFID_decoded_flag) {
    cli();

					header_finder=SPI_SlaveTransmit(9);
					header_finder=SPI_SlaveTransmit(RFID_id[0]);
					header_finder=SPI_SlaveTransmit(RFID_id[1]);
					header_finder=SPI_SlaveTransmit(RFID_id[2]);// to trzeba odkomentowac
					header_finder=SPI_SlaveTransmit(RFID_id[3]);
					header_finder=SPI_SlaveTransmit(RFID_id[4]);
	sei();
	RFID_decoded_flag=0;
	}
		

	}
	
	
		
	
    }








