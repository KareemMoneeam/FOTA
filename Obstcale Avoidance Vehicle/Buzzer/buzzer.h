/*****************************************************************************************
 ****************** @file           : buzzer.h				      	  ********************
 ****************** @author         : FOTA Team                       ********************
 ****************** @brief          : interface file buzzer Driver	  ********************
******************************************************************************************/

#ifndef BUZZER_BUZZER_H_
#define BUZZER_BUZZER_H_

/******************************          Includes           ******************************/
#include "stm32f4xx_hal.h"
#include "gpio.h"
/******************************   Data Types Decelerations  ******************************/
//typedef struct
//{
//	GPIO_TypeDef*     Buzzer_Port;
//	uint16_t		  Buzzer_Pin;
//} buzzer_t;

/******************************      Software intefaces      ******************************/
void buzzer_On();
void buzzer_Off();

#endif /* BUZZER_BUZZER_H_ */

