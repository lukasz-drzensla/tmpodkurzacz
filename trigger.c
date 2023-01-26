#include "trigger.h"

void trig_init()
{
	//first sensor
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
	PORTB->PCR[TRIG_PIN1] = PORT_PCR_MUX(1); //MUX config as GPIO
	PTB->PDDR |= (1 << TRIG_PIN1);
	PTB->PCOR |= (1 << TRIG_PIN1);

	PORTB->PCR[TRIG_PIN2] = PORT_PCR_MUX(1); //MUX config as GPIO
	PTB->PDDR |= (1 << (TRIG_PIN2));
	PTB->PCOR |= (1 << (TRIG_PIN2));
}

void trigger(int sensor)
{
if (sensor == 0)
{
	PTB->PSOR |= (1 << (TRIG_PIN1));
	for (int j = 0; j < 60; j++)
	{
		DELAY(0);
	}
	PTB->PCOR |= (1 << (TRIG_PIN1));
} else if (sensor == 1)
{
	PTB->PSOR |= (1 << (TRIG_PIN2));
	for (int j = 0; j < 60; j++)
	{
		DELAY(0);
	}
	PTB->PCOR |= (1 << (TRIG_PIN2));
}
}
