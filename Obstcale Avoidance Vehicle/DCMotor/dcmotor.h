/*****************************************************************************************
 ****************** @file           : dcmotor.h	            	      ********************
 ****************** @author         : FOTA  Team                      ********************
 ****************** @brief          : interface file DC Motor         ********************
******************************************************************************************/

#ifndef DCMOTOR_DCMOTOR_H_
#define DCMOTOR_DCMOTOR_H_

/******************************          Includes           ******************************/
#include "stm32f4xx_hal.h"
#include "gpio.h"
/******************************   Data Types Decelerations  ******************************/

/******************************      Software intefaces      ******************************/
void Car_Move_Forward(void);
void Car_Move_Backward(void);
void Car_Move_Right(void);
void Car_Move_Left(void);
void Car_Stop(void);

#endif /* DCMOTOR_DCMOTOR_H_ */
