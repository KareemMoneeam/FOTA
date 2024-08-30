#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <HardwareSerial.h>


// CMDs
#define CBL_GET_CID_CMD              0x12
#define CBL_GET_RDP_STATUS_CMD       0x13
#define CBL_GO_TO_ADDR_CMD           0x14
#define CBL_FLASH_ERASE_CMD          0x15
#define CBL_MEM_WRITE_CMD            0x16
#define CBL_Get_Report_CMD           0xDD

// ACK
#define NOT_ACKNOWLEDGE       0xAB
#define JUMP_SECCEDDED        0x01
#define ERASE_SUCCEDDED       0x03
#define WRITE_SUCCEDDED       0x01


// 
void Send_CMD_Read_CID();
void Send_CMD_Read_Protection_Level();
void Send_CMD_Jump_To_Application();
void Send_CMD_Erase_Flash();
void Send_CMD_Upload_Application(uint8_t* fileBuffer,size_t fileSize);
void set_serial(void);
uint8_t Send_JUMP_TO_BOOT();
uint8_t Recive_Report();
void Read_Packet(uint8_t* fileBuffer);