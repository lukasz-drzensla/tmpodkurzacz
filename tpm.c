/******************************************************************************
 * This file is a part of the Sysytem Microprocessor Tutorial (C).            *
 ******************************************************************************/

/**
 * @file tpm.c
 * @author Koryciak
 * @date Nov 2020
 * @brief File containing definitions for TPM.
 * @ver 0.1
 */

#include "tpm.h"

/******************************************************************************\
* Private definitions
\******************************************************************************/
/******************************************************************************\
* Private prototypes
\******************************************************************************/
void TPM1_IRQHandler(void);
/******************************************************************************\
* Private memory declarations
\******************************************************************************/
static uint16_t tpm1Diff = 0;
static uint16_t tpm1New = 0;
static uint16_t tpm1Old = 0;

static uint16_t tpm0Diff = 0;
static uint16_t tpm0New = 0;
static uint16_t tpm0Old = 0;

static uint8_t tpm0Enabled = 0;

void TPM0_Init_InputCapture() {
		SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;		// ToDo 2.1.1: Enable TPM1 mask in SCGC6 register
		SIM->SOPT2 |= SIM_SOPT2_TPMSRC(2);// ToDo 2.1.1: Choose MCGFLLCLK clock source
		SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);
	
		SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK; // ToDo 2.1.2: Connect port A to clock
		PORTA->PCR[5] = PORT_PCR_MUX(2);  // ToDo 2.1.2: Set multiplekser to TPM1 for PTA0, get channel number (page 148 of the Reference Manual)
	
		TPM0->SC |= TPM_SC_PS(128);  				// ToDo 2.1.3: Set prescaler to 128
		TPM0->SC |= TPM_SC_CMOD(1);					// ToDo 2.1.4: For TMP1, select the internal input clock source
	
// ToDo 2.1.5: Connect correct channel from TPM1 for "input capture" mode
		TPM0->SC &= ~TPM_SC_CPWMS_MASK; 		/* up counting */
		TPM0->CONTROLS[5].CnSC &= ~ (TPM_CnSC_MSA_MASK | TPM_CnSC_MSB_MASK);
		TPM0->CONTROLS[5].CnSC |= (TPM_CnSC_ELSA_MASK | TPM_CnSC_ELSB_MASK); /* capture on both edges */ 
		TPM0->CONTROLS[5].CnSC |= TPM_CnSC_CHIE_MASK; // ToDo 2.1.6: Enable interrupt on selected channel
	
		NVIC_SetPriority(TPM0_IRQn, 1);  /* TPM1 interrupt priority level  */

		NVIC_ClearPendingIRQ(TPM1_IRQn); 
		NVIC_EnableIRQ(TPM0_IRQn);	/* Enable Interrupts */

}

void TPM1_Init_InputCapture(int port) {
		SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK;		// ToDo 2.1.1: Enable TPM1 mask in SCGC6 register
		SIM->SOPT2 |= SIM_SOPT2_TPMSRC(2);// ToDo 2.1.1: Choose MCGFLLCLK clock source
		SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);
	
		SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK; // ToDo 2.1.2: Connect port A to clock
		PORTA->PCR[port] = PORT_PCR_MUX(2);  // ToDo 2.1.2: Set multiplekser to TPM1 for PTA0, get channel number (page 148 of the Reference Manual)
	
		TPM1->SC |= TPM_SC_PS(128);  				// ToDo 2.1.3: Set prescaler to 128
		TPM1->SC |= TPM_SC_CMOD(1);					// ToDo 2.1.4: For TMP1, select the internal input clock source
	
// ToDo 2.1.5: Connect correct channel from TPM1 for "input capture" mode
		TPM1->SC &= ~TPM_SC_CPWMS_MASK; 		/* up counting */
		TPM1->CONTROLS[0].CnSC &= ~ (TPM_CnSC_MSA_MASK | TPM_CnSC_MSB_MASK);
		TPM1->CONTROLS[0].CnSC |= (TPM_CnSC_ELSA_MASK | TPM_CnSC_ELSB_MASK); /* capture on both edges */ 
		TPM1->CONTROLS[0].CnSC |= TPM_CnSC_CHIE_MASK; // ToDo 2.1.6: Enable interrupt on selected channel
	
		NVIC_SetPriority(TPM1_IRQn, 3);  /* TPM1 interrupt priority level  */

		NVIC_ClearPendingIRQ(TPM1_IRQn); 
		NVIC_EnableIRQ(TPM1_IRQn);	/* Enable Interrupts */

}


uint32_t TPM1_GetVal() {
	return tpm1Diff;
}

uint32_t TPM0_GetVal() {
	return tpm0Diff;
}

void TPM0_IRQHandler(void) {
	
	tpm0Old = tpm0New;
	tpm0New = TPM0->CONTROLS[5].CnV & 0xFFFF;  // ToDo 2.4 Enable saving counter value
	tpm0Diff = tpm0New - tpm0Old;	/* calculate difference */
	//LED_Ctrl(LED_RED, LED_TOGGLE); // ToDo 2.1.7: Enable red led toggle
	TPM0->CONTROLS[5].CnSC |= TPM_CnSC_CHF_MASK; // ToDo 2.1.8: Clear channel flag
	
}
/**
 * @brief Interrupt handler for TPM1.
 */
void TPM1_IRQHandler(void) {
	
	tpm1Old = tpm1New;
	tpm1New = TPM1->CONTROLS[0].CnV & 0xFFFF;  // ToDo 2.4 Enable saving counter value
	tpm1Diff = tpm1New - tpm1Old;	/* calculate difference */
	//LED_Ctrl(LED_RED, LED_TOGGLE); // ToDo 2.1.7: Enable red led toggle
	TPM1->CONTROLS[0].CnSC |= TPM_CnSC_CHF_MASK; // ToDo 2.1.8: Clear channel flag
	
}
