/*********************************************************************************************
 ****************** @file           : ultrasonic_cfg.c	            	    ********************
 ****************** @author         : FOTA  Team                          ********************
 ****************** @brief          : configuration Ultrasonic Driver     ********************
**********************************************************************************************/
#include "ultrasonic.h"

const ultrasonic_t ultrasonic =
{
		.Ultrasonic_Port = GPIOB,
		.Ultrasonic_Echo_Pin = GPIO_PIN_6,
		.Ultrasonic_Trigger_Pin = GPIO_PIN_7,
};
