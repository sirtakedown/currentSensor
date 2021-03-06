/*
* CurrentSensor.c
*
* Created: 9/28/2013 2:12:45 PM
* Author: Matthew Herbert
* Description: Configures ADC for the ATMega128. Reads from portf and returns value
* More details on pages 21 - 22 in notebook
*
* PORTF -> ADC STUFF (F0 and F1 specifically)
* PORTC(0) -> LED switch
* PORT whatever -> Current, voltage switches
*/

//IT WORKS AS SHOWN BELOW, BUT IS NOW ONLY READING FOR ANY VALUE
#include <avr/io.h>
#include <avr/interrupt.h>
#include "CurrentSensor.h"


#define F_CPU 1600000UL //16 MHz
#define __DELAY_BACKWARD_COMPATIBLE__ 

#include <util/delay.h>

#define ByteValue(bit) (1<<(bit)) //converts the bit into a byte value
#define clearbit(reg,bit) reg &= ~(ByteValue(bit)) //clears the corresponding bit in register reg
#define setbit(reg,bit) reg |= (ByteValue(bit)) //sets the corresponding bit in register reg
#define fosc 16000000
#define F_CPU 1600000UL //16 MHz
#define BAUD 9600
#define myubrr fosc/16/BAUD-1
#define CURRENT 1
#define VOLTAGE 0

int main(void)
{
	int voltage, current, power;
	
	//DDRC = 0xFF;                        //configure PORTA to output so led's can be lit for testing
    adcinit();				//initialize ADC
	USART_Init(myubrr);                        //instead of 51,use myubrr
	USART_Init0(myubrr);
	DDRF = 0x00;                        //configure PORTF (ADC) as input so analog signals can be measured
	DDRC = 0xFF;                        //configure PORTA to output so led's can be lit for testing
	DDRA = 0xFF;
	PORTF = 0x00;                        //make sure internal pull up resistors are turned off
	UCSR1B |= (1 << RXCIE); 
	UCSR0B |= (1 << RXCIE); 
	sei();					//enable the global interrupt enable flag so the interupts can be processed
	
	while(1){
			voltage = ADCRead(VOLTAGE);
			USARTsend0(70);
			USARTsend0(voltage);
			delaysec(10.444*2/7);
			
			current = ADCRead(CURRENT);
			USARTsend0(80);
			USARTsend0(current);
			delaysec(10.444*2/7);
			
			power = current * voltage;
			USARTsend0(80);
			USARTsend0(power);
			delaysec(10.444*2/7);
	}
	return 0;
}

//interrupt service routine for usart 0 receive
ISR(USART0_RX_vect){
	int sec;
	unsigned char speed, sendspeed;
	//speed = USARTrecieve0(); //put in an if statement
	speed = UDR0;
	sec = (int)speed;
	lightprotocol(sec);
	sendspeed = speed;
	USARTsend0(sendspeed);
	
}

//interrupt service routine for usart 1 receive
ISR(USART1_RX_vect){
	int sec;
	unsigned char speed, sendspeed;
	//speed = USARTrecieve(); //put in an if statement
	speed = UDR1;
	sec = (int)speed;
	lightprotocol(sec);
	sendspeed = speed;
	USARTsend(sendspeed);
}


/*
adcinit -> initializes the analog to digital conversion

detailed procedure for adcinit can be found on page 21 in notebook
*/
void adcinit(void){
        setbit(ADCSRA,ADEN); //enables ADC
        clearbit(ADCSRA,ADFR); //free running conversion (for setbit) (clearbit is single)
        ADCSRA = ((ADCSRA & 0b11111000)|0b00000110); //selects div by 64 clock prescaler
        ADMUX = ((ADMUX &=0b00111111)|0b01000000); //selects AVcc (~5 volts) as Vref
        clearbit(ADMUX,ADLAR); //selects right adjust of ADC result
        ADMUX &= 0b11100000; //selects single ended conversion of PF0
}

/**********************************************************************
 * @description: Reads current and voltage from ADC ports and returns
 * the scaled value.  Results are scaled by the resolution of the
 * sensors used and any offset present in that signal
 *
 * @param: port selects which adc port is read, 0 is for voltage, 1 is for current
 *
 * @return: current or voltage measurement
 *
 *********************************************************************/
int ADCRead(int port){
	
	DDRF = 000;			//configure PORTF (ADC) as input so analog signals can be measured
	PORTF = 0x00;       //make sure internal pull up resistors are turned off
	float result;
	switch (port){
		case VOLTAGE:
			
			
			ADMUX = (ADMUX & 0xE0) | (0);   //select channel (MUX0-4 bits) 
			setbit(ADCSRA,ADSC);			//start conversion
			while(ADCSRA & 0b01000000);		//wait until the conversion is complete
			result = ((ADCL)|((ADCH)<<8));	//10 bit conversion for channel 0
			result = ((result/51.2));
			//result = ((result-103)/27)*10;
			return result;
			break;
			
			/*
			setbit(ADCSRA,ADSC);                        //start conversion
			while(ADCSRA & 0b01000000);                //wait until the conversion is complete
			result = ((ADCL)|((ADCH)<<8));        //10 bit conversion for channel 0
			return (result*5/256);
			break;
			*/
		case CURRENT:
			//setbit(ADMUX,1);
			ADMUX = (ADMUX & 0xE0) | (1);   //select channel 1
			setbit(ADCSRA,ADSC);			//start conversion
			while(ADCSRA & 0b01000000);		//wait until the conversion is complete
			result = ((ADCL)|((ADCH)<<8));	//10 bit conversion for channel 0
			result = ((result-102)/27);           //conversion from ADC output to Amps.
			return result;
			break;
		default:
			result = 0;
			break;	
	}
	return result;
	
}
/************************************************************************
* @description: Initialize USART1
*
* @param: ubrr: the baud rate variable
*
* @return: nothing
************************************************************************/
void USART_Init(unsigned int ubrr){
        UBRR1H = (unsigned char)(ubrr>>8);		//sets baud rate for usart
        UBRR1L = (unsigned char)(ubrr);         //sets baud rate for usart
        UCSR1B = (1<<RXEN)|(1<<TXEN);           //enables reciever and transmitter
        UCSR1C = (1<<USBS)|(3<<UCSZ1);          //set frame format: 8 data, 2 stop bit
}

/************************************************************************
* @description: Initialize USART0
*
* @param: ubrr: the baud rate variable
*
* @return: nothing
************************************************************************/
void USART_Init0(unsigned int ubrr){
	
	UBRR0H = (unsigned char)(ubrr>>8);		//sets baud rate for usart
	UBRR0L = (unsigned char)(ubrr);         //sets baud rate for usart
	UCSR0B = (1<<RXEN)|(1<<TXEN);           //enables reciever and transmitter
	UCSR0C = (1<<USBS)|(3<<UCSZ0);          //set frame format: 8 data, 2 stop bit
	
}


/************************************************************************
* @description: Send 1 byte of data via UART.
*
* @param: data: Character to be sent.
*
* @return: nothing
************************************************************************/
void USARTsend(unsigned char data){ 
        
        while(!(UCSR1A & (1<<UDRE1)));		 //check to see if there is space in the buffer
        UDR1 = data;                         //if there is space, load data into register/send
        
}

/************************************************************************
* @description: Send 1 byte of data via UART0.
*
* @param: data: Character to be sent.
*
* @return: nothing
************************************************************************/
void USARTsend0(unsigned char data){
	
	while(!(UCSR0A & (1<<UDRE0)));		//check to see if there is space in the buffer
	UDR0 = data;                        //if there is space, load data into register/send
	
}

/************************************************************************
* @description: receive 1 character via UART.
*
* @param: none
*
* @return: UDR0: new data in the buffer
************************************************************************/
 unsigned char USARTrecieve(){                //from radar.c could be unsigned char
        //while(!(UCSR1A & (1<<RXC1))){} //checks to see if there is new data in receive register
        return UDR1;                         //if there is new data, return it
}
/************************************************************************
* @description: receive 1 character via UART.
*
* @param: none
*
* @return: UDR0: new data in the buffer
************************************************************************/
unsigned char USARTrecieve0(){                //from radar.c could be unsigned char
	//while(!(UCSR1A & (1<<RXC1))){} //checks to see if there is new data in receive register
	return UDR0;                         //if there is new data, return it
}


/************************************************************************
* @description: Send multiple characters via UART.
*
* @param: data: Characters to be sent.
*
* @return: diditwork: test variable
************************************************************************/
char USARTstringsend(char* data){
        unsigned char diditwork = 0;
        int i = 0;
        while(data[i] != '\0'){                        
                USARTsend(data[i]);                        // Print string one char at a time
                i++;
        }
        
        diditwork = 1;
        
        return(diditwork);
}
/************************************************************************
* @description: delays process a given amount of milliseconds
*
* @param: numsec: number of seconds to delay
*
* @return: none
************************************************************************/
void delaysec(int numsec){
	int i;
	for(i=0;i<numsec;i++){
		_delay_ms(10000);
	}
}

unsigned char radarget(){
	unsigned char temp[5];
	for(int i=0; i<5;i++){
		temp[i]=USARTrecieve();
	}
	return temp[1];
}


void lightprotocol(int t){
	int sec;
	int distance = 10;
	PORTC = 1;							//time on = speed *distance + 10s
	sec = (t * distance + 10) * 10.444/7 *0.2 ; //see notebook for calibration
	delaysec(sec);
	PORTC = 0;
}
