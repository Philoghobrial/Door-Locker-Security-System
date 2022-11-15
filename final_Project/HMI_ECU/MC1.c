/*
 * MC1.c
 * APP: Door security lock system
 *  Human interface MC
 *  Created on: Oct 26, 2022
 *      Author: philopater ghobrial
 */
#include "lcd.h"
#include "keypad.h"
#include "uart.h"
#include "timer.h"
#include "std_types.h"
#include <util/delay.h> /* For the delay functions */
#include <avr/io.h>

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/

#define Hold_Door  'a'
#define Unlock_Door  'b'
#define Lock_Door  'c'
#define Confirm_Msg   'e'
#define Invalid_Pass  'f'
#define Error_Alarm   'g'
#define MC2_ready    'h'
#define Open_Door_Protocol  'i'
#define Change_Pass_Protocol  'j'
#define New_Password 'k'
#define Password_Match 'm'

/*******************************************************************************
 *                      Global Variables                               *
 *******************************************************************************/

static uint8 g_Tick=0;
uint8 send_Pass[7]; //string used to save password given by user and sent to MC2
uint8 confirm_Pass[7];//string used to save password given by user and sent to MC2
uint8 safety_Count=0; //used to count how many times user entered wrong password
uint8 access_Times=0;
/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
static void Seconds_Count(void){ //used to increase global variable every one second
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

int main(void){

	SREG |= (1<<7);
	Timer1_setCallBack(Seconds_Count);
	Timer1_ConfigType timer_config={0,7813,div1024,CTC};
	Timer1_init(&timer_config);/* Initialize the TIMER module */
	UART_ConfigType Uart_Config={9700,One_Bit,Disable,Eight_Bit};
	UART_init(&Uart_Config);/* Initialize the UART module */
	LCD_init();/* Initialize the LCD module */
	while(UART_recieveByte()!=MC2_ready); //wait until MC2 is ready to start process

	while(1){

		while(access_Times==0){ //while first use for application or change password case
			UART_sendByte(New_Password); //notification about upcoming process to other MC
			LCD_displayString("plz enter pass:");
			LCD_moveCursor(1,0); //messege displayed for human interface
			uint8 temp=0;
			uint8 i=0;
			uint8 key=0;
			for(i=0;i<5;i++){
				if((KEYPAD_getPressedKey()>=0)&&(KEYPAD_getPressedKey()<=9)){// to make sure user pushes a number
					send_Pass[i]=KEYPAD_getPressedKey(); //save input from user in first string
					_delay_ms(500);//to make sure there is no re-bouncing
					LCD_displayString("*"); //for safety print '*'
				}
				else{
					i--;
				}
			}
			while(KEYPAD_getPressedKey()!='=');// to submit password
			LCD_clearScreen();
			LCD_displayString("    loading"); //to display that system is running
			_delay_ms(500);//to make sure there is no re-bouncing
			LCD_clearScreen(); //clear screen from previous writings
			send_Pass[i]='#'; // character chosen to be the last in any Uart send string
			LCD_displayString("plz re-enter the");
			LCD_moveCursor(1,0); //user display to know what action to do
			LCD_displayString("same pass:");
			LCD_moveCursor(1,11);
			for(i=0;i<5;i++){
				if((KEYPAD_getPressedKey()>=0)&&(KEYPAD_getPressedKey()<=9)){// to make sure user pushes a number
					confirm_Pass[i]=KEYPAD_getPressedKey();//save input from user in second string
					_delay_ms(500);//to make sure there is no re-bouncing
					LCD_displayString("*");
				}
				else{
					i--;
				}
			}
			while(KEYPAD_getPressedKey()!='=');// to submit password
			LCD_clearScreen();
			LCD_displayString("    loading"); //to display that system is running
			_delay_ms(500); //to make sure there is no re-bouncing
			confirm_Pass[i]='#';// character chosen to be the last in any Uart send string
			UART_sendString(send_Pass);//send first string to control MC
			while(UART_recieveByte()!=Confirm_Msg); //wait until confirmation notification
			UART_sendString(confirm_Pass); //send second string to control MC
			key=0;
			temp=0;
			while(temp==0){
				while(UART_recieveByte()!=Confirm_Msg); //waiting confirmation from MC2 to continue
				key=UART_recieveByte();
				if(key==Password_Match){//passwords are identical notification
					LCD_clearScreen();
					LCD_displayString(" password saved");
					LCD_moveCursor(1,0); //user display on system status
					LCD_displayString("  successfully ");
					_delay_ms(1000);
					LCD_clearScreen(); //clear screen after finished writing
					access_Times=1; //to get out of while loop and go to main options loop
					temp=1; //to get out of current loop
				}
				else if(key==Invalid_Pass){ //passwords are invalid notification
					LCD_clearScreen();
					LCD_displayString("password entered");
					LCD_moveCursor(1,0);//user display on system status
					LCD_displayString(" is incorrect ");
					_delay_ms(1000); //to give time for user to read system status
					LCD_clearScreen();
					LCD_displayString("please enter the");
					LCD_moveCursor(1,0);//user display on system status
					LCD_displayString("passwords again ");
					_delay_ms(1000);//to give time for user to read system status
					LCD_clearScreen();
					temp=1; // to exit loop
				}
			}

		}
		while(access_Times==1){ //as system has a saved password we exit first loop
			uint8 key=0;
			uint8 temp=0;
			uint8 send_Pass[7]; //string used to take user input password and send it to MC2 to compare with saved password
			LCD_clearScreen();
			LCD_displayString(" + : Open door ");
			LCD_moveCursor(1,0);//user display on system options to choose from
			LCD_displayString(" - : Change pass");
			while(KEYPAD_getPressedKey()==KEYPAD_BUTTON_RELEASED);//wait while user pushes on keypad
			key=KEYPAD_getPressedKey();
			LCD_clearScreen();
			LCD_displayString("    loading");
			_delay_ms(500);//to make sure there is no re-bouncing
			LCD_clearScreen();
			switch(key){
			/*
			 * case open door:
			 * once choosen user inputs password and system sends it to MC2 to compare it with the saved one in memory
			 * if they are matched system opens door and status of door is displayed on LCD
			 * if not user is given another chance unless for 3 consecutive times user enters wrong password
			 * system will go in alert mode for 1 min buzzer will turn on and no inputs will be taken
			 *
			 */
			case'+':
				UART_sendByte(Open_Door_Protocol); //get MC2 to work on open door process
				LCD_displayString("plz enter pass:");
				LCD_moveCursor(1,0);
				uint8 i=0;
				for(i=0;i<5;i++){
					if((KEYPAD_getPressedKey()>=0)&&(KEYPAD_getPressedKey()<=9)){// to make sure user pushes a number
						send_Pass[i]=KEYPAD_getPressedKey(); //take varibles from keypadinside the string
						_delay_ms(500);//to make sure there is no re-bouncing
						LCD_displayString("*");
					}
					else{
						i--;
					}
				}
				while(KEYPAD_getPressedKey()!='='); // to submit user answer
				send_Pass[i]='#';// character chosen to be the last in any Uart send string
				LCD_clearScreen();
				LCD_displayString("    loading");
				_delay_ms(500);//to make sure there is no re-bouncing
				LCD_clearScreen();
				UART_sendString(send_Pass); //send user's inputed password to MC2
				i=0;
				temp=0;
				while(temp==0){
					while(UART_recieveByte()!=Confirm_Msg);
					key=UART_recieveByte();
					if(key==Open_Door_Protocol){ //affirmation that passwords are matched
						LCD_clearScreen();
						LCD_displayString(" Unlocking Door");//display current status as door is opening
						Timer_Count(15); //time until next instruction is executed
						LCD_clearScreen();
						LCD_displayString("  Door is Open");//display current status as door is open
						Timer_Count(3);//time until next instruction is executed
						LCD_clearScreen();
						LCD_displayString("  Locking Door");//display current status as door is closing
						Timer_Count(15);//time until next instruction is executed
						LCD_clearScreen();
						safety_Count=0; //reset count as user entered right password
						temp=1; //to get out of loop
					}
					if(key==Invalid_Pass){ //notification that password entered is invalid
						LCD_clearScreen();
						LCD_displayString("password entered");
						LCD_moveCursor(1,0); //inform user that password is invalid
						LCD_displayString(" is incorrect ");
						_delay_ms(1000); //to give time user to see status
						LCD_clearScreen();
						safety_Count++;
						if(safety_Count==3){ //if user entered wrong password 3 consecutive times
							LCD_clearScreen();
							LCD_displayString(" ERROR NOINPUTS ");
							LCD_moveCursor(1,0);
							LCD_displayString("INVALID PASSWORD ");
							Timer_Count(60); //system is out of service for 1 minute
							LCD_clearScreen();
							safety_Count=0; //reset timer
						}
						else{
							LCD_clearScreen();
							LCD_displayString("please enter the");
							LCD_moveCursor(1,0); //inform user of wrong password
							LCD_displayString("password again ");
							_delay_ms(1000);
						}
						LCD_clearScreen();
						temp=1; //to exit loop

					}
				}
				key=0;
				temp=0;
				break;
			case '-':
				UART_sendByte(Change_Pass_Protocol); //to inform MC2 of upcoming process
				LCD_displayString("plz enter pass:");
				LCD_moveCursor(1,0);
				i=0;
				for(i=0;i<5;i++){
					if((KEYPAD_getPressedKey()>=0)&&(KEYPAD_getPressedKey()<=9)){// to make sure user pushes a number
						send_Pass[i]=KEYPAD_getPressedKey();// send user inputed password
						_delay_ms(500);
						LCD_displayString("*");
					}
					else{
						i--;
					}
				}
				while(KEYPAD_getPressedKey()!='='); //to submit password user entered
				send_Pass[i]='#';// character chosen to be the last in any Uart send string
				LCD_clearScreen();
				LCD_displayString("    loading"); //load system status
				_delay_ms(500);
				LCD_clearScreen();
				UART_sendString(send_Pass); //send string to MC2
				i=0;
				temp=0;
				while(temp==0){
					while(UART_recieveByte()!=Confirm_Msg);
					key=UART_recieveByte();
					if(key==Password_Match){ //password matches password in eeprom notification
						LCD_clearScreen();
						LCD_displayString("password entered");
						LCD_moveCursor(1,0); //user display on system status
						LCD_displayString("  successfully ");
						_delay_ms(1000);
						LCD_clearScreen();
						access_Times=0;//return to change password protocol
						temp=1;
					}
					if(key==Invalid_Pass){ //password doesn't matches password in eeprom notification
						LCD_clearScreen();
						LCD_displayString("password entered");
						LCD_moveCursor(1,0);
						LCD_displayString(" is incorrect ");
						_delay_ms(1000);
						LCD_clearScreen();
						safety_Count++;//everytime user enters wrong password safety count increments
						if(safety_Count==3){//this case for 1 min system is out of service as inruder alert
							LCD_clearScreen();
							LCD_displayString(" ERROR NOINPUTS ");
							LCD_moveCursor(1,0);
							LCD_displayString("INVALID PASSWORD ");
							Timer_Count(60);
							LCD_clearScreen();
							safety_Count=0;//reset counter from 0
						}
						else{
							LCD_clearScreen();
							LCD_displayString("please enter the");
							LCD_moveCursor(1,0);
							LCD_displayString("password again ");
							_delay_ms(1000);
						}
						LCD_clearScreen();
						temp=1;

					}
				}
				key=0;
				temp=0;
				break;
			}
		}
	}
}
