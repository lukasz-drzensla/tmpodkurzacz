/**************************************************************
 * This file is a part of the PWM Demo (C).                   *
 **************************************************************/
/**
 * @file main.c
 * @author Koryciak
 * @date Nov 2020 
 * @brief File containing the main function. 
 * @ver 0.5
 */
 
#include "frdm_bsp.h" 
#include "led.h" 
#include "lcd1602.h" 
#include "tsi.h"
#include "pit.h"
#include "tpm.h" // ToDo 2.2: Attach TPM header
//#include "tpm_pcm.h"  // ToDo 4.1 Attach TPM_PCM header (detach tpm.h)

/******************************************************************************\
* Private prototypes
\******************************************************************************/
void SysTick_Handler(void);
void lcd_static(void);
void lcd_update(void);
/******************************************************************************\
* Private memory declarations
\******************************************************************************/
static uint8_t sliderVal = 50;
static uint8_t sliderOld = 0;
static int msTicks = 0;
static uint8_t newTick = 0;

int sensor0 = 0;
int sensor1 = 1;

 #define trig_pin 6 //pin of the first sensor
 
 void trig_init()
 {
	 //first sensor
	 SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
	 PORTB->PCR[trig_pin] = PORT_PCR_MUX(1); //MUX config as GPIO
	 PTB -> PDDR |= (1<<trig_pin);
	 PTB->PSOR |= (1<<trig_pin);
	 
	 PORTB->PCR[trig_pin+1] = PORT_PCR_MUX(1); //MUX config as GPIO
	 PTB -> PDDR |= (1<<(trig_pin+1));
	 PTB->PSOR |= (1<<(trig_pin+1));
 }
 
void trigger (int sensor)
{
			PTB->PCOR |= (1<<(trig_pin+sensor));
			for (int j =0; j < 60; j++)
			{
				DELAY(0);
			}
			PTB->PSOR |= (1<<(trig_pin+sensor));
}

int get_distance(uint32_t value)
{
	return (int)(10*value-6)/19;
}
 
int main (void) { 
	
	uint8_t sliderTemp;
	
	//TSI_Init ();  													/* initialize slider */ 

	LCD1602_Init (); 												/* initialize LCD */ 
	lcd_static(); 													/* display default on LCD */

	SysTick_Config(1000000); 								/* initialize system timer */
	
				// ToDo 2.2: Enable TPM1 initialization  // ToDo 3.2: Disable TPM1 initialization
	
	trig_init(); //initialize all the triggers
	TPM1_Init_InputCapture (0);
	TPM0_Init_InputCapture();
	
  while(1) {
		__WFI();															/* sleep & wait for interrupt */

		// simple schedule
		if (newTick) {

				trigger(0); //always trigger
				trigger(1); //always trigger

			newTick = 0;												/* clear flag & choose task */

			// task 2 - refresh display
			if( msTicks%5 == 0) {

				//sensor0=get_distance(TPM1_GetVal());
				//sensor1=get_distance(TPM0_GetVal());
				int temp0 = get_distance (TPM1_GetVal());
				int temp1 = get_distance (TPM0_GetVal());
				if (temp0 < 500)
				{
					sensor0 = temp0;
				}
				if (temp1 < 500)
				{
					sensor1=temp1;
				}
					
				lcd_update();
			}
		}
	} /* end_while */
}

/**
 * @brief System time update. 
 */
void SysTick_Handler(void) {
	msTicks++;
	newTick = 1;
}
/**
 * @brief Display on LCD static text and default values. 
 */
void lcd_static(void) {
	
	LCD1602_ClearAll();
	LCD1602_SetCursor(0, 0);
	LCD1602_Print("Sensor0 = ");
	LCD1602_SetCursor(0, 1);
	LCD1602_Print("Sensor1 = "); // ToDo 2.5: Enable to see result on LCD
	lcd_update();								// ToDo 2.5: Enable to see result on LCD
}
/**
 * @brief Update values on LCD. 
 */
void lcd_update(void) {

		LCD1602_SetCursor(9, 0);
		LCD1602_PrintNum(sensor0);
	LCD1602_SetCursor(9, 1);  // ToDo 2.5: Enable to see result on LCD
	LCD1602_PrintNum(sensor1); // ToDo 2.5: Enable to see result on LCD
}
