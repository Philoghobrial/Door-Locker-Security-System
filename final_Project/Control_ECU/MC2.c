/*
 * MC2.c
 * APP: Door security lock system.
 *  Created on: Oct 26, 2022
 *      Author: philopater ghobrial
 */

#include "uart.h"
#include "buzzer.h"
#include "motor.h"
#include "external_eeprom.h"
#include "twi.h"
#include "timer.h"
#include <util/delay.h> /* For the delay functions */
#include <avr/io.h>

/*******************************************************************************
 *                      Definitions                                  *
 *******************************************************************************/
#define Hold_Door  'a'
#define Unlock_Door  'b'
#define Lock_Door  'c'
#define First_Byte_Pass_Address  0x01
#define Confirm_Msg   'e'
#define Invalid_Pass  'f'
#define Error_Alarm   'g'
#define MC2_ready    'h'
#define Open_Door_Protocol  'i'
#define Change_Pass_Protocol  'j'
#define New_Password 'k'
#define Password_Match 'm'

/*******************************************************************************
 *                      Global Variables                                 *
 *******************************************************************************/

static uint8 g_Tick=0;
uint8 safety_Count=0; //variable to check how many wrong times user entered password
uint8 recieved_Pass[7]; //string used to recieve first string from user
uint8 confirm_Pass[7]; //string used to recieve second string from user
uint8 confirm_Variable=0; //variable checks whether passwords match or not
/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

static void Seconds_Count(void){ //function used to increase global variable every second
	g_Tick++;

}
/*
 * function description:
 * stay in loop until input time equal to global variable Tick
 */
static void Timer_Count(uint8 time){
	g_Tick=0;
	while(g_Tick!=time);

}


void door_Process(void){
	DcMotor_Rotate(clockwise,100); //open door for 15 seconds
	Timer_Count(15);
	DcMotor_Rotate(stop,0);//hold door open for 3 seconds
	Timer_Count(3);
	DcMotor_Rotate(anticlockwise,100); //close door for 15 seconds
	Timer_Count(15);
	DcMotor_Rotate(stop,0);
}

int main(void){

	SREG |= (1<<7); //enable interrupts in system
	Timer1_ConfigType timer_config={0,7813,div1024,CTC};
	Timer1_init(&timer_config); /* Initialize the TIMER module */
	Timer1_setCallBack(Seconds_Count);
	DcMotor_init(); /* Initialize the MOTOR module */
	TWI_ConfigType TWI_Config={0x01,f400k};
	TWI_init(&TWI_Config);/* Initialize the TWI/I2C module */
	UART_ConfigType Uart_Config={9700,One_Bit,Disable,Eight_Bit};
	UART_init(&Uart_Config); /* Initialize the UART module */
	Buzzer_init();/* Initialize the BUZZER module */
	uint8 key;
	UART_sendByte(MC2_ready); //byte sent to give permission to start process as MC2 is ready

	while(1){
		key=UART_recieveByte(); //recieve what user wants to do
		switch(key){
		/*
		 * in this case user enters two identical passwords to save in the memory
		 */
		case New_Password:
			confirm_Variable=0;
			UART_receiveString(recieved_Pass); //recieve first string user entered
			UART_sendByte(Confirm_Msg);//process done notification
			UART_receiveString(confirm_Pass); //recieve second string user entered
			for(uint8 i=0;i<6;i++){
				if(recieved_Pass[i]!=confirm_Pass[i]){ //check if the two passwords entered by user are identical
					confirm_Variable=1;
				}
			}
			if(confirm_Variable==0){ //if passwords entered are identical following process is done
				for(uint8 i=0;i<6;i++){
					EEPROM_writeByte(First_Byte_Pass_Address+i,confirm_Pass[i]); //save password to memory
					_delay_ms(20); //done every read or write operation
				}
				UART_sendByte(Confirm_Msg); //process done notification
				UART_sendByte(Password_Match);  //passwords are identical notification

			}
			else if(confirm_Variable==1){
				UART_sendByte(Confirm_Msg);//process done notification
				UART_sendByte(Invalid_Pass);//user entered two different passwords

			}

			key=0;
			break;

		case Open_Door_Protocol:
			UART_receiveString(recieved_Pass);//recieve first string user entered
			for(uint8 i=0;i<6;i++){
				EEPROM_readByte(First_Byte_Pass_Address+i,&confirm_Pass[i]);//read password from memory to local variable
				_delay_ms(20); //done every read or write operation
			}
			for(uint8 i=0;i<6;i++){
				if(recieved_Pass[i]!=confirm_Pass[i])//check if the password entered by user is identical to saved password
				{
					confirm_Variable=1;
				}
			}
			if(confirm_Variable==0){
				UART_sendByte(Confirm_Msg);//process done notification
				UART_sendByte(Open_Door_Protocol); //protocol start notification
				door_Process();
				safety_Count=0;
			}

			else if(confirm_Variable==1){
				UART_sendByte(Confirm_Msg);//process done notification
				UART_sendByte(Invalid_Pass); //wrong password notification
				safety_Count++;
				if(safety_Count==3){//user entered wrong password three consecutive times
					Buzzer_on(); //open alarm for 1 minute
					Timer_Count(60);
					Buzzer_off();
					safety_Count=0; //reset safety checking variable
				}

			}
			confirm_Variable=0;
			key=0;
			break;
			/*
			 * case used to check for password if right user can change the system pasword like in first case
			 */
		case Change_Pass_Protocol:
			UART_receiveString(recieved_Pass);// recieve first string user entered
			for(uint8 i=0;i<6;i++){
				EEPROM_readByte(First_Byte_Pass_Address+i,&confirm_Pass[i]); //read password from memory to local variable
				_delay_ms(20); //done every read or write operation
			}
			for(uint8 i=0;i<6;i++){
				if(recieved_Pass[i]!=confirm_Pass[i])//check if the password entered by user is identical to saved password
				{
					confirm_Variable=1;
				}
			}
			if(confirm_Variable==0){//password is identical to system password
				UART_sendByte(Confirm_Msg);//process done notification
				UART_sendByte(Password_Match); //password matches system password
				safety_Count=0;
			}

			else if(confirm_Variable==1){
				UART_sendByte(Confirm_Msg);//process done notification
				UART_sendByte(Invalid_Pass);//wrong password notification
				safety_Count++;
				if(safety_Count==3){//user entered wrong password three consecutive times
					Buzzer_on(); //open alarm for 1 minute
					Timer_Count(60);
					Buzzer_off();
					safety_Count=0; //reset safety checking variable
				}

			}
			confirm_Variable=0;
			key=0;



			break;
		}
	}
}
