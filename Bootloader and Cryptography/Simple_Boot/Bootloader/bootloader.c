
/* Includes ------------------------------------------------------------------*/
#include "bootloader.h"

/* Private define ------------------------------------------------------------*/
#define BL_HOST_BUFFER_RX_LENGHT 200

/* DataTypes definition | Declration ---------------------------------------------------------*/
static uint8_t BL_Host_Buffer[BL_HOST_BUFFER_RX_LENGHT] ;
static uint8_t BL_commands[12] ={
	CBL_GET_VER_CMD,
	CBL_GET_HELP_CMD,
	CBL_GET_CID_CMD,
	CBL_GET_RDP_STATUS_CMD,
	CBL_GO_TO_ADDR_CMD,
	CBL_FLASH_ERASE_CMD,
	CBL_MEM_WRITE_CMD,
	CBL_ED_W_PROTECT_CMD,
	CBL_MEM_READ_CMD,
	CBL_READ_SECTOR_STATUS_CMD,
	CBL_OTP_READ_CMD,
	CBL_CHANGE_ROP_Level_CMD
};

uint8_t success[1] = {0x01};
/*static functions declration ----------------------------------------------------*/
/*Helpers*/
BL_status BL_Print_Message(char*format,...);
static uint8_t Bootloader_CRC_Verify(uint8_t *pData, uint32_t Data_Len, uint32_t Host_CRC);
static void Bootloader_Send_ACK(uint8_t Replay_Len);
static void Bootloader_Send_NACK(void);
static void Bootloader_Send_Data_To_Host(uint8_t *Host_Buffer, uint32_t Data_Len);
static uint8_t Host_Address_Verification(uint32_t Jump_Address);
static uint8_t Perform_Flash_Erase(uint8_t Sector_Numebr, uint8_t Number_Of_Sectors);
static uint8_t Flash_Memory_Write_Payload(uint8_t *Host_Payload, uint32_t Payload_Start_Address, uint16_t Payload_Len);
static uint8_t Test_Flash_Memory_Write_Payload(uint8_t *Host_Payload, uint32_t Payload_Start_Address, uint16_t Payload_Len);
static uint8_t CBL_STM32F407_Get_RDP_Level();
static uint8_t Change_ROP_Level(uint32_t ROP_Level);

/*Commands*/
static void Bootloader_Get_Version(uint8_t *Host_Buffer);
static void Bootloader_Get_Help(uint8_t *Host_Buffer);
static void Bootloader_Get_Chip_Identification_Number(uint8_t *Host_Buffer);
static void Bootloader_Jump_User_APP(void);
static void Bootloader_Jump_To_Address(uint8_t *Host_Buffer);
static void Bootloader_Erase_Flash(uint8_t *Host_Buffer);
static void Bootloader_Memory_Write(uint8_t *Host_Buffer);
static void Test_Bootloader_Memory_Write(uint8_t *Host_Buffer);
static void Bootloader_Read_Protection_Level(uint8_t *Host_Buffer);
static void Bootloader_Change_Read_Protection_Level(uint8_t *Host_Buffer);
static void Test_Bootloader_Erase_Flash(uint8_t *Host_Buffer);
/*
static void Bootloader_Enable_RW_Protection(uint8_t *Host_Buffer);
static void Bootloader_Memory_Read(uint8_t *Host_Buffer);
static void Bootloader_Get_Sector_Protection_Status(uint8_t *Host_Buffer);
static void Bootloader_Read_OTP(uint8_t *Host_Buffer);


*/

/*  functions Definitions -----------------------------------------------*/
/*Helpers definitions*/
BL_status BL_Print_Message(char*format,...){
	BL_status status = BL_NOK;
	char Message[100] = {0};
	va_list args;
	va_start(args,format);
	vsprintf(Message,format,args);
	#if (BL_ENABLE_DEBUGER_USART == 0x01) 
	 HAL_UART_Transmit(BL_DEBUGER,(uint8_t*)Message,sizeof(Message),HAL_MAX_DELAY);
	 status = BL_OK;
	#endif
	va_end(args);
	return status;
}
static uint8_t Bootloader_CRC_Verify(uint8_t *pData, uint32_t Data_Len, uint32_t Host_CRC){
	uint8_t CRC_Status = CRC_VERIFICATION_FAILED;
	uint32_t MCU_CRC_Calculated = 0;
	uint8_t Data_Counter = 0;
	uint32_t Data_Buffer = 0;
	
	/* Calculate CRC32 */
	/* can be done as:
	 MCU_CRC_Calculated = HAL_CRC_Accumulate(CRC_ENGINE_OBJ,(uint32_t*)pData, Data_Len); 
	*/

		//------ CMSIS method 
		for(Data_Counter = 0; Data_Counter < Data_Len; Data_Counter++){
		Data_Buffer = (uint32_t)pData[Data_Counter];
		MCU_CRC_Calculated = HAL_CRC_Accumulate(CRC_ENGINE_OBJ, &Data_Buffer, 1);
		}
	// Reset the CRC Calculation Unit 
  __HAL_CRC_DR_RESET(CRC_ENGINE_OBJ);

	//MCU_CRC_Calculated = calculateCRC32((uint8_t *)pData,Data_Len);
	/* Compare the Host CRC and Calculated CRC */
	if(MCU_CRC_Calculated == Host_CRC){
		CRC_Status = CRC_VERIFICATION_PASSED;
		#if BL_DEBUG_ENABLE  ==    DEBUG_INFO_ENABLE
		BL_Print_Message("CRC Verification  Passed\r\n ");
		#endif
	}
	else{
		CRC_Status = CRC_VERIFICATION_FAILED;
		#if BL_DEBUG_ENABLE  ==    DEBUG_INFO_ENABLE
		BL_Print_Message("CRC Verification  Failed\r\n ");
		#endif
	}
	
	return CRC_Status;	
}

static void Bootloader_Send_ACK(uint8_t Replay_Len){
	uint8_t Ack_Value[2] = {0};
	Ack_Value[0] = CBL_SEND_ACK;
	Ack_Value[1] = Replay_Len;
	HAL_UART_Transmit(BL_HOST_COMM_UART, (uint8_t *)Ack_Value, 2, HAL_MAX_DELAY);
}

static void Bootloader_Send_NACK(void){
	uint8_t Ack_Value = CBL_SEND_NACK;
	HAL_UART_Transmit(BL_HOST_COMM_UART, &Ack_Value, 1, HAL_MAX_DELAY);
}
static void Bootloader_Send_Data_To_Host(uint8_t *Host_Buffer, uint32_t Data_Len){
	HAL_UART_Transmit(BL_HOST_COMM_UART, Host_Buffer, Data_Len, HAL_MAX_DELAY);
}
static uint8_t Host_Address_Verification(uint32_t Jump_Address){
	uint8_t Address_Verification = ADDRESS_NOT_VALID;
	if((Jump_Address>FLASH_BASE)&& (Jump_Address <= FLASH_END_ADDRESS)){
		Address_Verification = ADDRESS_VALID;
		#if BL_DEBUG_ENABLE  ==    DEBUG_INFO_ENABLE
		BL_Print_Message("Jumping Address 0x%X is valid\r\n ",Jump_Address);
		#endif
	}else if((Jump_Address>SRAM1_BASE)&& (Jump_Address <= SRAM1_END_ADDRESS)){
		Address_Verification = ADDRESS_VALID;
		#if BL_DEBUG_ENABLE  ==    DEBUG_INFO_ENABLE
		BL_Print_Message("Jumping Address 0x%X is valid\r\n ",Jump_Address);
		#endif
	}else{
		Address_Verification = ADDRESS_NOT_VALID;
		#if BL_DEBUG_ENABLE  ==    DEBUG_INFO_ENABLE
		BL_Print_Message("Jumping Address 0x%X is NOT VALID\r\n ",Jump_Address);
		#endif
	}
	return Address_Verification;
}
static uint8_t Perform_Flash_Erase(uint8_t Sector_Numebr, uint8_t Number_Of_Sectors){
	uint8_t Erase_state = UNSUCCESSFUL_ERASE;
	uint8_t remaining_sectors = 0;
	HAL_StatusTypeDef HAL_status = HAL_ERROR;
	FLASH_EraseInitTypeDef pEraseInit;
	uint32_t SectorError = 0;
	
		if(Number_Of_Sectors > CCU6_FLASH_MAX_SECTORS){
			Erase_state = INVALID_SECTOR_NUMBER;
		}else{
			if(Sector_Numebr <= (CCU6_FLASH_MAX_SECTORS-1) ||(MASS_ERASE == Sector_Numebr)){
				if(MASS_ERASE == Sector_Numebr){
					//performing Flash mass erase 
					pEraseInit.TypeErase = FLASH_TYPEERASE_MASSERASE;
					#if (BL_DEBUG_ENABLE == DEBUG_INFO_ENABLE)
					BL_Print_Message("Flash Mass erase activation \r\n");
					#endif
				}else{
					#if (BL_DEBUG_ENABLE == DEBUG_INFO_ENABLE)
					BL_Print_Message("User needs Sector erase \r\n");
					#endif
					remaining_sectors = CCU6_FLASH_MAX_SECTORS - Sector_Numebr;
					(Number_Of_Sectors>remaining_sectors)?Number_Of_Sectors=remaining_sectors:0;
					
					pEraseInit.TypeErase = FLASH_TYPEERASE_SECTORS; /* Sectors erase only */
					pEraseInit.Sector = Sector_Numebr;				/* Initial FLASH sector to erase when Mass erase is disabled */
					pEraseInit.NbSectors = Number_Of_Sectors;		/* Number of sectors to be erased. */	
				}
			pEraseInit.Banks = FLASH_BANK_1;				 /* Bank 1  */
			pEraseInit.VoltageRange = FLASH_VOLTAGE_RANGE_3; /* Device operating range: 2.7V to 3.6V */
				
			HAL_status = HAL_FLASH_Unlock();
			/* Perform a mass erase or erase the specified FLASH memory sectors */
			HAL_status = HAL_FLASHEx_Erase(&pEraseInit, &SectorError);
			Erase_state =(HAL_SUCCESSFUL_ERASE == SectorError)?SUCCESSFUL_ERASE:UNSUCCESSFUL_ERASE;
			HAL_status = HAL_FLASH_Lock();
			}else{
				Erase_state = UNSUCCESSFUL_ERASE;
			}
		}
	
	return Erase_state;
}
static uint8_t Flash_Memory_Write_Payload(uint8_t *Host_Payload, uint32_t Payload_Start_Address, uint16_t Payload_Len){
	HAL_StatusTypeDef HAL_Status = HAL_ERROR;
	uint8_t Flash_Payload_Write_Status = FLASH_PAYLOAD_WRITE_FAILED;
  uint8_t Decryption_status = DEC_NOT_VERIFIED;
	uint8_t* Decrypted_Byte = 0;
	
	/* Unlock the FLASH control register access */
	HAL_Status = HAL_FLASH_Unlock();
	if (HAL_Status != HAL_OK)
	{
		Flash_Payload_Write_Status = FLASH_PAYLOAD_WRITE_FAILED;
	}else{
		// Decrypting buffer befor writing
		Decryption_status = AES_decrypt_buffer(Host_Payload,Payload_Len,Decrypted_Byte);
		for (uint16_t Payload_Counter = 0; Payload_Counter < Payload_Len; Payload_Counter++)
		{	
			if(DEC_VERIFIED== Decryption_status){
			#if BL_DEBUG_ENABLE  ==    DEBUG_INFO_ENABLE
			BL_Print_Message("DEC_VERIFIED \r\n ");
			#endif
			/* Program a byte at a specified address */   //Decrypted_Byte[Payload_Counter]
			HAL_Status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE,Payload_Start_Address + Payload_Counter, Decrypted_Byte[Payload_Counter]);
			HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_RESET); // making sure every thing is okay
			if(HAL_Status != HAL_OK){
				Flash_Payload_Write_Status = FLASH_PAYLOAD_WRITE_FAILED;
				break;
			}else{
				Flash_Payload_Write_Status = FLASH_PAYLOAD_WRITE_PASSED;
			 }
			}else{
				Flash_Payload_Write_Status = FLASH_PAYLOAD_WRITE_FAILED;
				break;
			 }
		}
	}
	if ((FLASH_PAYLOAD_WRITE_PASSED == Flash_Payload_Write_Status) && (HAL_OK == HAL_Status))
	{
		/* Locks the FLASH control register access */
		HAL_Status = HAL_FLASH_Lock();
		if (HAL_Status != HAL_OK)
		{
			Flash_Payload_Write_Status = FLASH_PAYLOAD_WRITE_FAILED;
		}
		else
		{
			Flash_Payload_Write_Status = FLASH_PAYLOAD_WRITE_PASSED;
		}
	}
	else
	{
		Flash_Payload_Write_Status = FLASH_PAYLOAD_WRITE_FAILED;
	}

	
	return Flash_Payload_Write_Status ;
}

static uint8_t Test_Flash_Memory_Write_Payload(uint8_t *Host_Payload, uint32_t Payload_Start_Address, uint16_t Payload_Len)
{
	HAL_StatusTypeDef HAL_Status = HAL_ERROR;
	uint8_t Flash_Payload_Write_Status = FLASH_PAYLOAD_WRITE_FAILED;
	uint16_t Payload_Counter = 0;

	/* Unlock the FLASH control register access */
	HAL_Status = HAL_FLASH_Unlock();

	if (HAL_Status != HAL_OK)
	{
		Flash_Payload_Write_Status = FLASH_PAYLOAD_WRITE_FAILED;
	}
	else
	{
		for (Payload_Counter = 0; Payload_Counter < Payload_Len; Payload_Counter++)
		{
			/* Program a byte at a specified address */
			HAL_Status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, Payload_Start_Address + Payload_Counter, Host_Payload[Payload_Counter]);
			if (HAL_Status != HAL_OK)
			{
				Flash_Payload_Write_Status = FLASH_PAYLOAD_WRITE_FAILED;
				break;
			}
			else
			{
				Flash_Payload_Write_Status = FLASH_PAYLOAD_WRITE_PASSED;
			}
		}
	}

	if ((FLASH_PAYLOAD_WRITE_PASSED == Flash_Payload_Write_Status) && (HAL_OK == HAL_Status))
	{
		/* Locks the FLASH control register access */
		HAL_Status = HAL_FLASH_Lock();
		if (HAL_Status != HAL_OK)
		{
			Flash_Payload_Write_Status = FLASH_PAYLOAD_WRITE_FAILED;
		}
		else
		{
			Flash_Payload_Write_Status = FLASH_PAYLOAD_WRITE_PASSED;
		}
	}
	else
	{
		Flash_Payload_Write_Status = FLASH_PAYLOAD_WRITE_FAILED;
	}

	return Flash_Payload_Write_Status;
}
static uint8_t CBL_STM32F407_Get_RDP_Level(){
	FLASH_OBProgramInitTypeDef FLASH_OBProgram;
	/* Get the Option byte configuration */
	HAL_FLASHEx_OBGetConfig(&FLASH_OBProgram);

	return (uint8_t)(FLASH_OBProgram.RDPLevel);
}
static uint8_t Change_ROP_Level(uint32_t ROP_Level){
	HAL_StatusTypeDef HAL_Status = HAL_ERROR;
	FLASH_OBProgramInitTypeDef FLASH_OBProgramInit;
	uint8_t ROP_Level_Status = ROP_LEVEL_CHANGE_INVALID;

	/* Unlock the FLASH Option Control Registers access */
	HAL_Status = HAL_FLASH_OB_Unlock();
	if (HAL_Status != HAL_OK)
	{
		ROP_Level_Status = ROP_LEVEL_CHANGE_INVALID;
#if (BL_DEBUG_ENABLE == DEBUG_INFO_ENABLE)
		BL_Print_Message("Failed -> Unlock the FLASH Option Control Registers access \r\n");
#endif
	}
	else
	{
#if (BL_DEBUG_ENABLE == DEBUG_INFO_ENABLE)
		BL_Print_Message("Passed -> Unlock the FLASH Option Control Registers access \r\n");
#endif
		FLASH_OBProgramInit.OptionType = OPTIONBYTE_RDP; /* RDP option byte configuration */
		FLASH_OBProgramInit.Banks = FLASH_BANK_1;
		FLASH_OBProgramInit.RDPLevel = ROP_Level;
		/* Program option bytes */
		HAL_Status = HAL_FLASHEx_OBProgram(&FLASH_OBProgramInit);
		if (HAL_Status != HAL_OK)
		{
#if (BL_DEBUG_ENABLE == DEBUG_INFO_ENABLE)
			BL_Print_Message("Failed -> Program option bytes \r\n");
#endif
			HAL_Status = HAL_FLASH_OB_Lock();
			ROP_Level_Status = ROP_LEVEL_CHANGE_INVALID;
		}
		else
		{
#if (BL_DEBUG_ENABLE == DEBUG_INFO_ENABLE)
			BL_Print_Message("Passed -> Program option bytes \r\n");
#endif
			/* Launch the option byte loading */
			HAL_Status = HAL_FLASH_OB_Launch();
			if (HAL_Status != HAL_OK)
			{
				ROP_Level_Status = ROP_LEVEL_CHANGE_INVALID;
			}
			else
			{
				/* Lock the FLASH Option Control Registers access */
				HAL_Status = HAL_FLASH_OB_Lock();
				if (HAL_Status != HAL_OK)
				{
					ROP_Level_Status = ROP_LEVEL_CHANGE_INVALID;
				}
				else
				{
					ROP_Level_Status = ROP_LEVEL_CHANGE_VALID;
#if (BL_DEBUG_ENABLE == DEBUG_INFO_ENABLE)
					BL_Print_Message("Passed -> Program ROP to Level : 0x%X \r\n", ROP_Level);
#endif
				}
			}
		}
	}
	return ROP_Level_Status;
}

/*commands definitions*/
BL_status BL_UART_Featch_Host_Command(void){
	BL_status status = BL_NOK;
	HAL_StatusTypeDef HAL_Status = HAL_ERROR;
	uint8_t Data_lenght = 0;
	// --  --- --- -- --- -- 
	memset(BL_Host_Buffer, 0, BL_HOST_BUFFER_RX_LENGHT);
	HAL_Status = HAL_UART_Receive(BL_HOST_COMM_UART,BL_Host_Buffer,1,HAL_MAX_DELAY);
	 
	if(HAL_Status != HAL_OK){
		status = BL_NOK;
	}
	else{
		Data_lenght  = BL_Host_Buffer[0];
	  // Recieve commamds as lenght as you need from host index 1 ---> BL_HOST_Buffer[0]
	  HAL_Status = HAL_UART_Receive(BL_HOST_COMM_UART,&BL_Host_Buffer[1],Data_lenght,HAL_MAX_DELAY);
		if(HAL_Status != HAL_OK){
			status = BL_NOK;
		}else{
				 /* HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_RESET);  // we reached here
				  Test_Bootloader_Memory_Write(BL_Host_Buffer);*/
				switch(BL_Host_Buffer[1]){
				case CBL_GET_VER_CMD:
					Bootloader_Get_Version(BL_Host_Buffer);
					status = BL_OK;
					break;
				case CBL_GET_HELP_CMD:
					Bootloader_Get_Help(BL_Host_Buffer);
					status = BL_OK;
					break;
				case CBL_GET_CID_CMD:
					Bootloader_Get_Chip_Identification_Number(BL_Host_Buffer);
					status = BL_OK;
					break;
				case CBL_GET_RDP_STATUS_CMD:
					Bootloader_Read_Protection_Level(BL_Host_Buffer);
					status = BL_OK;
					break;
				case CBL_GO_TO_ADDR_CMD:
					//Bootloader_Jump_To_Address(BL_Dynamic_Host_Buffer);
				  Bootloader_Jump_User_APP();
					status = BL_OK;
					break;
				case CBL_FLASH_ERASE_CMD:
					//Test_Bootloader_Erase_Flash(BL_Dynamic_Host_Buffer);
					Bootloader_Erase_Flash(BL_Host_Buffer);
					status = BL_OK;
					break;
				case CBL_MEM_WRITE_CMD:
					//Bootloader_Memory_Write(BL_Host_Buffer);
				  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_RESET);  // we reached here
				  Test_Bootloader_Memory_Write(BL_Host_Buffer);
					status = BL_OK;
					break;
				case CBL_ED_W_PROTECT_CMD:
					BL_Print_Message("Enable or Disable write protect on different sectors of the user flash \r\n");
					status = BL_OK;
					break;
				case CBL_MEM_READ_CMD:
					BL_Print_Message("Read data from different memories of the microcontroller \r\n");
					status = BL_OK;
					break;
				case CBL_READ_SECTOR_STATUS_CMD:
					BL_Print_Message("Read all the sector protection status \r\n");
					status = BL_OK;
					break;
				case CBL_OTP_READ_CMD:
					BL_Print_Message("Read the OTP contents \r\n");
					status = BL_OK;
					break;
				case CBL_CHANGE_ROP_Level_CMD:
					Bootloader_Change_Read_Protection_Level(BL_Host_Buffer);
					status = BL_OK;
					break;
				case 0xFF:
					Bootloader_Send_Data_To_Host((uint8_t *)&success, 1);
					break;
				default:
					BL_Print_Message("Invalid command code received from host !! \r\n");
					break;
			}
		}
		
	}
	
	return status;
}

void try_jump(void){
Bootloader_Jump_User_APP();
}

static void Bootloader_Get_Version(uint8_t *Host_Buffer){

	uint8_t BL_version[4] = {CBL_VENDOR_ID,CBL_SW_MAJOR_VERSION,CBL_SW_MINOR_VERSION,CBL_SW_PATCH_VERSION};
	uint16_t Host_CMD_Packet_Len  = 0;
	uint32_t Host_CRC32 = 0;
	// getting the total packet lencg & crc host sent value
	Host_CMD_Packet_Len = Host_Buffer[0] + 1;
	Host_CRC32 = *((uint32_t*)((Host_Buffer + Host_CMD_Packet_Len)-CRC_TYPE_SIZE_BYTE));
	//  crc verification
	if(CRC_VERIFICATION_PASSED == Bootloader_CRC_Verify((uint8_t *)&Host_Buffer[0] , Host_CMD_Packet_Len - 4, Host_CRC32)){
		#if BL_DEBUG_ENABLE  ==    DEBUG_INFO_ENABLE
		BL_Print_Message("Getting BL version \r\n ");
		#endif
		Bootloader_Send_ACK(4);
		Bootloader_Send_Data_To_Host((uint8_t *)(&BL_version[0]), 4);
		
	}else{
		Bootloader_Send_NACK();		
	}
	
}
static void Bootloader_Get_Help(uint8_t *Host_Buffer){
	uint16_t Host_CMD_Packet_Len  = 0;
	uint32_t Host_CRC32 = 0;
	// getting the total packet lencg & crc host sent value
	Host_CMD_Packet_Len = Host_Buffer[0] + 1;
	Host_CRC32 = *((uint32_t*)((Host_Buffer + Host_CMD_Packet_Len)-CRC_TYPE_SIZE_BYTE));
	if(CRC_VERIFICATION_PASSED == Bootloader_CRC_Verify((uint8_t *)&Host_Buffer[0] , Host_CMD_Packet_Len - 4, Host_CRC32)){
		#if BL_DEBUG_ENABLE  ==    DEBUG_INFO_ENABLE
		BL_Print_Message("Getting BL available commands \r\n ");
		#endif
		Bootloader_Send_ACK(12);
		Bootloader_Send_Data_To_Host(BL_commands,12);
	}else{
		Bootloader_Send_NACK();
	}
}

static void Bootloader_Get_Chip_Identification_Number(uint8_t *Host_Buffer){
	uint16_t Host_CMD_Packet_Len  = 0;
	uint32_t Host_CRC32 = 0;
	uint16_t MCU_Identification_Number = 0 ;
	// getting the total packet lencg & crc host sent value
	Host_CMD_Packet_Len = Host_Buffer[0] + 1;
	Host_CRC32 = *((uint32_t*)((Host_Buffer + Host_CMD_Packet_Len)-CRC_TYPE_SIZE_BYTE));
	if(CRC_VERIFICATION_PASSED == Bootloader_CRC_Verify((uint8_t *)&Host_Buffer[0] , Host_CMD_Packet_Len - 4, Host_CRC32)){
		#if BL_DEBUG_ENABLE  ==    DEBUG_INFO_ENABLE
		BL_Print_Message("Getting Get Chip Identification Number \r\n ");
		#endif
		/* Get the MCU chip identification number */ 
		MCU_Identification_Number = (uint16_t)((DBGMCU->IDCODE) & 0x00000FFF);
		/* Report chip identification number to HOST */
		Bootloader_Send_ACK(2);
		Bootloader_Send_Data_To_Host((uint8_t *)&MCU_Identification_Number, 2);
	}else{
		Bootloader_Send_NACK();
	}
}
static void Bootloader_Jump_User_APP(void){
	uint32_t MSP_Address =  *((volatile uint32_t*)FLASH_MEM_SEC2_BASE_ADDRESS);// first 4 bytes of the flash memory should contains the MSP value
	uint32_t RH_Address = *((volatile uint32_t*)(FLASH_MEM_SEC2_BASE_ADDRESS+4));
	void (*Reset_Handler)(void) =  (void*)RH_Address;
	// from CMSIS drivers --> setting the MSP_Address value to the MSP spicial register 
	__set_MSP(MSP_Address);
	
	// Deinint for modules
	HAL_RCC_DeInit();
	// jumping to the Application reset handler function 
	Reset_Handler();
}

static void Bootloader_Jump_To_Address(uint8_t *Host_Buffer){
	uint16_t Host_CMD_Packet_Len  = 0;
	uint32_t Host_CRC32 = 0;
	uint8_t AddressVerification= ADDRESS_NOT_VALID;

	// getting the total packet lencg & crc host sent value
	Host_CMD_Packet_Len = Host_Buffer[0] + 1;
	Host_CRC32 = *((uint32_t*)((Host_Buffer + Host_CMD_Packet_Len)-CRC_TYPE_SIZE_BYTE));
	if(CRC_VERIFICATION_PASSED == Bootloader_CRC_Verify((uint8_t *)&Host_Buffer[0] , Host_CMD_Packet_Len - 4, Host_CRC32)){
		#if BL_DEBUG_ENABLE  ==    DEBUG_INFO_ENABLE
		BL_Print_Message("Jumpping to Address 0x%x \r\n ",Host_Buffer);
		#endif
		/*Jumpping to Address  */ 
		uint32_t HOST_Jump_Address  = *((uint32_t *)(&Host_Buffer[2]));
		AddressVerification = Host_Address_Verification(HOST_Jump_Address);
		if(ADDRESS_VALID == AddressVerification){
			Jump_PTR Jump_Address = ((Jump_PTR)(HOST_Jump_Address+1));
			Jump_Address();
		}
		/* Report the jumping state  */
		Bootloader_Send_ACK(1);
		Bootloader_Send_Data_To_Host(&AddressVerification, 1);
	}else{
		Bootloader_Send_NACK();
	}

}



static void Bootloader_Erase_Flash(uint8_t *Host_Buffer){
	uint16_t Host_CMD_Packet_Len  = 0;
	uint32_t Host_CRC32 = 0;
	uint8_t Erase_state = UNSUCCESSFUL_ERASE;
	// getting the total packet lencg & crc host sent value
	Host_CMD_Packet_Len = Host_Buffer[0] + 1;
	Host_CRC32 = *((uint32_t*)((Host_Buffer + Host_CMD_Packet_Len)-CRC_TYPE_SIZE_BYTE));
	if(CRC_VERIFICATION_PASSED == Bootloader_CRC_Verify((uint8_t *)&Host_Buffer[0] , Host_CMD_Packet_Len - 4, Host_CRC32)){
		#if BL_DEBUG_ENABLE  ==    DEBUG_INFO_ENABLE
		BL_Print_Message("Erasing Flash \r\n ");
		#endif
		Bootloader_Send_ACK(1);
		Erase_state = Perform_Flash_Erase(Host_Buffer[2], Host_Buffer[3]);
			if(SUCCESSFUL_ERASE == Erase_state){
			/* Report erase Passed */
			Bootloader_Send_Data_To_Host((uint8_t *)&Erase_state, 1);
			#if (BL_DEBUG_ENABLE == DEBUG_INFO_ENABLE)
			BL_Print_Message("Successful Erase \r\n");
			#endif
		}
		else{
			/* Report erase failed */
			Bootloader_Send_Data_To_Host((uint8_t *)&Erase_state, 1);
			#if (BL_DEBUG_ENABLE == DEBUG_INFO_ENABLE)
			BL_Print_Message("Erase request failed !!\r\n");
			#endif
			}
	}else{
		Bootloader_Send_NACK();
	}	
}

static void Test_Bootloader_Erase_Flash(uint8_t *Host_Buffer){
	uint16_t Host_CMD_Packet_Len  = 0;
	//uint32_t Host_CRC32 = 0;
	uint8_t Erase_state = UNSUCCESSFUL_ERASE;
	// getting the total packet lencg & crc host sent value
	//Host_CMD_Packet_Len = Host_Buffer[0] + 1;
	//Host_CRC32 = *((uint32_t*)((Host_Buffer + Host_CMD_Packet_Len)-CRC_TYPE_SIZE_BYTE));
//	if(CRC_VERIFICATION_PASSED == Bootloader_CRC_Verify((uint8_t *)&Host_Buffer[0] , Host_CMD_Packet_Len - 4, Host_CRC32)){
		#if BL_DEBUG_ENABLE  ==    DEBUG_INFO_ENABLE
		BL_Print_Message("Erasing Flash \r\n ");
		#endif
		Bootloader_Send_ACK(1);
		Erase_state = Perform_Flash_Erase(Host_Buffer[2], Host_Buffer[3]);
			if(SUCCESSFUL_ERASE == Erase_state){
			/* Report erase Passed */
			Bootloader_Send_Data_To_Host((uint8_t *)&Erase_state, 1);
			#if (BL_DEBUG_ENABLE == DEBUG_INFO_ENABLE)
			BL_Print_Message("Successful Erase \r\n");
			#endif
		}
		else{
			/* Report erase failed */
			Bootloader_Send_Data_To_Host((uint8_t *)&Erase_state, 1);
			#if (BL_DEBUG_ENABLE == DEBUG_INFO_ENABLE)
			BL_Print_Message("Erase request failed !!\r\n");
			#endif
			}
	//}else{
	//	Bootloader_Send_NACK();
	//}	
}
static void Bootloader_Memory_Write(uint8_t *Host_Buffer){
	uint16_t Host_CMD_Packet_Len  = 0;
	uint32_t Host_CRC32 = 0;
	uint32_t Host_Address = 0;
  uint8_t Payload_Len = 0;	
	uint8_t Address_Verification = ADDRESS_NOT_VALID;
	uint8_t Flash_Payload_Write_Status = FLASH_PAYLOAD_WRITE_FAILED;
// length command address 3 4 5 payload_len App CRC
	
	Host_CMD_Packet_Len = Host_Buffer[0] + 1;
	Host_CRC32 = *((uint32_t*)((Host_Buffer + Host_CMD_Packet_Len)-CRC_TYPE_SIZE_BYTE));
	if(CRC_VERIFICATION_PASSED == Bootloader_CRC_Verify((uint8_t *)&Host_Buffer[0] , Host_CMD_Packet_Len - 4, Host_CRC32)){
		#if BL_DEBUG_ENABLE  ==    DEBUG_INFO_ENABLE
		BL_Print_Message("Writing data to memory Address \r\n ");
		#endif
		Bootloader_Send_ACK(1);
		Host_Address = *((uint32_t*)(&BL_Host_Buffer[2])); // host address extraction
		Payload_Len = Host_Buffer[6];
		#if (BL_DEBUG_ENABLE == DEBUG_INFO_ENABLE)
		BL_Print_Message("HOST_Address = 0x%X \r\n", Host_Address);
		#endif
		Address_Verification = Host_Address_Verification(Host_Address);
		//ADDRESS_VALID == Address_Verification
		if(1){
			// writing the payload to the flasjh memory
			Flash_Payload_Write_Status = Flash_Memory_Write_Payload((uint8_t *)&Host_Buffer[7],Host_Address,Payload_Len);
			//Flash_Payload_Write_Status = Flash_Memory_Write_Payload((uint8_t *)TestPacket,0x08008000,5); // for testing
			if (FLASH_PAYLOAD_WRITE_PASSED == Flash_Payload_Write_Status)
			{
				/* Report payload write passed */
				Bootloader_Send_Data_To_Host((uint8_t *)&Flash_Payload_Write_Status, 1);
				#if (BL_DEBUG_ENABLE == DEBUG_INFO_ENABLE)
				BL_Print_Message("Payload Valid \r\n");
				#endif
			}
			else
			{
				#if (BL_DEBUG_ENABLE == DEBUG_INFO_ENABLE)
				BL_Print_Message("Payload InValid \r\n");
				#endif
				/* Report payload write failed */
				Bootloader_Send_Data_To_Host((uint8_t *)&Flash_Payload_Write_Status, 1);
			}
		}else{
			// reporting Address verfication failuer
			Bootloader_Send_Data_To_Host(&Flash_Payload_Write_Status,1);
		}
		
	}else{
		Bootloader_Send_NACK();
	}	
}
static void Test_Bootloader_Memory_Write(uint8_t *Host_Buffer){
	uint16_t Host_CMD_Packet_Len  = 0;
	uint32_t Host_CRC32 = 0;
	uint32_t Host_Address = 0;
  uint8_t Payload_Len = 0;	
	uint8_t Address_Verification = ADDRESS_NOT_VALID;
	uint8_t Flash_Payload_Write_Status = FLASH_PAYLOAD_WRITE_FAILED;
	uint8_t Decryption_status = DEC_NOT_VERIFIED;
	uint8_t Decrypted_Byte[64] = {0};
	
// length command address 3 4 5 payload_len App CRC
	if(Host_Buffer[7]== 0x01){
		Perform_Flash_Erase(2,5);
	} 
	Host_CMD_Packet_Len = Host_Buffer[0] + 1;
	Host_CRC32 = *((uint32_t*)((Host_Buffer + Host_CMD_Packet_Len)-CRC_TYPE_SIZE_BYTE));
	if(CRC_VERIFICATION_PASSED == Bootloader_CRC_Verify((uint8_t *)&Host_Buffer[0] , Host_CMD_Packet_Len - 4, Host_CRC32)){
		#if BL_DEBUG_ENABLE  ==    DEBUG_INFO_ENABLE
		BL_Print_Message("Writing data to memory Address \r\n ");
		#endif
		Bootloader_Send_ACK(1);
		Host_Address = *((uint32_t*)(&BL_Host_Buffer[2])); // host address extraction
		Payload_Len = Host_Buffer[6];
		#if (BL_DEBUG_ENABLE == DEBUG_INFO_ENABLE)
		BL_Print_Message("HOST_Address = 0x%X \r\n", Host_Address);
		#endif
		Address_Verification = Host_Address_Verification(Host_Address);
		//ADDRESS_VALID == Address_Verification
		if(1){
			// writing the payload to the flasjh memory
			//Flash_Payload_Write_Status = Flash_Memory_Write_Payload((uint8_t *)&Host_Buffer[7],Host_Address,Payload_Len);
			Decryption_status = AES_decrypt_buffer(&Host_Buffer[8],Payload_Len,Decrypted_Byte);
			//Decryption_status = AES_decrypt_buffer(TestPacket,64,Decrypted_Byte);
			HAL_FLASH_Unlock();
			for(uint16_t i = 0 ; i<Payload_Len;i++){
				HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, Host_Address+i, Decrypted_Byte[i]);
			}
			HAL_FLASH_Lock();
			//Flash_Payload_Write_Status = Flash_Memory_Write_Payload((uint8_t *)TestPacket,0x08008000,5); // for testing
			if (FLASH_PAYLOAD_WRITE_PASSED == Flash_Payload_Write_Status)
			{
				/* Report payload write passed */
				Bootloader_Send_Data_To_Host((uint8_t *)&Flash_Payload_Write_Status, 1);
				#if (BL_DEBUG_ENABLE == DEBUG_INFO_ENABLE)
				BL_Print_Message("Payload Valid \r\n");
				#endif
			}
			else
			{
				#if (BL_DEBUG_ENABLE == DEBUG_INFO_ENABLE)
				BL_Print_Message("Payload InValid \r\n");
				#endif
				/* Report payload write failed */
				Bootloader_Send_Data_To_Host((uint8_t *)&Flash_Payload_Write_Status, 1);
			}
		}else{
			// reporting Address verfication failuer
			Bootloader_Send_Data_To_Host(&Flash_Payload_Write_Status,1);
		}
		
	}else{
		Bootloader_Send_NACK();
	}	
}
static void Bootloader_Read_Protection_Level(uint8_t *Host_Buffer){
	uint16_t Host_CMD_Packet_Len = 0;
	uint32_t Host_CRC32 = 0;
	uint8_t RDP_Level = 0;
	
	#if (BL_DEBUG_ENABLE == DEBUG_INFO_ENABLE)
	BL_Print_Message("Read the FLASH Read Protection Out level \r\n");
	#endif
	/* Extract the CRC32 and packet length sent by the HOST */
	Host_CMD_Packet_Len = Host_Buffer[0] + 1;
	Host_CRC32 = *((uint32_t *)((Host_Buffer + Host_CMD_Packet_Len) - CRC_TYPE_SIZE_BYTE));
	/* CRC Verification */
	if (CRC_VERIFICATION_PASSED == Bootloader_CRC_Verify((uint8_t *)&Host_Buffer[0], Host_CMD_Packet_Len - 4, Host_CRC32))
	{
		#if (BL_DEBUG_ENABLE == DEBUG_INFO_ENABLE)
		BL_Print_Message("CRC Verification Passed \r\n");
		#endif
		Bootloader_Send_ACK(1);
		/* Read Protection Level */
		RDP_Level = CBL_STM32F407_Get_RDP_Level();
		/* Report Valid Protection Level */
		Bootloader_Send_Data_To_Host((uint8_t *)&RDP_Level, 1);
	}
	else
	{
#if (BL_DEBUG_ENABLE == DEBUG_INFO_ENABLE)
		BL_Print_Message("CRC Verification Failed \r\n");
#endif
		Bootloader_Send_NACK();
	}

}
static void Bootloader_Change_Read_Protection_Level(uint8_t *Host_Buffer){
	uint16_t Host_CMD_Packet_Len = 0;
	uint32_t Host_CRC32 = 0;
	uint8_t ROP_Level_Status = ROP_LEVEL_CHANGE_INVALID;
	uint8_t Host_ROP_Level = 0; /// {0,0,0,0,0,0}

#if (BL_DEBUG_ENABLE == DEBUG_INFO_ENABLE)
	BL_Print_Message("Change read protection level of the user flash \r\n");
#endif
	/* Extract the CRC32 and packet length sent by the HOST */ // *length command relative crc 
	Host_CMD_Packet_Len = Host_Buffer[0] + 1;
	Host_CRC32 = *((uint32_t *)((Host_Buffer + Host_CMD_Packet_Len) - CRC_TYPE_SIZE_BYTE));
	/* CRC Verification */
	if (CRC_VERIFICATION_PASSED == Bootloader_CRC_Verify((uint8_t *)&Host_Buffer[0], Host_CMD_Packet_Len - 4, Host_CRC32))
	{
#if (BL_DEBUG_ENABLE == DEBUG_INFO_ENABLE)
		BL_Print_Message("CRC Verification Passed \r\n");
#endif
		Bootloader_Send_ACK(1);
		/* Request change the Read Out Protection Level */
		Host_ROP_Level = Host_Buffer[2];
		/* Warning: When enabling read protection level 2, it s no more possible to go back to level 1 or 0 */
		if ((CBL_ROP_LEVEL_2 == Host_ROP_Level) || (OB_RDP_LEVEL_2 == Host_ROP_Level))
		{
			ROP_Level_Status = ROP_LEVEL_CHANGE_INVALID;
		}
		else
		{
			if (CBL_ROP_LEVEL_0 == Host_ROP_Level)
			{
				Host_ROP_Level = 0xAA;
			}
			else if (CBL_ROP_LEVEL_1 == Host_ROP_Level)
			{
				Host_ROP_Level = 0x55;
			}
			ROP_Level_Status = Change_ROP_Level(Host_ROP_Level);
		}
		Bootloader_Send_Data_To_Host((uint8_t *)&ROP_Level_Status, 1);
	}
	else
	{
#if (BL_DEBUG_ENABLE == DEBUG_INFO_ENABLE)
		BL_Print_Message("CRC Verification Failed \r\n");
#endif
		Bootloader_Send_NACK();
	}
}

