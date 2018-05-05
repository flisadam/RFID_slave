/*
 * SoftSPI.h
 *
 * Created: 2018-04-08 00:07:13
 *  Author: Adam
 */ 

//#define  F_CPU 4000000UL
#ifndef SOFTSPI_H_
#define SOFTSPI_H_

#include <avr/io.h>
//#include <util/delay.h>
#include <avr/interrupt.h>
#define SMISO PORTA0
#define SMOSI PORTA1
#define SSCK  PORTA3
#define SSCK_0 PORTA &= ~(1<<SSCK)
#define SSCK_1 PORTA |= (1<<SSCK)
#define SMOSI_0 PORTA &= ~(1<<SMOSI)
#define SMOSI_1 PORTA |= (1<<SMOSI)

#define SCK PORTA4
#define MISO PORTA5
#define MOSI PORTA6
#define CS PORTA7


#define READ_TAG			0b11100000
#define GET_SAMPLING_TIME	0b00000010
#define GET_CONFIG_PAGE		0b00000100
#define READ_PHASE			0b00001000
#define WRITE_TAG			0b11000000
//#define SET_CONFIG_PAGE		0b01
//#define SET_SAMPLING_TIME	0b0

void InitSoftSPI (void);
void SendSoftSPI (uint8_t byte);
void InitHardSPI (void);
uint8_t SPI_SlaveTransmit(uint8_t bajt);
//void Timer_init(void);
void softspi_write_bit(uint8_t x, uint8_t m);
void softspi_write_byte(uint8_t x);
void softspi_read_bit(uint8_t*x , uint8_t i);
uint8_t softspi_read_byte(void);
void rfid_start_read_tag (void);
uint8_t *convertFrom16To8(uint16_t dataAll);
uint16_t convertFrom8To16(uint8_t dataFirst, uint8_t dataSecond);
uint8_t SPI_BytesTransmit(uint8_t *tab);






#endif /* SOFTSPI_H_ */