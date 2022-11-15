/*
 * motor.h
 * module: Motor
 *  Created on: Oct 5, 2022
 *      Author: philopater ghobrial
 */

#ifndef MOTOR_H_
#define MOTOR_H_

#include "std_types.h"

/*******************************************************************************
 *                           Global functions                                  *
 *******************************************************************************/

extern void PWM_Timer0_Start(uint8 duty_cycle);


/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/

typedef enum
{
	stop,clockwise,anticlockwise
}DcMotor_state;



/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * function description
 * initialize motor to work correctly
 */
void DcMotor_init(void);

/*
 * function description
 * choose the rotation direction and speed of rotation according to upcoming inputs
 */

void DcMotor_Rotate(DcMotor_state state,uint8 speed);

#endif /* MOTOR_H_ */
