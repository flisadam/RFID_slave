/*
 * rfid.c
 *
 * Created: 2018-04-12 09:42:36
 *  Author: Adam
 */ 
//#define  F_CPU 4000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
//#include <util/delay.h>
#include "rfid.h"
#define BIT_MASK 0b1111111110000000000000000000000000000000000000000000000000000000ULL
uint64_t bit_mask=0b1111111110000000000000000000000000000000000000000000000000000000;
uint8_t k=10;
uint16_t CRC16= 0x8005;
volatile uint64_t RFID_data;//zmienna globalna przechowuj�ca ostatnio odebrane 64 bity danych
volatile uint8_t RFID_decoded_flag;//flaga ustawiana w celu sygnalizacji odebrania ramki
volatile uint8_t PW_read_flag;
volatile uint8_t RFID_id[5];//tablica w kt�rej zapisywane s� zdekodowane dane
volatile uint8_t test_flag;
void RFID_init(void)
{
	DDRA &=~(1<<PORTA0);
	DDRB &=~(1<<PORTB2);
	PORTA |= (1<<PORTA0);
	PORTB |= (1<<PORTB2);/// PB2 i PA0 wej�cia z ppull upem
	TCCR2B |= (1<<CS21); //8 prescaller
	TCCR2B |=(1<<ICES2);//zbocze opadajace
	TCCR2B |= (1<<ICNC2);// noise canceler
	TIMSK2 |= (1<<ICIE2); //zezwolenie na przerwanie
	RFID_decoded_flag = 0;
	
	
	
}

	
	
uint8_t header_align(){
	uint8_t mbit;
	for(uint8_t i=0;i<64;i++){
		//PORTA ^= (1<<PORTA2);
		if((RFID_data&bit_mask)==bit_mask){
PORTA ^= (1<<PORTA2);
			return 1;
			//break;
		}
	//	PORTA ^= (1<<PORTA2);//
		if(RFID_data&MBIT_MASK)
		{
			mbit=1;

		}
		else mbit=0;//sprawdzamy najwy�szy bit
		RFID_data=RFID_data<<1;//przes�wamy ramk� o 1 bit w lewo
		if(mbit) RFID_data|=LBIT_MASK;//dopisujemy wcze�niejszy najwy�szy bit na koniec
	}
	if((RFID_data&bit_mask)==bit_mask){//sprawdzamy po ostatnim przesuni�ciu
	//PORTA ^= (1<<PORTA2);
		return 1;
	}//je�li nadal nie mamy headeru to ramka by�a b��dna
	return 0;
	}

	
uint8_t parity_cal(uint8_t value){
	switch(value){
		case 0b0000: return 0;
		case 0b0001: return 1;
		case 0b0010: return 1;
		case 0b0011: return 0;
		case 0b0100: return 1;
		case 0b0101: return 0;
		case 0b0110: return 0;
		case 0b0111: return 1;
		case 0b1000: return 1;
		case 0b1001: return 0;
		case 0b1010: return 0;
		case 0b1011: return 1;
		case 0b1100: return 0;
		case 0b1101: return 1;
		case 0b1110: return 1;
		case 0b1111: return 0;
	}
	return 2;
}



uint8_t h_parity(){
	
	//PORTA ^= (1<<PORTA2);
	uint8_t hbyte=0;
	for(uint8_t i=0;i<10;i++){
		uint8_t par_c,par_r;
		hbyte=(RFID_data&((uint64_t)D10_MASK)<<(i*5))>>(D10_SHIFT+(i*5));
		par_c=parity_cal(hbyte);
		par_r=(RFID_data&((uint64_t)P10_MASK)<<(i*5))>>(P10_SHIFT+(i*5));
		if(par_c!=par_r){
			
	
		}
		if(i%2==0){
		
			RFID_id[i/2]=hbyte;
			}else{
			RFID_id[i/2]|=hbyte<<4;
		}
	}
	
	return 1;//ok;//zwracamy czy parzysto�� si� zgadza
}


uint8_t v_parity(){//obliczamy bit parzysto�ci pionowej
	for(uint8_t i=0;i<4;i++){
		uint8_t vpar_c=0;
		uint8_t vpar_r;
		for(uint8_t j=0;j<5;j++){//obliczamy na podstawie po��czonych bajt�w
			if(RFID_id[j]&(0b10000000>>i))vpar_c^=0x01;
			if(RFID_id[j]&(0b00001000>>i))vpar_c^=0x01;
		}
		//�ci�gamy odczytany bit parzysto�ci
		vpar_r=(RFID_data&((uint64_t)CP4_MASK)<<(3-i))>>(CP4_SHIFT+(3-i));
		if(vpar_r!=vpar_c){//sprawdzamy - pierwszy b��d i nie ma sensu sprawdza� dalej
			return 0;
		}
	}
	return 1;
}











ISR(TIMER2_CAPT_vect)// zbocze opadajace to "1"
{
	//if (RFID_decoded_flag==0){//
	//PORTB ^= (1<<PORTB1);
	static uint16_t PulseWidth;//szeroko�� impulsu mi�dzy zboczami
	static uint8_t  EdgeCt;//licznik zbocz
	static uint8_t  BitCt;//licznik bit�w odebranych
	static uint8_t  BitVal;//warto�� aktualnie przetwarzanego bitu
	uint8_t  decode_ok;//zmienna tymczasowa pomagaj�ca obs�ugiwa� kontrol� poprawno�ci danych
	static uint64_t RFID_tmp;//tymczasowa zmienna do kt�rej zapisywane s� kolejno odbierane bity
	static uint8_t header_finder;
	static uint16_t LastICR;//ostatnia warto�� przechwycenia
	PulseWidth = ICR2- LastICR;//szeroko�� impulsu
	LastICR=ICR2;//zapisujemy dane tego zbocza
	TCCR2B ^= (1<<ICES2);//zmiana zbocza wyzwalaj�cego na przeciwne
		if(EdgeCt == 0){//je�li system dekodowania zosta� wyzerowany
			BitCt=0;//resetujemy wszystkie zmienne
			BitVal=1;
			RFID_tmp=0;
			header_finder=0;
		}

		if(PulseWidth < MIN_HALF_BIT || PulseWidth > MAX_BIT){//impuls za d�ugi lub za kr�tki - reset dekodowania



			EdgeCt=0;
			}else if(PulseWidth >= MIN_HALF_BIT && PulseWidth <= MAX_HALF_BIT){//impuls kr�tki (1/2 CLK)

			if(EdgeCt % 2  == 0){//je�li to parzyste zbocze
				RFID_tmp<<=1;//to zapisujemy bit
				RFID_tmp|=(uint64_t)BitVal;
				header_finder++;
				BitCt++;//i zwi�kszamy licznik odebranych bit�w
			}
			EdgeCt++;//zwi�kszamy licznik zbocz
			}else{//przeciwny wypadek - (PulseWidth > MAX_HALF_BIT && PulseWidth < MAX_BIT)
			//czyli d�ugi impuls (1 CLK)
			header_finder=0;
			BitVal^=0x01;//zmieniamy warto�� aktualnie przetwarzanego bitu
			RFID_tmp<<=1;//i zapisujemy bit
			RFID_tmp|=(uint64_t)BitVal;
			BitCt++;//i zwi�kszamy licznik odebranych bit�w
			EdgeCt+=2;//i zwi�kszamy licznik zbocz o 2 (dla porz�dku)
		}
		if(BitCt>64){

			EdgeCt=0;
			if (RFID_decoded_flag == 0) {
				RFID_data=RFID_tmp;

				decode_ok=header_align();//

				if(decode_ok)decode_ok=h_parity();
				if(decode_ok)decode_ok=v_parity();
				if(!decode_ok){
					RFID_data=~RFID_data;
					decode_ok=header_align();
					if(decode_ok)decode_ok=h_parity();
					if(decode_ok)decode_ok=v_parity();
				}
				RFID_decoded_flag=decode_ok;
				if (decode_ok==1) PORTA^=(1<<PORTA2);
				else PORTB ^= (1<<PORTB1);
			}
		}
		
			}

