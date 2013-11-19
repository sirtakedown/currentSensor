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
void USARTsend(unsigned char data);
unsigned char USARTrecieve();
void USARTflush();
char USARTstringsend(char* data);
void delaysec(int numsec);
int ADCRead(int port);
int main(void);
unsigned char radarget();



#endif