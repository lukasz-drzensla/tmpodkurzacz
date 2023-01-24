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
#include "tpm.h" 
void clear();

//kola
#define wheelLeftF 5
#define wheelRightB 1


#define wheelLeftB 2
#define wheelRightF 13

#define trig_pin 6 //pin of the first sensor


void wheels_init()
{
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
	PORTB->PCR[wheelLeftB] = PORT_PCR_MUX(1); //MUX config as GPIO
	PORTB->PCR[wheelRightB] = PORT_PCR_MUX(1); //MUX config as GPIO	
	PORTB->PCR[wheelLeftF] = PORT_PCR_MUX(1); //MUX config as GPIO
	PORTB->PCR[wheelRightF] = PORT_PCR_MUX(1); //MUX config as GPIO
	PTB -> PDDR |= (1<<wheelLeftB) | (1<<wheelRightB) | (1<<wheelLeftF) | (1<<wheelRightF); //wheels as output
}


int n = 0;


void forward()
{
	clear();	
	n+=1;
	if (n%2==0){
		PTB->PDOR &= ~(( 1 << wheelLeftB ) | (1 << wheelRightB));		//set LED as ON
		n = 0;
	}
	
}

void back()
{
	clear();
	PTB->PDOR &= ~(( 1 << wheelLeftF ) | (1 << wheelRightF));		//set LED as ON
	
}
void TurnRight_90()
{
	clear();
	PTB->PDOR &= ~((1 << wheelLeftF));		//set LED as ON
	
}

void TurnLeft_90()
{
	clear();
	PTB->PDOR &= ~(( 1 << wheelRightF ));		//set LED as ON	
}



/******************************************************************************\
* Private prototypes
\******************************************************************************/
void SysTick_Handler(void);
void lcd_static(void);
void lcd_update(void);
/******************************************************************************\
* Private memory declarations
\******************************************************************************/
static int msTicks = 0;
static uint8_t newTick = 0;

int sensor0 = 0;
int sensor1 = 0;

 
 void trig_init()
 {
	 //first sensor
	 SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
	 PORTB->PCR[trig_pin] = PORT_PCR_MUX(1); //MUX config as GPIO
	 PTB -> PDDR |= (1<<trig_pin);
	 PTB->PCOR |= (1<<trig_pin);
	 
	 PORTB->PCR[trig_pin+1] = PORT_PCR_MUX(1); //MUX config as GPIO
	 PTB -> PDDR |= (1<<(trig_pin+1));
	 PTB->PCOR |= (1<<(trig_pin+1));
 }
 
void trigger (int sensor)
{
	
			PTB->PSOR |= (1<<(trig_pin+sensor));
			for (int j =0; j < 60; j++)
			{
				DELAY(0);
			}
			PTB->PCOR |= (1<<(trig_pin+sensor));
			
}

void clear(){
	PTB -> PSOR = ((1<<wheelLeftB) | (1<<wheelRightB) | (1<<wheelLeftF) | (1<<wheelRightF));
}

int get_distance(uint32_t value)
{
	return (int)(10*value-6)/19;
}
 


int newSensorData = 0;


int corr_right = 0;
int corr_cnt = 0;

int main (void) { 

	wheels_init();
	trig_init();

	LCD1602_Init (); 												/* initialize LCD */ 
	lcd_static(); 													/* display default on LCD */

	SysTick_Config(5000000); 								//over 60 ms measurement cycle is recommended
		
	TPM1_Init_InputCapture ();
	TPM0_Init_InputCapture();
	
  while(1) {
		__WFI();															/* sleep & wait for interrupt */

		// simple schedule
		if (newTick) {	
			
					trigger(0); 
					trigger(1);
					newTick=0;
				
		}	
    
		newSensorData = 0; //reset the flag
		if(getNewDist1()) { //newDist is set to 1 in TPM1 cnt 
			resetNewDist1();
			int temp0=get_distance(TPM1_GetVal());
			if(temp0 < 500)
					sensor0=temp0;
			newSensorData = 1;	
		}
	
		if(getNewDist0()){ //newDist is set to 1 in TPM0 cnt 
				resetNewDist0();
				int temp1=get_distance(TPM0_GetVal());
				if (temp1 < 500)
					sensor1=temp1;
				newSensorData = 1;
		}
				
		if(newSensorData==1){	
				lcd_update();				
			
			
				if (corr_right == 1)
				{
					corr_cnt += 1;
					if (corr_cnt > 10)
					{
						corr_cnt = 0;
						corr_right = 0;
					}
				}
			
			
				if (sensor1 > 25)
				{
					if (corr_right == 0)
					{
						corr_right=1;
						TurnRight_90();
						continue;
					}										
				}
			
				if (sensor0 > 25)
				{
					forward();
				} else {					
					TurnLeft_90();		
				}

			}
	} 
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
