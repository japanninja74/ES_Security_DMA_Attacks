/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           funct_joystick.h
** Last modified Date:  2018-12-30
** Last Version:        V1.00
** Descriptions:        High level joystick management functions
** Correlated files:    lib_joystick.c, funct_joystick.c
**--------------------------------------------------------------------------------------------------------       
*********************************************************************************************************/


#ifndef lpc17xx_H 
	#define lpc17xx_H 
	#include "lpc17xx.h"
#endif

#ifndef JOYSTICK_H 
	#define JOYSTICK_H 
	#include "joystick.h"	
#endif

#ifndef LED_H 
	#define LED_H 
	#include "../led/led.h"
#endif


/*----------------------------------------------------------------------------
  Function that turns on requested led
 *----------------------------------------------------------------------------*/
void joystick_On(unsigned int num) {
	//LED_Out(num);
}
