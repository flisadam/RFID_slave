/*
 * rfid.c
 *
 * Created: 2018-04-12 09:42:36
 *  Author: Adam
 */ 
//#define  F_CPU 4000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "rfid.h"

volatile uint64_t RFID_data;//zmienna globalna przechowuj¹ca ostatnio odebrane 64 bity danych
//volatile uint64_t RFID_data2;
volatile uint8_t RFID_decoded_flag;//flaga ustawiana w celu sygnalizacji odebrania ramki

volatile uint8_t RFID_id[5];//tablica w której zapisywane s¹ zdekodowane dane
volatile uint16_t PulseWidth;
volatile uint8_t licznik=0;
volatile uint8_t test_flag;
void RFID_init(void)
{
	DDRA &=~(1<<PORTA0);
	DDRB &=~(1<<PORTB2);
	PORTA |= (1<<PORTA0);
	PORTB |= (1<<PORTB2);/// PB2 i PA0 wejœcia z ppull upem
	TCCR2B |= (1<<CS21); //8 prescaller
	TCCR2B |=(1<<ICES2);//zbocze opadajace
	TCCR2B |= (1<<ICNC2);// noise canceler
	TIMSK2 |= (1<<ICIE2); //zezwolenie na przerwanie
	RFID_decoded_flag = 0;
	
	
	
}

/*
funkcja operujaca na zmiennej globalenj RFID_data
wyszukuje bity startu. Jeœli je odnajdzie to zwraca 1, a w zmiennej RFID_data
mamy wyrównan¹ do lewej strony ramkê
w przeciwnym wypadku fukcja zwraca 0
*/
//z RFID_DATA nie daje rady-nawet jak wpisze na sztywno
// jak do ifa wpisze sie na sztywno bity to dzia³a
uint8_t header_align(){
	// wchodzi z dobrze zapisanymi danymi i wywala b³¹d..
uint16_t RFID_data1=0b1110111111111010;//101010101010100101010101010101010110101010110110;
		 //			0b1100000000000000000000000000000000000000000000000000000000000000
	//PORTA ^= (1<<PORTA2);
	
	uint8_t mbit=1;//bit tymczasowy
	for(uint8_t i=0;i<64;i++){//maksymalnie mo¿emy obróciæ ca³¹ ramkê - 64 bity
		if((RFID_data1&SBIT_MASK)==SBIT_MASK){//sprawdzamy czy ramka jest OK
//PORTB ^= (1<<PORTB1);//nie spelnia warunku
PORTA ^= (1<<PORTA2);
			return 1;//jeœli tak to przerywamy dzia³anie
			//break;
		}//jeœli ramka nie jest ok
		if(RFID_data1&MBIT_MASK)
		{
			mbit=1;
			//PORTB ^= (1<<PORTB1);
		}
		else mbit=0;//sprawdzamy najwy¿szy bit
		RFID_data1=RFID_data1<<1;//przesówamy ramkê o 1 bit w lewo
		if(mbit)RFID_data1|=LBIT_MASK;//dopisujemy wczeœniejszy najwy¿szy bit na koniec
	}
	if((RFID_data1&SBIT_MASK)==SBIT_MASK){//sprawdzamy po ostatnim przesuniêciu
		//PORTB ^= (1<<PORTB1);// nie spelnia warunku
		return 1;
	}//jeœli nadal nie mamy headeru to ramka by³a b³êdna
	//PORTB ^= (1<<PORTB1);//- tu co 40ms jest wywalony blad
	return 0;
}

/*
szybkie obliczanie even parity dla czterech bitów
*/
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


/*
funkcja sprawdza parzystoœæ poziom¹ (P0-P9)
i przy okazji ³¹czy 4-ro bitowe fragmenty danych w pe³ne bajty
*/
uint8_t h_parity(){
	
	//PORTA ^= (1<<PORTA2);
	uint8_t ok=1,hbyte=0;
	for(uint8_t i=0;i<10;i++){
		uint8_t par_c,par_r;
		hbyte=(RFID_data&((uint64_t)D10_MASK)<<(i*5))>>(D10_SHIFT+(i*5));//œci¹gamy po³ówkê bajtu
		par_c=parity_cal(hbyte);//obliczamy parzystoœæ even
		par_r=(RFID_data&((uint64_t)P10_MASK)<<(i*5))>>(P10_SHIFT+(i*5));//œci¹gamy odczytan¹ parzystoœæ
		if(par_c!=par_r){//jeœli siê nie zgadza to mamy b³¹d
			ok=0;//parity_error
		}
		if(i%2==0){//przy okazji ³¹czmy po³ówki bajtów w bajty i zapisujemy zdekodowany numer tagu
			//licznik++;
			RFID_id[i/2]=hbyte;
			}else{
			RFID_id[i/2]|=hbyte<<4;
		}
	}
	
	return ok;//zwracamy czy parzystoœæ siê zgadza
}


uint8_t v_parity(){//obliczamy bit parzystoœci pionowej
	for(uint8_t i=0;i<4;i++){
		uint8_t vpar_c=0;
		uint8_t vpar_r;
		for(uint8_t j=0;j<5;j++){//obliczamy na podstawie po³¹czonych bajtów
			if(RFID_id[j]&(0b10000000>>i))vpar_c^=0x01;
			if(RFID_id[j]&(0b00001000>>i))vpar_c^=0x01;
		}
		//œci¹gamy odczytany bit parzystoœci
		vpar_r=(RFID_data&((uint64_t)CP4_MASK)<<(3-i))>>(CP4_SHIFT+(3-i));
		if(vpar_r!=vpar_c){//sprawdzamy - pierwszy b³¹d i nie ma sensu sprawdzaæ dalej
			return 0;
		}
	}
	return 1;
}





/// 540 us - 1/2 CLK 
// przerwanie co 250 us lub co 500 us


// przerwanie bez zliczania ca³ej ramki trwa 24 us
// wywoluje sie co 276 us-

ISR(TIMER2_CAPT_vect)
{
	//PORTB ^= (1<<PORTB1);
	//PORTA ^= (1<<PORTA2);
	static uint16_t LastICR;//ostatnia wartoœæ przechwycenia
	static uint8_t  EdgeCt;//licznik zbocz
	static uint8_t  BitCt;//licznik bitów odebranych
	static uint8_t  BitVal;//wartoœæ aktualnie przetwarzanego bitu
	uint8_t  decode_ok;//zmienna tymczasowa pomagaj¹ca obs³ugiwaæ kontrolê poprawnoœci danych
	static uint8_t RFID_tmp;//tymczasowa zmienna do której zapisywane s¹ kolejno odbierane bity

	PulseWidth = ICR2- LastICR;//szerokoœæ impulsu
	LastICR=ICR2;//zapisujemy dane tego zbocza

	TCCR2B ^= (1<<ICES2);//zmiana zbocza wyzwalaj¹cego na przeciwne
	//licznik++;
	if(EdgeCt == 0){//jeœli system dekodowania zosta³ wyzerowany
		BitCt=0;//resetujemy wszystkie zmienne
		BitVal=1;
		RFID_tmp=0;
		//PORTA ^= (1<<PORTA2);
	}

	if(PulseWidth < MIN_HALF_BIT || PulseWidth > MAX_BIT){//impuls za d³ugi lub za krótki - reset dekodowania
		EdgeCt=0;
		//PORTA ^= (1<<PORTA2);
		}else if(PulseWidth >= MIN_HALF_BIT && PulseWidth <= MAX_HALF_BIT){//impuls krótki (1/2 CLK)
	
			//PORTA ^= (1<<PORTA2);
		//licznik= SPI_SlaveTransmit(130);
		if(EdgeCt % 2  == 0){//jeœli to parzyste zbocze
			RFID_tmp<<=1;//to zapisujemy bit
			//if (BitVal==1) PORTA ^= (1<<PORTA2);// test iloœci jedynek
			//else PORTB ^= (1<<PORTB1);			// test iloœci zer
			RFID_tmp|=(uint64_t)BitVal;
			BitCt++;//i zwiêkszamy licznik odebranych bitów
			
		}
		EdgeCt++;//zwiêkszamy licznik zbocz
		}else{//przeciwny wypadek - (PulseWidth > MAX_HALF_BIT && PulseWidth < MAX_BIT)
				//PORTA ^= (1<<PORTA2);
		//czyli d³ugi impuls (1 CLK)
		//licznik= SPI_SlaveTransmit(131);
		BitVal^=0x01;//zmieniamy wartoœæ aktualnie przetwarzanego bitu
		RFID_tmp<<=1;//i zapisujemy bit
		//if (BitVal==1) PORTA ^= (1<<PORTA2);	// test iloœci jedynek
		//else PORTB ^=(1<<PORTB1);				// test iloœci zer
	
		RFID_tmp|=(uint64_t)BitVal;
		BitCt++;//i zwiêkszamy licznik odebranych bitów
		EdgeCt+=2;//i zwiêkszamy licznik zbocz o 2 (dla porz¹dku)
	}
	
	//PORTA |= (1<<PORTA2);
// wywoluje siê co 64 przerwanie- i blokuje przerwania na 4.5ms
	//jeœli odebraliœmy ca³¹ ramkê/// to trwa 4.5ms i wywo³ujê siê co 33,7 ms- 
	if(BitCt>64){
		//PORTB ^= (1<<PORTB1);
		//_delay_ms(1);
		EdgeCt=0;//resetujemy system do odbioru kolejnej
		//RFID_decoded_flag=0;
		if (RFID_decoded_flag == 0) {//i jeœli poprzednie dekodowanie zosta³o odebrane w programie
			RFID_data=RFID_tmp;//to zapisujemy ramkê w formacie niezdekodowanym
			//PORTB ^= (1<<PORTB1);
			decode_ok=header_align();//i dekodujemy j¹
			//licznik++;
			if(decode_ok)decode_ok=h_parity();
			if(decode_ok)decode_ok=v_parity();
			if(!decode_ok){//jeœli dekodowanie posz³o nie tak jak trzeba
				RFID_data=~RFID_data;//to negujemy ramkê (mo¿e zaczêliœmy dekodowanie od niew³aœciwego zbocza)
				decode_ok=header_align();//i ponawiamy próbê dekodowania
				if(decode_ok)decode_ok=h_parity();
				if(decode_ok)decode_ok=v_parity();
			//	PORTB |= (1<<PORTB1);
			}
			RFID_decoded_flag=decode_ok;//i przypisujemy fladze to czy zdekodowano poprawnie ramkê, czy nie
		}
	}
	//PORTA ^= (1<<PORTA2);
}









// ramki przychodz¹ce s¹ ok 
// 0 jak parzysta
//hexa zapis RFID taga
// ad3868003c = 10101101 00111000 01101000 00000000 00111100

///bitowy zapis RFID taga
//111111111
//0011/parity0/  1100/parity0/ 0000/parity0/ 0000/parity0/ 0110/parity0/ 1001/parity0/ 

// 14 zer - na pewno!

/////testowanie przerwania RFID

//////-przerwanie wywo³ywane prawid³owo OK
//////-prescaler 8x OK
//////-odbiór ca³ej ramki OK
//////-dekodowanie OK
/////-header_align - nie dziala- nie znajduje 9 jedynek
///// co 40ms wywala blad blednej ramkiw header align
///// blad musi wynikaæ ze z³ych PulseWidthów
///// zegar ok F_CPU ok
////// h_parity
////// v_parity