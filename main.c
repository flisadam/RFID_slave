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
extern volatile uint8_t licznik;
volatile uint8_t rise_scope=10; 
//volatile uint16_t PulseWidth=5;
extern volatile uint8_t test_flag;
void send64(uint64_t data);

int main(void)
{
	uint64_t volatile RFID_data1=0b1110111111111010101010101010100101010101010101010110101010110110;
	uint16_t volatile RFID16=	 0b1011111111101010;
	uint8_t test =2;
	uint8_t send = 8;
	uint16_t send16=640; //2 i 120
	uint16_t PulseWidth1=500;
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
	if (RFID_decoded_flag)
	{
		PORTB ^= (1<<PORTB1);
		RFID_decoded_flag=0;
	}


		if (!(PINA & (1<<PORTA7)))//sprawdz czy CS ma niskie stan
	{
		//test =SPI_SlaveTransmit(PulseWidth>>8 & 0x00FF);
		//test =SPI_SlaveTransmit(PulseWidth & 0x00FF);
		//	test =SPI_SlaveTransmit(ICR2>>8 & 0x00FF);
		//	test =SPI_SlaveTransmit(ICR2 & 0x00FF);
		//	test =SPI_SlaveTransmit(licznik);
	}

    }
}




void send64(uint64_t data)
{
	int nothing;
				uint64_t RFID_data5=data;
				uint32_t change1;
				uint32_t change2;
				change1=RFID_data5>>32 & 0x00000000FFFFFFFF;
				change2= RFID_data5 & 0x00000000FFFFFFFF;
				uint16_t changee1=change1>>16 & 0x0000FFFF;
				uint16_t changee2=change1 & 0x0000FFFF;
				uint16_t changee3=change2>>16 & 0x0000FFFF;
				uint16_t changee4=change2 & 0x0000FFFF;
				nothing= SPI_SlaveTransmit(changee1 >> 8 & 0x00FF);
				nothing= SPI_SlaveTransmit(changee1 & 0x00FF);
				nothing= SPI_SlaveTransmit(changee2 >> 8 & 0x00FF);
				nothing= SPI_SlaveTransmit(changee2 & 0x00FF);
				nothing= SPI_SlaveTransmit(changee3 >> 8 & 0x00FF);
				nothing= SPI_SlaveTransmit(changee3 & 0x00FF);
				nothing= SPI_SlaveTransmit(changee4 >> 8 & 0x00FF);
				nothing= SPI_SlaveTransmit(changee4 & 0x00FF);
}

