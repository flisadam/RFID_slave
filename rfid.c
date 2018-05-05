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
volatile uint64_t RFID_data;//zmienna globalna przechowuj�ca ostatnio odebrane 64 bity danych
//volatile uint64_t RFID_data2;
///static uint8_t  EdgeCt;//licznik zbocz
// static uint8_t  BitCt;//licznik bit�w odebranych
// static uint8_t  BitVal;//warto�� aktualnie przetwarzanego bitu
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

/*
funkcja operujaca na zmiennej globalenj RFID_data
wyszukuje bity startu. Je�li je odnajdzie to zwraca 1, a w zmiennej RFID_data
mamy wyr�wnan� do lewej strony ramk�
w przeciwnym wypadku fukcja zwraca 0
*/
//z RFID_DATA nie daje rady-nawet jak wpisze na sztywno
// jak do ifa wpisze sie na sztywno bity to dzia�a
/*uint8_t header_align(){
PORTB ^= (1<<PORTB1);
	// wchodzi z dobrze zapisanymi danymi i wywala b��d..
static uint8_t left_ones[8];
static uint8_t right_ones[8];
uint8_t rbit=0b00000001;//bit tymczasowy
uint8_t lbit=0b10000000;
//
//RFID_data[0]=0b00001111;
//RFID_data[1]=0b11110000;
//RFID_data[2]=0b00001111; wektory testowe
//RFID_data[3]=0b11110000;
//RFID_data[4]=0b00011111;
//RFID_data[5]=0b11110000;
//RFID_data[6]=0b00001111;
//RFID_data[7]=0b11110000;
	for (int i=0; i<8;i++)
	{
		lbit=	0b10000000;
		rbit =	0b00000001;
		right_ones[i]=0;
		left_ones[i]=0;
		
		while ((RFID_data[i]&rbit)==rbit)
		{
			rbit= rbit<<1;
			right_ones[i]++;
		}
		while ((RFID_data[i]&lbit)==lbit)
		{
			lbit= lbit>>1;
			left_ones[i]++;
		}
	}
for (int j=0;j<8;j++)
{
	if (j==7)
	{
			if (right_ones[j]+left_ones[0]==6)
			{
				//PORTA ^= (1<<PORTA2);
				return 1;
			}
	}
	else
	{
		
			if (right_ones[j]+left_ones[j+1]==6)
			{
				//PORTA ^= (1<<PORTA2);
				return 1;
			}
			
	}
}
return 0;
}// dziala-sprawdza 9 jedynek kolo siebie i je 
	
*/
	
	
uint8_t header_align(){
	uint8_t mbit;
	for(uint8_t i=0;i<64;i++){
		//PORTA ^= (1<<PORTA2);
		//maksymalnie mo�emy obr�ci� ca�� ramk� - 64 bity
		if((RFID_data&bit_mask)==bit_mask){
PORTA ^= (1<<PORTA2);
			return 1;//je�li tak to przerywamy dzia�anie
			//break;
		}
	//	PORTA ^= (1<<PORTA2);//je�li ramka nie jest ok
		if(RFID_data&MBIT_MASK)
		{
			mbit=1;
			//PORTA ^= (1<<PORTA2);
		//	PORTB ^= (1<<PORTB1);
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

	


/*
szybkie obliczanie even parity dla czterech bit�w
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
funkcja sprawdza parzysto�� poziom� (P0-P9)
i przy okazji ��czy 4-ro bitowe fragmenty danych w pe�ne bajty
*/
uint8_t h_parity(){
	
	//PORTA ^= (1<<PORTA2);
	uint8_t ok=1,hbyte=0;
	for(uint8_t i=0;i<10;i++){
		uint8_t par_c,par_r;
		hbyte=(RFID_data&((uint64_t)D10_MASK)<<(i*5))>>(D10_SHIFT+(i*5));//�ci�gamy po��wk� bajtu
		par_c=parity_cal(hbyte);//obliczamy parzysto�� even
		par_r=(RFID_data&((uint64_t)P10_MASK)<<(i*5))>>(P10_SHIFT+(i*5));//�ci�gamy odczytan� parzysto��
		if(par_c!=par_r){//je�li si� nie zgadza to mamy b��d
			ok=0;//parity_error
	
		}
		if(i%2==0){//przy okazji ��czmy po��wki bajt�w w bajty i zapisujemy zdekodowany numer tagu
			//licznik++;
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





/// 540 us - 1/2 CLK 
// przerwanie co 250 us lub co 500 us


// przerwanie bez zliczania ca�ej ramki trwa 24 us
// wywoluje sie co 276 us-

















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
		if(BitCt>64){//je�li odebrali�my ca�� ramk�

			EdgeCt=0;//resetujemy system do obdioru kolejnej
			if (RFID_decoded_flag == 0) {//i je�li poprzednie dekodowanie zosta�o odebrane w programie
				RFID_data=RFID_tmp;//to zapisujemy ramk� w formacie niezdekodowanym

				decode_ok=header_align();//i dekodujemy j�

				if(decode_ok)decode_ok=h_parity();
				if(decode_ok)decode_ok=v_parity();
				if(!decode_ok){//je�li dekodowanie posz�o nie tak jak trzeba
					//RFID_data=~RFID_data;//to negujemy ramk� (mo�e zacz�li�my dekodowanie od niew�a�ciwego zbocza)
					//decode_ok=header_align();//i ponawiamy pr�b� dekodowania
					//if(decode_ok)decode_ok=h_parity();
					//if(decode_ok)decode_ok=v_parity();
				}
				RFID_decoded_flag=decode_ok;//i przypisujemy fladze to czy zdekodowano poprawnie ramk�, czy nie
				if (decode_ok==1) PORTA^=(1<<PORTA2);
				else PORTB ^= (1<<PORTB1);
			}
		}
		//	}
			}







/*ISR(TIMER2_CAPT_vect)
{
	PORTB ^= (1<<PORTB1);
	
	//PORTB ^= (1<<PORTB1);
	//PORTA ^= (1<<PORTA2);
	static uint16_t LastICR;//ostatnia warto�� przechwycenia
	static uint8_t  EdgeCt;//licznik zbocz
	static uint8_t  BitCt;//licznik bit�w odebranych
	static uint8_t  BitVal;//warto�� aktualnie przetwarzanego bitu
	uint8_t  decode_ok;//zmienna tymczasowa pomagaj�ca obs�ugiwa� kontrol� poprawno�ci danych
	static uint8_t RFID_tmp[8];//tymczasowa zmienna do kt�rej zapisywane s� kolejno odbierane bity
	static uint8_t point;
	PulseWidth = ICR2- LastICR;//szeroko�� impulsu
	LastICR=ICR2;//zapisujemy dane tego zbocza
	static uint8_t  header_finder;
	if (test_flag==0)
{
	TCCR2B ^= (1<<ICES2);//zmiana zbocza wyzwalaj�cego na przeciwne
	//licznik++;
	if(EdgeCt == 0){//je�li system dekodowania zosta� wyzerowany
		BitCt=0;//resetujemy wszystkie zmienne
		BitVal=1;
		point =0;
		header_finder=0;
		for (int i=0;i<8;i++)
		{
			*(RFID_tmp + i)=0;// zerujemy cala tablice
		}
		//PORTA ^= (1<<PORTA2);
	}

	if(PulseWidth < MIN_HALF_BIT || PulseWidth > MAX_BIT){//impuls za d�ugi lub za kr�tki - reset dekodowania
		EdgeCt=0;
		//PORTA ^= (1<<PORTA2);
		}else if(PulseWidth >= MIN_HALF_BIT && PulseWidth <= MAX_HALF_BIT){//impuls kr�tki (1/2 CLK)
			
			//PORTA ^= (1<<PORTA2);
		//licznik= SPI_SlaveTransmit(130);
		if(EdgeCt % 2  == 0){//je�li to parzyste zbocze
			//RFID_tmp[point]<<=1;//to zapisujemy bit
			header_finder++;
			//if (BitVal==1) PORTA ^= (1<<PORTA2);// test ilo�ci jedynek
			//else PORTB ^= (1<<PORTB1);			// test ilo�ci zer
			//RFID_tmp[point]|=BitVal;
			//BitCt++;//i zwi�kszamy licznik odebranych bit�w
			
		}
		EdgeCt++;//zwi�kszamy licznik zbocz
		}else{//przeciwny wypadek - (PulseWidth > MAX_HALF_BIT && PulseWidth < MAX_BIT)
				//PORTA ^= (1<<PORTA2);
				header_finder=0;
		//czyli d�ugi impuls (1 CLK)
		//licznik= SPI_SlaveTransmit(131);
		//BitVal^=0x01;//zmieniamy warto�� aktualnie przetwarzanego bitu
		//RFID_tmp[point]<<=1;//i zapisujemy bit
		//if (BitVal==1) PORTA ^= (1<<PORTA2);	// test ilo�ci jedynek
		//else PORTB ^=(1<<PORTB1);				// test ilo�ci zer
	
		RFID_tmp[point]|=BitVal;
		BitCt++;//i zwi�kszamy licznik odebranych bit�w
		EdgeCt+=2;//i zwi�kszamy licznik zbocz o 2 (dla porz�dku)
	}
	if (BitCt % 8) point++;
	if (point> 7) point =0;
	//PORTA |= (1<<PORTA2);
// wywoluje si� co 64 przerwanie- i blokuje przerwania na 4.5ms
	//je�li odebrali�my ca�� ramk�/// to trwa 4.5ms i wywo�uj� si� co 33,7 ms- 
		}
		if(header_finder==8) PORTA ^= (1<<PORTA2);
	if(BitCt>64){
		
		for (int i=0; i<8; i++)
		{
			*(RFID_data+i)=*(RFID_tmp+i);
		}
		PORTA ^= (1<<PORTA2);
		if (*(RFID_tmp+0)>10) 
		PORTB ^= (1<<PORTB1);
		test_flag=1;
		//_delay_ms(1);
		EdgeCt=0;//resetujemy system do odbioru kolejnej
		//RFID_decoded_flag=0;
		if (RFID_decoded_flag == 0) {//i je�li poprzednie dekodowanie zosta�o odebrane w programie
				for (int i=0; i<8; i++)
				{
					*(RFID_data+i)=*(RFID_tmp+i);
				}
			//PORTB ^= (1<<PORTB1);
			decode_ok=header_align();//i dekodujemy j�
			//licznik++;
			if(decode_ok)decode_ok=h_parity();
			if(decode_ok)decode_ok=v_parity();
			if(!decode_ok){//je�li dekodowanie posz�o nie tak jak trzeba
				//RFID_data=~RFID_data;//to negujemy ramk� (mo�e zacz�li�my dekodowanie od niew�a�ciwego zbocza)
				decode_ok=header_align();//i ponawiamy pr�b� dekodowania
				if(decode_ok)decode_ok=h_parity();
				if(decode_ok)decode_ok=v_parity();
			//	PORTB |= (1<<PORTB1);
			}
		//	RFID_decoded_flag=decode_ok;//i przypisujemy fladze to czy zdekodowano poprawnie ramk�, czy nie
		//}
	}
	//PORTA ^= (1<<PORTA2);
	}
}

*/






// ramki przychodz�ce s� ok 
// 0 jak parzysta
//hexa zapis RFID taga
// ad3868003c = 10101101 00111000 01101000 00000000 00111100

///bitowy zapis RFID taga
//111111111
//0011/parity0/  1100/parity0/ 0000/parity0/ 0000/parity0/ 0110/parity0/ 1001/parity0/ 

// 14 zer - na pewno!

/////testowanie przerwania RFID

//////-przerwanie wywo�ywane prawid�owo OK
//////-prescaler 8x OK
//////-odbi�r ca�ej ramki OK
//////-dekodowanie OK
/////-header_align - nie dziala- nie znajduje 9 jedynek
///// co 40ms wywala blad blednej ramkiw header align
///// blad musi wynika� ze z�ych PulseWidth�w
///// zegar ok F_CPU ok
////// h_parity
////// v_parity