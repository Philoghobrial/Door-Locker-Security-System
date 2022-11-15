/*
 * timer.c
 * Module : Timer
 *  Created on: Oct 6, 2022
 *      Author: philopater ghobrial
 */
#include "timer.h"
#include "common_macros.h" /* To use the macros like SET_BIT */
#include <avr/io.h>
#include "gpio.h"
#include <avr/interrupt.h>

/*******************************************************************************
 *                     Global variables                                 *
 *******************************************************************************/


static volatile void (*g_callBackPtr)(void) = NULL_PTR;

/*******************************************************************************
 *                     ISR Definitions                                  *
 *******************************************************************************/

ISR(TIMER1_OVF_vect){ //ISR for Timer in normal mode
	if(g_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*g_callBackPtr)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}

}

ISR(TIMER1_COMPA_vect){ //ISR for timer in CTC mode
	if(g_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*g_callBackPtr)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}

}

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/


/*
 * Description: Function to initialize timer according to choosen configuration
 */
void Timer1_init(const Timer1_ConfigType (* Config_Ptr)){
	TCCR1B= (TCCR1B&0xF7) | ((Config_Ptr->mode)<<3); //choose mode of timer between CTC and normal(overflow)
	TCCR1B= ((TCCR1B&0xF8) | (Config_Ptr->prescaler));
	TCNT1=Config_Ptr->initial_value;//value timer starts counting from
	if(Config_Ptr->mode==CTC){
		OCR1A=Config_Ptr->compare_value; //value compared with in CTC mode
		TIMSK |= (1<<OCIE1A);//enable CTC interrupt
		TCCR1A |= (1<<FOC1A); //timer work at compare mode
	}
	else if(Config_Ptr->mode==0){
		TIMSK |= (1<<TOIE1); //enable overflow interrupt
	}

}

/*
 * Description: Function to set the Call Back function address.
 */
void Timer1_setCallBack(void(*a_ptr)(void))
{
	/* Save the address of the Call back function in a global variable */
	g_callBackPtr = a_ptr;
}


void PWM_Timer0_Start(uint8 duty_cycle){

	TCNT0 = timer_initial_value; // Set Timer Initial Value to 0
	OCR0  = duty_cycle; // Set Compare Value
	GPIO_setupPinDirection(PORTD_ID,PIN4_ID,PIN_OUTPUT);
	// Configure PB3/OC0 as output pin --> pin where the PWM signal is generated from MC

	/* Configure timer control register
	 * 1. Fast PWM mode FOC0=0
	 * 2. Fast PWM Mode WGM01=1 & WGM00=1
	 * 3. Clear OC0 when match occurs (non inverted mode) COM00=0 & COM01=1
	 * 4. clock = F_CPU/8 CS00=0 CS01=1 CS02=0
	 */
	SET_BIT(TCCR0,WGM00);
	SET_BIT(TCCR0,WGM01);
	CLEAR_BIT(TCCR0,FOC0);
	SET_BIT(TCCR0,COM01);
	CLEAR_BIT(TCCR0,COM00);
	CLEAR_BIT(TCCR0,CS00);
	SET_BIT(TCCR0,CS01);
	CLEAR_BIT(TCCR0,CS02);

}

/*
 * Description: Function to de-initialize timer function.
 * put all timer registers with zero
 */
void Timer1_deInit(void){
	TCCR1B=0;
	TIMSK=0;
	TCCR1A = 0;
}
