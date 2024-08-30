#include "BL_SR.h"
#include "CRC.h"

uint8_t packet [200];
const byte rxPin = 14;
const byte txPin = 12; 
uint32_t App_BaseAddress =  0x08008000;

/***************************************** Bootloader Supported Commands' Functions ********************************************/
void SendCMDPacketToBootloader(uint8_t packetLength)
{
  Serial.println("Send CMDPacke to Bootloader");
    for(uint8_t dataIndex = 0 ; dataIndex < packetLength ; dataIndex++){

      Serial2.write(packet[dataIndex]);
      Serial.print(packet[dataIndex], HEX);
      Serial.print(" ");
    }
    Serial.println("\n");

}

void ReceiveReplayFromBootloader(uint8_t packetLength)
{
    for(uint8_t dataIndex = 0 ; dataIndex < packetLength ; dataIndex++)
    {
        while(!Serial2.available());
        packet[dataIndex] =  Serial2.read();
        if(NOT_ACKNOWLEDGE == packet[0]) break;
    }
}

/***************************************** Bootloader Supported Commands' Functions ********************************************/
void Send_CMD_Read_CID()
{ 
    char Replay[16] = {0};
    String sReplay = "Chip ID: 0x";
    
    packet[0] = 5;
    packet[1] = 0x10;
    *((uint32_t*)((uint8_t*)packet + 2)) = calculateCRC32((uint8_t*)packet, 2);
    SendCMDPacketToBootloader(6);
    ReceiveReplayFromBootloader(2);

    if(NOT_ACKNOWLEDGE == packet[0]){ sReplay = "NACK"; }
    else{ sReplay += String(packet[1], HEX);   sReplay += String(packet[0], HEX); }

}

void Send_CMD_Read_Protection_Level()
{
    char Replay[16] = {0};
    String sReplay = "ProtecLvl: ";

    packet[0] = 5;
    packet[1] = 0x11;
    *((uint32_t*)((uint8_t*)packet + 2)) = calculateCRC32((uint8_t*)packet, 2);
    SendCMDPacketToBootloader(6);
    ReceiveReplayFromBootloader(1);

    if(NOT_ACKNOWLEDGE == packet[0]){ sReplay = "NACK"; }
    else{ sReplay += String(packet[0], HEX); }
}

void Send_CMD_Jump_To_Application()
{
    packet[0] = 5;
    packet[1] = CBL_GO_TO_ADDR_CMD;
    *((uint32_t*)((uint8_t*)packet + 2)) = calculateCRC32((uint8_t*)packet, 2);
    SendCMDPacketToBootloader(6);
}

void Send_CMD_Erase_Flash()
{  
    packet[0] = 7;
    packet[1] = CBL_FLASH_ERASE_CMD;
    packet[2] = 2;   //Sector Index
    packet[3] = 5;   //Num of Sectors
    *((uint32_t*)((uint8_t*)packet + 4)) = calculateCRC32((uint8_t*)packet, 4);
    SendCMDPacketToBootloader(packet[0] + 1);
  //  ReceiveReplayFromBootloader(1);

}

void Send_CMD_Upload_Application(uint8_t* fileBuffer,size_t fileSize)
{
    packet[0] =75 ; //64 + 7 + 4 
    packet[1] = CBL_MEM_WRITE_CMD;
    packet[2] =  App_BaseAddress;
    packet[6] = 64;
    packet[7] = 0x00;

    if (fileBuffer) {
        Serial.println("file buffer contains something ");
    }

    size_t AddressCounter = 0;
    for(size_t i = 0 ;i<fileSize;i++){
      packet[7] = (0== AddressCounter)?0x01:0x00;
      Serial.println(i);
      if(( 0 == i%64 && i != 0) || (fileSize-1 == i) ){
        //*(int*)(&(packet[2])) = 0x8008000 + (0x40 * AddressCounter);
        *((int*)(&(packet[2]))) = 0x08008000 + (0x40 * AddressCounter); 
        Serial.printf("Start Address: 0x%X\n", *(int*)(&(packet[2])));
        *((uint32_t*)(((uint8_t*)packet)+ 8 + 64)) = calculateCRC32((uint8_t*)packet, 72);
        SendCMDPacketToBootloader(packet[0] + 1);
        AddressCounter++;
        delay(2500);//2500
      }
      packet[(i%64) + 8] = fileBuffer[i];
    }
    //ReceiveReplayFromBootloader(1);

    // if(WRITE_SUCCEDDED == packet[0])
    // {
    //   Serial.println("Upload SUCCEDDED");
    //   Send_CMD_Erase_Flash();
    //   Send_CMD_Jump_To_Application();
    // }
    // else{Serial.println("Upload UNSUCCEDDED");}
}
uint8_t Send_JUMP_TO_BOOT()
{
  uint8_t status = 0;
    packet[0] = 0x01;
    packet[1]= 0xFF;
    SendCMDPacketToBootloader(2);
    ReceiveReplayFromBootloader(1);
    if(0x01 == packet[0]){
      status = 1 ;
    }

    return status;
}

uint8_t Recive_Len()
{
  uint8_t status = 0;
  while(Serial2.available())
  {
    packet[0] =  Serial2.read();
  }
  if(packet[0] > 0 )
  {
    status = 1; 
  }
  return status;
}

uint8_t Recive_Report()
{
  uint8_t status = 0;
  for(uint8_t dataIndex = 0 ; dataIndex < 200 ; dataIndex++)
  {
      if(Serial2.available())
      { 
        packet[dataIndex] =  Serial2.read();
        Serial.print(packet[dataIndex], HEX);
        status = 1;
      }
  }
  return status;
}

void Read_Packet(uint8_t* fileBuffer)
{
  for(uint8_t dataIndex = 0 ; dataIndex < 200 ; dataIndex++)
  {
    fileBuffer[dataIndex] = packet[dataIndex];
  }
}