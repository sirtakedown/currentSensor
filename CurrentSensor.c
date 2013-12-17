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

	

	unsigned char speed, sendspeed;
	int voltage, current, sec, i, avg;
	int distance = 10;
	int ADCarray[100];
	
	//DDRC = 0xFF;                        //configure PORTA to output so led's can be lit for testing
    adcinit();				//initialize ADC
	USART_Init(myubrr);                        //instead of 51,use myubrr
	USART_Init0(myubrr);
	DDRF = 0x00;                        //configure PORTF (ADC) as input so analog signals can be measured
	DDRC = 0xFF;                        //configure PORTA to output so led's can be lit for testing
	PORTF = 0x00;                        //make sure internal pull up resistors are turned off
	while(1){ //should be ADCREAD instead of 1
			//voltage = ADCRead(VOLTAGE);
			/*
			ADCarray[i] = ADCRead(VOLTAGE);
			i++;
		//voltage = ADCRead(VOLTAGE);
			if(i>=100){
				i = 0;
			}
			avg = ADCavg(ADCarray);
		*/
		
		
		//check both USARTS
			if((UCSR0A & (1<<RXC0))){
				speed = USARTrecieve0(); //put in an if statement
				sec = (int)speed;
				lightprotocol(sec);
				sendspeed = speed;
				USARTsend0(sendspeed);
			}
		
			else if((UCSR1A & (1<<RXC1))){		//checks to see if there is new data in receive register
			 speed = USARTrecieve(); //put in an if statement
			 sec = (int)speed;
			 lightprotocol(sec);
			 sendspeed = speed;
			 USARTsend(sendspeed);
			}
			
			 /*
			 PORTC = 1;							//time on = speed *distance + 10s
			 sec = (sec * distance + 10) * 10.444/7; //see notebook for calibration
			 delaysec(sec);
			 PORTC = 0;
			 */
			 
			// sendspeed = speed;
			 //USARTsend(sendspeed);
			 for(int i=0;i<20000;i++){}
			//DO SERIAL STUFF
			//USART_init
			//if(USART_recieve != 0) INTERPRET SPEED AND THEN LIGHT PROTOCALL
			//else if(radar != 0)	DETERMINE SPEED AND LIGHT PROTOCALL, THEN SEND DATA
			 
			
			//voltage = ADCRead(VOLTAGE);
		//}
		//PORTC = ADCRead(CURRENT);	
	}
	return 0; 

/*
        unsigned char test = 35;                //ascii for 35
        unsigned char speed;
		unsigned char sendspeed;
		int distance = 10;
		int sec;
        USART_Init(myubrr);                        //instead of 51,use myubrr
        DDRC = 0xFF;                        //configure PORTA to output so led's can be lit for testing
        PORTC= test;
        while(1){
                speed = USARTrecieve();
				sec = (int)speed;
				PORTC = sec;							//time on = speed *distance + 10s
				sec = (sec * distance + 10) * 10.444/7; //see notebook for calibration
				delaysec(sec);
				PORTC = 0;
				
				
                sendspeed = speed;
			    USARTsend(sendspeed);
                for(int i=0;i<20000;i++){}
                
                        
        }

return 0;
*/
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
/*
	int result;
	setbit(ADCSRA,ADSC);                        //start conversion
	while(ADCSRA & 0b01000000);                //wait until the conversion is complete
	result = ((ADCL)|((ADCH)<<8));        //10 bit conversion for channel 0
		//result = (result/1024)*5;
		//if any result is read, light LED
		//PORTA = 0xFF;
	return  (result*5/256);
*/		
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
			//result = ((result-102)/27)*10;
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
        UBRR1H = (unsigned char)(ubrr>>8); //sets baud rate for usart
        UBRR1L = (unsigned char)(ubrr);         //sets baud rate for usart
        UCSR1B = (1<<RXEN)|(1<<TXEN);         //enables reciever and transmitter
        UCSR1C = (1<<USBS)|(3<<UCSZ1);         //set frame format: 8 data, 2 stop bit
}

/************************************************************************
* @description: Initialize USART0
*
* @param: ubrr: the baud rate variable
*
* @return: nothing
************************************************************************/
void USART_Init0(unsigned int ubrr){
	UBRR0H = (unsigned char)(ubrr>>8); //sets baud rate for usart
	UBRR0L = (unsigned char)(ubrr);         //sets baud rate for usart
	UCSR0B = (1<<RXEN)|(1<<TXEN);         //enables reciever and transmitter
	UCSR0C = (1<<USBS)|(3<<UCSZ0);         //set frame format: 8 data, 2 stop bit
}


/************************************************************************
* @description: Send 1 byte of data via UART.
*
* @param: data: Character to be sent.
*
* @return: nothing
************************************************************************/
void USARTsend(unsigned char data){ //from radar.c
        
        while(!(UCSR1A & (1<<UDRE1))); //check to see if there is space in the buffer
        UDR1 = data;                                 //if there is space, load data into register/send
        
}

/************************************************************************
* @description: Send 1 byte of data via UART0.
*
* @param: data: Character to be sent.
*
* @return: nothing
************************************************************************/
void USARTsend0(unsigned char data){ //from radar.c
	
	while(!(UCSR0A & (1<<UDRE0))); //check to see if there is space in the buffer
	UDR0 = data;                                 //if there is space, load data into register/send
	
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
* @description: flush the USART
*
* @param: nothing
*
* @return: nothing
************************************************************************/
void USARTflush(void){                         //from atmega128 datasheet
        unsigned char dummy;                 //buffer to be emptied when receiver is disabled
        while( UCSR0A & (1<<RXC0) ){
			 dummy = UDR0;
		}
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


int ADCavg(int a[]){
	int j, sum;
	for(j=0;j<100;j++){
		sum += a[j];
	}
	return sum/100;
}


/*
void lcdshow(void){
	

		
		
		unsigned int temp;
		DDRA = 0xFE;		//set the PORTA Direction Set every pin of PORTA as out except AN0
		DDRB = 0xFF;		//set the PORTB Direction Set every pin of PORTB as out as our lcd on this
		

		ADCSRA=0X00;		// CODE for ADC demo (optional)
		ADMUX = 0x40;
		ADCSRA = 0x87;
		
		
		lcdInit();

		while(1){
		prints("LCDATMEGA128");
		}
		while(1)
		{
			
			ADCSRA |= 0x40;			// start the adc conversion on AN0
			while(ADCSRA & 0x40);
			temp = ADC;

			gotoXy(1,1 );			//set the cursor to 1 column of 1st row
			prints("ADC = ");
			integerToLcd(temp);		// print adc value to the lcd

			
			_delay_ms(300);
			
			
		}
}

*/
