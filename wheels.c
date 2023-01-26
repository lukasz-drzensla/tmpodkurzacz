#include "wheels.h"

void wheels_init()
{
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
	PORTB->PCR[WHEEL_LEFT_B] = PORT_PCR_MUX(1); //MUX config as GPIO
	PORTB->PCR[WHEEL_RIGHT_B] = PORT_PCR_MUX(1); //MUX config as GPIO	
	PORTB->PCR[WHEEL_LEFT_F] = PORT_PCR_MUX(1); //MUX config as GPIO
	PORTB->PCR[WHEEL_RIGHT_F] = PORT_PCR_MUX(1); //MUX config as GPIO
	PTB->PDDR |= (1 << WHEEL_LEFT_B) | (1 << WHEEL_RIGHT_B) | (1 << WHEEL_LEFT_F) | (1 << WHEEL_RIGHT_F); //wheels as output
}

int n = 0;

void forward()
{
	clear();
	n+=1;
	if (n%3!=0)
	{	
		PTB->PDOR &= ~((1 << WHEEL_LEFT_B) | (1 << WHEEL_RIGHT_B));		
	} else {
		n=0;
	}
}

void back()
{
	clear();
	PTB->PDOR &= ~((1 << WHEEL_LEFT_F) | (1 << WHEEL_RIGHT_F));		

}
void turn_right()
{
	clear();
	PTB->PDOR &= ~((1 << WHEEL_LEFT_F));		

}

void turn_left()
{
	clear();
	PTB->PDOR &= ~((1 << WHEEL_RIGHT_F));		
}

void clear() {
	PTB->PSOR = ((1 << WHEEL_LEFT_B) | (1 << WHEEL_RIGHT_B) | (1 << WHEEL_LEFT_F) | (1 << WHEEL_RIGHT_F));
}

