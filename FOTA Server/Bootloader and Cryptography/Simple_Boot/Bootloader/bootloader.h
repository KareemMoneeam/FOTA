#ifndef _BOOTLOADER_H_
#define _BOOTLOADER_H_

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "usart.h"
#include "crc.h"
#include "AES_DEC.h"
#include "S_CRC.h"  // made from scratch 
/*  defines ------------------------------------------------------------*/
#define BL_DEBUGER	              &huart6
#define BL_HOST_COMM_UART	        &huart2
#define BL_ENABLE_DEBUGER_USART		0x01
#define CRC_ENGINE_OBJ            &hcrc
/*debug feature */
#define DEBUG_INFO_DISABLE           0
#define DEBUG_INFO_ENABLE            1
#define BL_DEBUG_ENABLE              DEBUG_INFO_ENABLE
/*BL_Version info*/
#define CBL_VENDOR_ID                100
#define CBL_SW_MAJOR_VERSION         1
#define CBL_SW_MINOR_VERSION         1
#define CBL_SW_PATCH_VERSION         0
/* CRC_VERIFICATION */
#define CRC_TYPE_SIZE_BYTE           4 
#define CRC_VERIFICATION_FAILED      0x00
#define CRC_VERIFICATION_PASSED      0x01
#define CBL_SEND_NACK                0xAB
#define CBL_SEND_ACK                 0xCD
// HOST Commands
#define CBL_GET_VER_CMD              0x10
#define CBL_GET_HELP_CMD             0x11
#define CBL_GET_CID_CMD              0x12
/* Get Read Protection Status */
#define CBL_GET_RDP_STATUS_CMD       0x13
#define CBL_GO_TO_ADDR_CMD           0x14
#define CBL_FLASH_ERASE_CMD          0x15
#define CBL_MEM_WRITE_CMD            0x16
/* Enable/Disable Write Protection */
#define CBL_ED_W_PROTECT_CMD         0x17
#define CBL_MEM_READ_CMD             0x18
/* Get Sector Read/Write Protection Status */
#define CBL_READ_SECTOR_STATUS_CMD   0x19
#define CBL_OTP_READ_CMD             0x20
/* Change Read Out Protection Level */
#define CBL_CHANGE_ROP_Level_CMD     0x21


#define FLASH_MEM_SEC2_BASE_ADDRESS  0x8008000U


#define ADDRESS_VALID								0x01
#define ADDRESS_NOT_VALID						0x00

 /*!< FLASH(up to 1 MB) base address in the alias region */
 /*!< SRAM1(64 KB) base address in the alias region*/
#define FLASH_SIZE						(1024 *  1024)
#define SRAM1_SIZE						(64   *  1024)	
#define FLASH_END_ADDRESS			(FLASH_BASE+FLASH_SIZE)
#define SRAM1_END_ADDRESS			(SRAM1_BASE+SRAM1_SIZE)

#define HAL_SUCCESSFUL_ERASE         0xFFFFFFFFU
#define INVALID_SECTOR_NUMBER        0x00
#define VALID_SECTOR_NUMBER          0x01
#define UNSUCCESSFUL_ERASE           0x02
#define SUCCESSFUL_ERASE             0x03
#define MASS_ERASE								   0xFF
#define CCU6_FLASH_MAX_SECTORS			 8


/* CBL_MEM_WRITE_CMD */
#define FLASH_PAYLOAD_WRITE_FAILED   0x00
#define FLASH_PAYLOAD_WRITE_PASSED   0x01

#define FLASH_LOCK_WRITE_FAILED      0x00
#define FLASH_LOCK_WRITE_PASSED      0x01

#define ROP_LEVEL_CHANGE_INVALID     0x00
#define ROP_LEVEL_CHANGE_VALID       0X01

/* CBL_CHANGE_ROP_Level_CMD */
#define CBL_ROP_LEVEL_0              0x00
#define CBL_ROP_LEVEL_1              0x01
#define CBL_ROP_LEVEL_2              0x02
/*  DataTypes Declration---------------------------------------------------------*/
typedef enum {
	BL_NOK= 0,
	BL_OK
}BL_status;

typedef void(*Jump_PTR)(void) ;
/*  function prototypes -----------------------------------------------*/
BL_status BL_Print_Message(char*format,...);
BL_status BL_UART_Featch_Host_Command(void);
void try_jump(void);

#endif