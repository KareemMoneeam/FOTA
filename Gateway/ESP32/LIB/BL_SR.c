#include "BL_SR.h"
#include "CRC.h"

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

    sReplay.toCharArray(Replay, sReplay.length()+1);
    client.publish("/FOTA/BootReply", Replay, false); 
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

    sReplay.toCharArray(Replay, sReplay.length()+1);
    client.publish("/FOTA/BootReply", Replay, false); 
}

void Send_CMD_Jump_To_Application()
{
    char Replay[16] = {0};
    String sReplay = "";
    packet[0] = 5;
    packet[1] = 0x12;
    *((uint32_t*)((uint8_t*)packet + 2)) = calculateCRC32((uint8_t*)packet, 2);
    SendCMDPacketToBootloader(6);
    ReceiveReplayFromBootloader(1);

    if(JUMP_SECCEDDED == packet[0]){ sReplay += "Jump Success"; }
    else{ sReplay += "Jump Fail"; }

    sReplay.toCharArray(Replay, sReplay.length()+1);
    client.publish("/FOTA/BootReply", Replay, false); 
}

void Send_CMD_Erase_Flash()
{
    char Replay[16] = {0};
    String sReplay = "";
    packet[0] = 5;
    packet[1] = 0x13;
    *((uint32_t*)((uint8_t*)packet + 2)) = calculateCRC32((uint8_t*)packet, 2);
    SendCMDPacketToBootloader(6);
    ReceiveReplayFromBootloader(1);

    if(ERASE_SUCCEDDED == packet[0]){ sReplay += "Erase Success";  }
    else{ sReplay += "Erase Unsuccess"; }

    sReplay.toCharArray(Replay, sReplay.length()+1);
    client.publish("/FOTA/BootReply", Replay, false); 
}

void Send_CMD_Upload_Application()
{
    uint8_t isFailed = 0;
    if (Firebase.ready())
    {
        Serial.println("\nDownload file...\n");
  
        // The file systems for flash and SD/SDMMC can be changed in FirebaseFS.h.
        if (!Firebase.Storage.download(&fbdo, STORAGE_BUCKET_ID /* Firebase Storage bucket id */, "Application.bin" /* path of remote file stored in the bucket */, "/update.bin" /* path to local file */, mem_storage_type_flash /* memory storage type, mem_storage_type_flash and mem_storage_type_sd */, fcsDownloadCallback /* callback function */))
            Serial.println(fbdo.errorReason());

        File file = LittleFS.open("/update.bin", "r");                    
        short addressCounter = 0;                                     
        
        while(file.available())
        {
            char Replay[16] = {0};
            String sReplay = "";
            digitalWrite(2, !digitalRead(2));
            uint8_t ByteCounter = 0;                                         
            
            packet[0] = 74;                                               
            packet[1] = 0x14;                                           
            *(int*)(&(packet[2])) = 0x8008000 + (0x40 * addressCounter);  
            packet[6] = 64;                                 
            
            Serial.printf("Start Address: 0x%X\n", *(int*)(&(packet[2])));
            
            while(ByteCounter < 64)
            {
                packet[7 + ByteCounter] = file.read();
                ByteCounter++;
            }
            *((uint32_t*)((uint8_t*)packet + 71)) = calculateCRC32((uint8_t*)packet, 71);
            
            SendCMDPacketToBootloader(75);
            
            ReceiveReplayFromBootloader(1);

            if(WRITE_SUCCEDDED == packet[0]){ sReplay += "Write Success"; }
            else{ isFailed = 1; break; }

            sReplay.toCharArray(Replay, sReplay.length()+1);
            client.publish("/FOTA/BootReply", Replay, false); 

            addressCounter++;
        }

        if(isFailed){ client.publish("/FOTA/BootReply", "Upload Fail", false); }
        else{ client.publish("/FOTA/BootReply", "Upload Success", false);  }
    }
}