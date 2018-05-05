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




//uint8_t volatile RFID_ID [5];
extern volatile uint64_t RFID_data;
//extern volatile uint16_t PulseWidth;
extern volatile uint8_t test_flag;
extern volatile uint8_t PW_read_flag;
//static uint8_t  EdgeCt;//licznik zbocz
//static uint8_t  BitCt;//licznik bitów odebranych
//static uint8_t  BitVal;//wartoœæ aktualnie przetwarzanego bitu
static uint8_t header_finder;

int main(void)
{
	//uint64_t RFID_tmp=0;
	DDRA |=(1<<PORTA2);
	PORTA |= (1<<PORTA2); // led testowy poza p³ytk¹
	DDRB |= (1<<PORTB1);
	PORTB|=(1<<PORTB1);
	InitSoftSPI();
	InitHardSPI();
	rfid_start_read_tag();
	RFID_init();
	sei();
//	uint64_t RFID_data1=	0b0000000000000000000000000000000000100000000000000011111111100000;
//	uint64_t RFID_tmp=0;
//	uint8_t decode_ok=0;
	RFID_decoded_flag=0;
//cli();
    while (1) 
    {

	if (RFID_decoded_flag) {
    cli();
					header_finder=SPI_SlaveTransmit(9);
					header_finder=SPI_SlaveTransmit(RFID_id[0]);
					header_finder=SPI_SlaveTransmit(RFID_id[1]);
					header_finder=SPI_SlaveTransmit(RFID_id[2]);
					header_finder=SPI_SlaveTransmit(RFID_id[3]);
					header_finder=SPI_SlaveTransmit(RFID_id[4]);
					//RFID_id[0]=0;
	sei();
	RFID_decoded_flag=0;
	}
		
	
	
	//}
	}
	
	
	
	
		/*if (PW_read_flag==1 && RFID_decoded_flag==0)
		{
			//test();
			//PORTA &=~ (1<<PORTA2);
			if(EdgeCt == 0)
			{//jeœli system dekodowania zosta³ wyzerowany
				BitCt=0;//resetujemy wszystkie zmienne
				BitVal=1;
				//point =0;
				header_finder=0;
			}
					if(PulseWidth < MIN_HALF_BIT || PulseWidth > MAX_BIT)
					{//impuls za d³ugi lub za krótki - reset dekodowania
					EdgeCt=0;
						}else if(PulseWidth >= MIN_HALF_BIT && PulseWidth <= MAX_HALF_BIT)
						{//impuls krótki (1/2 CLK)
							//PORTA ^= (1<<PORTA2);
							if(EdgeCt % 2  == 0)
							{	RFID_tmp<<=1;//to zapisujemy bit
								RFID_tmp|=(uint64_t)BitVal;
								header_finder++;
									//	if (BitVal==1) PORTA |= (1<<PORTA2);// test iloœci jedynek
									//		else PORTA &=~ (1<<PORTA2);			// test iloœci zer
								BitCt++;//i zwiêkszamy licznik odebranych bitów
							}
						EdgeCt++;//zwiêkszamy licznik zbocz
						}
						else
						{//przeciwny wypadek - (PulseWidth > MAX_HALF_BIT && PulseWidth < MAX_BIT)
			//PORTA ^= (1<<PORTA2);
								header_finder=0;
								BitVal^=0x01;//zmieniamy wartoœæ aktualnie przetwarzanego bitu
								RFID_tmp<<=1;//i zapisujemy bit
								RFID_tmp|=(uint64_t)BitVal;
								//	if (BitVal==1) PORTA ^= (1<<PORTA2);// test iloœci jedynek
								//	else PORTA ^= (1<<PORTA2);			// test iloœci zer
								BitCt++;//i zwiêkszamy licznik odebranych bitów
								EdgeCt+=2;//i zwiêkszamy licznik zbocz o 2 (dla porz¹dku)
							}
							PW_read_flag=0;
					}
	
	if(BitCt>64){//jeœli odebraliœmy ca³¹ ramkê

		EdgeCt=0;//resetujemy system do obdioru kolejnej
		if (RFID_decoded_flag == 0) {//i jeœli poprzednie dekodowanie zosta³o odebrane w programie
			
			//RFID_tmp=0b0000000000000000000000000000000000100000000000000011111111100000;
			RFID_data=RFID_tmp;//to zapisujemy ramkê w formacie niezdekodowanym

			decode_ok=header_align(RFID_data);//i dekodujemy j¹
			
			if(decode_ok)
			{
					
				decode_ok=h_parity();
			}
			if(decode_ok)
			{
				PORTA ^=(1<<PORTA2);
				decode_ok=v_parity();
			}
			
			if(!decode_ok){//jeœli dekodowanie posz³o nie tak jak trzeba
				RFID_data=~RFID_data;//to negujemy ramkê (mo¿e zaczêliœmy dekodowanie od niew³aœciwego zbocza)
				decode_ok=header_align(RFID_data);//i ponawiamy próbê dekodowania
				//if(decode_ok)decode_ok=h_parity();
				//if(decode_ok)decode_ok=v_parity();
			}
			RFID_decoded_flag=decode_ok;//i przypisujemy fladze to czy zdekodowano poprawnie ramkê, czy nie
		}
	}
		}

	
		 */
    }
//}

							
							
			//czyli d³ugi impuls (1 CLK)
			//licznik= SPI_SlaveTransmit(131);
			//BitVal^=0x01;//zmieniamy wartoœæ aktualnie przetwarzanego bitu
			//RFID_tmp[point]<<=1;//i zapisujemy bit
			//if (BitVal==1) PORTA ^= (1<<PORTA2);	// test iloœci jedynek
			//else PORTB ^=(1<<PORTB1);				// test iloœci zer
			
			//RFID_tmp[point]|=BitVal;





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




void test()
{
	uint64_t RFID_data1= 0b0000111111111000000000111000000000000000000000000000000000000000;
						 
	uint64_t rbit=1;
	uint8_t ones=0;
	for (uint8_t j=1;j<64;j++)
	{
		//PORTA ^= (1<<PORTA2);
		if ((RFID_data1&rbit) == rbit)
		{
			//PORTB |=(1<<PORTB1);
			ones++;
			if (ones==9);// PORTB ^= (1<<PORTB1);
		}
		else {
		//PORTB ^=(1<<PORTB1);
		ones=0;
		}
		rbit=rbit <<j;
	}
}


