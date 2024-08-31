/*****************************************************************************************
 ****************** @file           : MEM_Handler.c		            	      ********************
 ****************** @author         : FOTA  Team                      ********************
 ****************** @brief          : interface file DC Motor         ********************
******************************************************************************************/

#include "MEM_Handler.h"


void User_APP_Jump_Bootloader(void){
	uint32_t MSP_Address =  *((volatile uint32_t*)FLASH_MEM_SEC0_BASE_ADDRESS);// first 4 bytes of the flash memory should contains the MSP value
	uint32_t RH_Address = *((volatile uint32_t*)(FLASH_MEM_SEC0_BASE_ADDRESS+4));
	void (*Reset_Handler)(void) =  (void*)RH_Address;
	// from CMSIS drivers --> setting the MSP_Address value to the MSP spicial register 
	__set_MSP(MSP_Address);
	
	// Deinint for modules
	HAL_RCC_DeInit();
	// jumping to the Application reset handler function 
	Reset_Handler();
}