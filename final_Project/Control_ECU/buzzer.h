/*
 * buzzer.h
 * MODULE: Buzzer
 *  Created on: Oct 26, 2022
 *      Author: Philopater Ghobrial
 */

#ifndef BUZZER_H_
#define BUZZER_H_

#include "std_types.h"


/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
#define BUZZER_PORT_ID    PORTB_ID
#define BUZZER_PIN_ID    PIN4_ID

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
/*
 * function description:
 * initialize buzzer module and initialize pin direction
 */
void Buzzer_init(void);

/*
 * function description:
 * turn on buzzer by putting 1 on it
 */
void Buzzer_on(void);

/*
 * function description:
 * turn off buzzer by puting l=0 on it
 */
void Buzzer_off(void);

#endif /* BUZZER_H_ */
