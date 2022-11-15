/*
 * buzzer.c
 * MODULE: Buzzer
 *  Created on: Oct 26, 2022
 *      Author: Philopater Ghobrial
 */
#include"buzzer.h"
#include"gpio.h"
#include "timer.h"
#include"common_macros.h"

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*
 * function description:
 * initialize buzzer module and initialize pin direction
 */
void Buzzer_init(void){
	GPIO_setupPinDirection(BUZZER_PORT_ID,BUZZER_PIN_ID,PIN_OUTPUT);
	GPIO_writePin(BUZZER_PORT_ID,BUZZER_PIN_ID,LOGIC_LOW);
}

/*
 * function description:
 * turn on buzzer by putting 1 on it
 */
void Buzzer_on(void){
	GPIO_writePin(BUZZER_PORT_ID,BUZZER_PIN_ID,LOGIC_HIGH);
}

/*
 * function description:
 * turn off buzzer by puting l=0 on it
 */
void Buzzer_off(void){
	GPIO_writePin(BUZZER_PORT_ID,BUZZER_PIN_ID,LOGIC_LOW);
}
