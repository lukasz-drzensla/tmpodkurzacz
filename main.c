#include "frdm_bsp.h" 
#include "tpm.h" 
#include "wheels.h"
#include "lcd1602.h" 
#include "trigger.h"

//function prototypes
void SysTick_Handler(void);
void lcd_static(void);
void lcd_update(void);

//ticks for systick
static int msTicks = 0;
static uint8_t newTick = 0;

//readings from the sensors
int sensor0 = 0;
int sensor1 = 0;


int get_distance(uint32_t value)
{
	return (int)(10 * value - 6) / 19;
}



int newSensorData = 0; //flag whether new data has been read
int left_turns = 0; //how many left turns have been made so far?

int corr_right = 0;
int corr_right_cnt = 0;

int corr_left = 0;
int corr_left_cnt = 0;

int main(void) {

	wheels_init();
	trig_init();
	
	LCD1602_Init(); 												/* initialize LCD */
	lcd_static(); 		

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

			if (corr_left == 1)
			{
				corr_left_cnt += 1;
				if (corr_left_cnt > 10)
				{
					corr_left_cnt = 0;
					corr_left = 0;
				}
			}

			if (sensor1 < 5 + left_turns)
			{
				if (corr_left == 0){
					corr_left=1;
					turn_left();
					continue;
				}
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

