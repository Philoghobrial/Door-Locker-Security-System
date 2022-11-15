/*
 * motor.c
 * Module: Motor
 *  Created on: Oct 5, 2022
 *      Author: philopater ghobrial
 */
#include "common_macros.h" /* To use the macros like SET_BIT */
#include <avr/io.h>
#include "motor.h"
#include "gpio.h"



/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
/*
 * function description:
 * initialize the pins in portD as output
 *
 */
void DcMotor_init(void){
	GPIO_setupPinDirection(PORTD_ID,PIN2_ID,PIN_OUTPUT);
	GPIO_setupPinDirection(PORTD_ID,PIN3_ID,PIN_OUTPUT);
	GPIO_setupPinDirection(PORTD_ID,PIN4_ID,PIN_OUTPUT);

	GPIO_writePin(PORTD_ID,PIN4_ID,LOGIC_HIGH);
}
/*
 * function description:
 * 1-function returns void
 * 2-function configures motor according to desired purpose
 * 3-giving the function direction of rotation and speed
 * function returns void
 * function calls PWM timer function to configure PWM according to motor configuration
 */
void DcMotor_Rotate(DcMotor_state state,uint8 speed){
	PORTD = ((PORTD&0xF3)| (state<<2));
	uint8 duty_cycle =(((float)speed*255)/(100));
	if(speed==0){
			duty_cycle=-1;
		}
    PWM_Timer0_Start(duty_cycle);
}
