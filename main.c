#include "frdm_bsp.h" 
#include "led.h" 
#include "lcd1602.h" 
#include "tpm.h" 

//function prototypes
void clear(); //sets wheels inactive (not moving)
void SysTick_Handler(void);
void lcd_static(void);
void lcd_update(void);

//ticks for systick
static int msTicks = 0;
static uint8_t newTick = 0;

//readings from the sensors
int sensor0 = 0;
int sensor1 = 0;

//wheels pins
#define WHEEL_LEFT_F 5
#define WHEEL_RIGHT_B 1
#define WHEEL_LEFT_B 2
#define WHEEL_RIGHT_F 13

#define TRIG_PIN 6 //pin of the first sensor


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
	n += 1;
	if (n % 2 == 0) {
		PTB->PDOR &= ~((1 << WHEEL_LEFT_B) | (1 << WHEEL_RIGHT_B));		//set LED as ON
		n = 0;
	}

}

void back()
{
	clear();
	PTB->PDOR &= ~((1 << WHEEL_LEFT_F) | (1 << WHEEL_RIGHT_F));		//set LED as ON

}
void turn_right()
{
	clear();
	PTB->PDOR &= ~((1 << WHEEL_LEFT_F));		//set LED as ON

}

void turn_left()
{
	clear();
	PTB->PDOR &= ~((1 << WHEEL_RIGHT_F));		//set LED as ON	
}

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

void clear() {
	PTB->PSOR = ((1 << WHEEL_LEFT_B) | (1 << WHEEL_RIGHT_B) | (1 << WHEEL_LEFT_F) | (1 << WHEEL_RIGHT_F));
}

int get_distance(uint32_t value)
{
	return (int)(10 * value - 6) / 19;
}



int newSensorData = 0; //flag whether new data has been read
int left_turns = 0; //how many left turns have been made so far?

int corr_right = 0;
int corr_right_cnt = 0;

int main(void) {

	wheels_init();
	trig_init();

	LCD1602_Init(); 												/* initialize LCD */
	lcd_static(); 													/* display default on LCD */

	SysTick_Config(5000000); 								//over 60 ms measurement cycle is recommended

	TPM1_Init_InputCapture();
	TPM0_Init_InputCapture();

	while (1) {
		__WFI();															/* sleep & wait for interrupt */

		// simple schedule
		if (newTick) {

			trigger(0);
			trigger(1);
			newTick = 0;

		}

		newSensorData = 0; //reset the flag
		if (getNewDist1()) { //newDist is set to 1 in TPM1 cnt 
			resetNewDist1();
			int temp0 = get_distance(TPM1_GetVal());
			if (temp0 < 500)
				sensor0 = temp0;
			newSensorData = 1;
		}

		if (getNewDist0()) { //newDist is set to 1 in TPM0 cnt 
			resetNewDist0();
			int temp1 = get_distance(TPM0_GetVal());
			if (temp1 < 500)
				sensor1 = temp1;
			newSensorData = 1;
		}

		if (newSensorData == 1) {
			lcd_update();


			if (corr_right == 1)
			{
				corr_right_cnt += 1;
				if (corr_right_cnt > 10)
				{
					corr_right_cnt = 0;
					corr_right = 0;
				}
			}

			if (sensor1 < 5)
			{
				
				continue;
			}

			if (sensor1 > 10 + left_turns)
			{
				if (corr_right == 0)
				{
					corr_right = 1;
					turn_right();
					continue;
				}
			}

			if (sensor0 > 25 + left_turns)
			{
				forward();
			}
			else {
				turn_left();
				if (!corr_right)
				{
					left_turns += 1;
				}
			}

		}
	}
}

void SysTick_Handler(void) {
	msTicks++;
	newTick = 1;
}

void lcd_static(void) {

	LCD1602_ClearAll();
	LCD1602_SetCursor(0, 0);
	LCD1602_Print("Sensor0 = ");
	LCD1602_SetCursor(0, 1);
	LCD1602_Print("Sensor1 = "); // ToDo 2.5: Enable to see result on LCD
	lcd_update();								// ToDo 2.5: Enable to see result on LCD
}

void lcd_update(void) {

	LCD1602_SetCursor(9, 0);
	LCD1602_PrintNum(sensor0);
	LCD1602_SetCursor(9, 1);  // ToDo 2.5: Enable to see result on LCD
	LCD1602_PrintNum(sensor1); // ToDo 2.5: Enable to see result on LCD
}
 