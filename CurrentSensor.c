/*
 * CurrentSensor.c
 *
 * Created: 9/28/2013 2:12:45 PM
 * Author: Matthew Herbert 
 * Description: Configures ADC for the ATMega128. Reads from portf and returns value
 * More details on pages 21 - 22 in notebook
 */ 

//IT WORKS AS SHOWN BELOW, BUT IS NOW ONLY READING FOR ANY VALUE
#include <avr/io.h>

#define ByteValue(bit) (1<<(bit))  //converts the bit into a byte value
#define clearbit(reg,bit) reg &= ~(ByteValue(bit))  //clears the corresponding bit in register reg
#define setbit(reg,bit) reg |= (ByteValue(bit)) //sets the corresponding bit in register reg
#define fosc 1843200 //clock speed
#define BAUD 9600
#define myubrr fosc/16/BAUD-1

void adcinit(void);
void USART_Init(unsigned int ubrr);
void USARTsend(unsigned char data);
unsigned char USARTrecieve();
void USARTflush();
int main(void)
{
	
/*  start the adc code...works
	unsigned short result;  //temp variable, regular int isn't needed
    adcinit();				//initialize ADC
	DDRF = 0x00;			//configure PORTF (ADC) as input so analog signals can be measured
	DDRA = 0xFF;			//configure PORTA to output so led's can be lit for testing
	PORTF = 0x00;			//make sure internal pull up resistors are turned off
	while(1)
    {
		setbit(ADCSRA,ADSC);			//start conversion
        while(ADCSRA & 0b01000000);		//wait until the conversion is complete
		result = ((ADCL)|((ADCH)<<8));	//10 bit conversion for channel 0
		//result = (result/1024)*5;
		if(result>=200){					//if any result is read, light LED
			//PORTA = 0xFF;
			PORTA = (result*5/1024)+1;
		}
		else{							//otherwise shut off
			PORTA = 0x00;
		}
			
    }
	return result;


*/
	USART_Init(myubrr);
	DDRA = 0xFF;			//configure PORTA to output so led's can be lit for testing
	unsigned char test;
	while(1){				//tests usartrecieve to see if working
	test = USARTrecieve();
	PORTA = test;
	}
	return 0;
}
/*
adcinit -> initializes the analog to digital conversion

detailed procedure for adcinit can be found on page 21 in notebook
*/
void adcinit(void){
	setbit(ADCSRA,ADEN); //enables ADC
	clearbit(ADCSRA,ADFR); //free running conversion (for setbit) (clearbit is single)
	ADCSRA = ((ADCSRA & 0b11111000)|0b00000110); //selects div by 64 clock prescaler
	ADMUX = ((ADMUX &=0b00111111)|0b01000000);   //selects AVcc (~5 volts) as Vref
	clearbit(ADMUX,ADLAR); //selects right adjust of ADC result
	ADMUX &= 0b11100000;   //selects single ended conversion of PF0
}

void USART_Init(unsigned int ubrr){
	UBRR0H = (unsigned char)(ubrr>>8); //sets baud rate for usart
	UBRR0L = (unsigned char)(ubrr);	  //sets baud rate for usart
	UCSR0B = (1<<RXEN)|(1<<TXEN);	  //enables reciever and transmitter
	UCSR0C = (1<<USBS)|(3<<UCSZ0);	  //set frame format: 8 data, 2 stop bit
}

void USARTsend(unsigned char data){  //from radar.c
	
	while(!(UCSR0A & (1<<UDRE0))); //check to see if there is space in the buffer
	UDR0 = data;				   //if there is space, load data into register/send
	
}

unsigned char USARTrecieve(){		//from radar.c
	
	while(!(UCSR0A & (1<<RXC0)));  //checks to see if there is new data in recieve register
	return UDR0;				   //if there is new data, return it
}

void USARTflush(void){			  //from atmega128 datasheet
	unsigned char dummy;		  //buffer to be emptied when reciever is disabled
	while( UCSR0A & (1<<RXC0) ) dummy = UDR0;
}