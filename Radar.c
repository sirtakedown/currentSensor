/*
 * Radar.c
 *
 * Created: 10/17/2013 2:58:14 PM
 *  Author: Matthew
 */ 

#define F_CPU 16000000UL        //Says to the compiler which is our clock frequency, permits the delay functions to be very accurate
#include <avr/io.h>            //General definitions of the registers values
#include <util/delay.h>            //This is where the delay functions are located

void USARTinit(void);
void USARTsend(unsigned char data);
unsigned char USARTrecieve(void);

int radar(void){                //The begin of our main function
	//Start and setup USART
	//function to send byte
	//function to receive byte
	return 0;                //As usual all non-void functions must have a return
}