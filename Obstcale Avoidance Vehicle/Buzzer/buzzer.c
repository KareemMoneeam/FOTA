/*****************************************************************************************
 ****************** @file           : buzzer.c				      	  ********************
 ****************** @author         : FOTA Team                       ********************
 ****************** @brief          : interface file buzzer Driver	  ********************
******************************************************************************************/

/******************************          Includes           ******************************/
#include "buzzer.h"
#include "stddef.h"

void buzzer_On()
{
		HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_SET);
}

void buzzer_Off()
{

	  HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_SET);
}

