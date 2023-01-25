#include "trigger.h"

void trig_init()
{
	//first sensor
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
	PORTB->PCR[TRIG_PIN] = PORT_PCR_MUX(1); //MUX config as GPIO
	PTB->PDDR |= (1 << TRIG_PIN);
	PTB->PCOR |= (1 << TRIG_PIN);

	PORTB->PCR[TRIG_PIN + 1] = PORT_PCR_MUX(1); //MUX config as GPIO
	PTB->PDDR |= (1 << (TRIG_PIN + 1));
	PTB->PCOR |= (1 << (TRIG_PIN + 1));
}

void trigger(int sensor)
{

	PTB->PSOR |= (1 << (TRIG_PIN + sensor));
	for (int j = 0; j < 60; j++)
	{
		DELAY(0);
	}
	PTB->PCOR |= (1 << (TRIG_PIN + sensor));

}
