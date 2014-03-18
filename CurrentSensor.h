/*
 * CurrentSensor.h
 *
 * Created: 11/11/2013 9:30:38 PM
 *  Author: Matthew
 */ 


#ifndef CURRENTSENSOR_H_
#define CURRENTSENSOR_H_

void adcinit(void);
void USART_Init(unsigned int ubrr);
void USART_Init0(unsigned int ubrr);
void USARTsend(unsigned char data);
void USARTsend0(unsigned char data);
unsigned char USARTrecieve();
unsigned char USARTrecieve0();
void delaysec(int numsec);
int ADCRead(int port);
int main(void);
void lightprotocol(int t);

#endif