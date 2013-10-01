/*
 * CurrentSensor.c
 *
 * Created: 9/28/2013 2:12:45 PM
 *  Author: Matthew
 */ 


#include <avr/io.h>

#define ByteValue(bit) (1<<(bit))  //converts the bit into a byte value
#define setbit(reg,bit) reg &= ~(ByteValue(bit))  //sets the corresponding bit in register reg
#define clearbit(reg,bit) reg |= (ByteValue(bit)) //clears the corresponding bit in regieter reg

void adcinit(void);

int main(void)
{
    while(1)
    {
        //TODO:: Please write your application code 
    }
}

void adcinit(void){
	setbit(ADCSRA,ADEN); //enables ADC
	setbit(ADCSRA,ADFR); //free running conversion
	ADCSRA = ((ADCSRA & 0b11111000)|0b00000110); //selects div by 64 clock prescaler
	ADMUX = ((ADMUX &=0b00111111)|0b01000000);   //selects AVcc (~5 volts) as Vref
	clearbit(ADMUX,ADLAR); //selects right adjust of ADC result
	ADMUX &= 0b11100000;   //selects single ended conversion of PF0
}