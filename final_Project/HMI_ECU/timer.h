/*
 * timer.h
 * Module: Timer
 *  Created on: Oct 6, 2022
 *      Author: philopater ghobrial
 */

#ifndef TIMER_H_
#define TIMER_H_

#include "std_types.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
#define timer_initial_value   0

/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/

typedef enum{
	normal,CTC
}Timer1_mode;

typedef enum{
	noclock,div1,div8,div64,div256,div1024,ext_clk_falling,ext_clk_rising
}Timer1_Prescaler;

typedef struct{
	uint16 initial_value;
	uint16 compare_value; // it will be used in compare mode only.
	Timer1_Prescaler prescaler;
	Timer1_mode mode;
}Timer1_ConfigType;


/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
/* function description:
 * initialize timer to work in PWM mode and give commands to DC motor
 */
void Timer1_setCallBack(void(*a_ptr)(void));
void PWM_Timer0_Start(uint8 duty_cycle);
void Timer1_init(const Timer1_ConfigType (* Config_Ptr));
//ISR(TIMER1_COMPA_vect);

#endif /* TIMER_H_ */
