/*
 * Radar.c
 *
 * Created: 10/17/2013 2:58:14 PM
 *  Author: Matthew Herbert
 */ 

#define CPU_Freq 16000000UL        //Says to the compiler which is our clock frequency, permits the delay functions to be very accurate
#include <avr/io.h>            //General definitions of the registers values
#include <util/delay.h>            //This is where the delay functions are located

#define BAUDRATE 9600
#define BAUDscaller (((CPU_Freq / (BAUDRATE * 16UL))) - 1)

void USARTinit(void);
void USARTsend(unsigned char data);
unsigned char USARTrecieve(void);



int radar(void){                //The begin of our main function
	//Start and setup USART
	//function to send byte
	//function to receive byte
	return 0;                //As usual all non-void functions must have a return
}

void USARTinit(void){
	
	UBRR0H = (uint8_t)(BAUDscaller>>8); //register to set the baudrate
	UBRR0L = (uint8_t)(BAUDscaller);	//register to set the baudrate
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);		//register to control whether Tx(transmit, by the TXEN0)
										//  by the TXEN0) and Rx (recieve, RXEN0) are activated or not
	UCSR0C = ((1<<UCSZ00)|(1<<UCSZ01)); //configuration bits, protocol specific. 8N1 settings as its common
}

void USARTsend(unsigned char data){
	
	while(!(UCSR0A & (1<<UDRE0))); //check to see if there is space in the buffer
	UDR0 = data;				   //if there is space, load data into register/send
	
}