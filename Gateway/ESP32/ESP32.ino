#include "BL_SR.h"


// CMDs
// #define CMD_GET_Decision 0x00
// #define CMD_Report 0x01
// #define CMD_Critical 0x02
// #define CMD_Send_File 0x03



void handleRoot();
void handleNotFound();
void handleFileUpload();
void SendReport();
void SendFileRecivedACK();

// WIFI CRED
// const char* ssid = "Don't_Ask";
// const char* password = "@ourFamily.4f4e45";
const char* ssid = "Gabr";
const char* password = "12345678";
WebServer server(8081);

//====Decisions Var==== 
bool decision = false; 
bool isReport = false;
bool critical = false;

//====File Buffer Var====
uint8_t* fileBuffer = nullptr;
uint8_t ReportBuffer[200] = {0};
size_t bufferIndex = 0;
size_t bufferSize = 0;

//====Server URL====
String serverURL = "http://192.168.137.135:8000/";

//=====================================SetUp=====================================
void setup() {
  //setting the custom uart
  Serial.begin(115200);
  Serial1.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, 16, 17);
  // HardwareSerial SerialPort(2);
  // SerialPort.begin(15200, SERIAL_8N1, 16, 17);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  
  
  server.on("/", handleRoot);
  server.on("/upload", HTTP_POST, []() {
    server.send(200, "text/plain", "File Uploaded Successfully");
  }, handleFileUpload);
  server.onNotFound(handleNotFound);
  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
  Serial.println("HTTP server started");
}

void loop() 
{
  if (Recive_Report())
  {
    Read_Packet(ReportBuffer);
    SendReport(ReportBuffer);
  }
}



//=====================================Handle server pages=====================================

// Just check if it is critical or feature
void handleRoot() {
  
  Serial.println("Comming req");
  

  if (server.args() > 0 ) { 
    if (server.hasArg("critical")) {
      Serial.println("Setting the critical var to TRUE");
      critical = (server.arg(0) == "True");  
    }
  }

  if (!critical) {
    Serial.println("There is a feature to install make decision :- ");

    while (Serial.available() == 0){}

    int Choice = Serial.parseInt();
    if (Choice == 0)
    {
      decision = false;
    }
    else {
      decision = true;
    }
  }

  server.send(200, "text/plain", "done");
  
}
void handleFileUpload() 
{
  Serial.println("I'm in handleFileUpload");
  // Serial2.write(0xFF); // make the app jump to bootloader 
  // delay(5000);
  if (critical || decision)
  {
    Serial.println("I'm in if");
    HTTPUpload& upload = server.upload();
    Serial.println(upload.status);
    if (upload.status == UPLOAD_FILE_START) {
      String filename = upload.filename;
      if (!filename.startsWith("/")) {
        filename = "/" + filename;
      }
      Serial.print("Handle file upload name: ");
      Serial.println(filename);
      bufferIndex = 0; // Reset buffer index for new file upload
      bufferSize = 0; // Reset buffer size
      if (fileBuffer) {
        delete[] fileBuffer; // Free any previously allocated buffer
        fileBuffer = nullptr;
      }
    } 
    else if (upload.status == UPLOAD_FILE_WRITE) {
      if (bufferIndex + upload.currentSize > bufferSize) {
        // Reallocate buffer
        size_t newBufferSize = bufferIndex + upload.currentSize;
        uint8_t* newBuffer = new uint8_t[newBufferSize];
        if (fileBuffer) 
        {
          memcpy(newBuffer, fileBuffer, bufferIndex);
          delete[] fileBuffer;
        }
        fileBuffer = newBuffer;
        bufferSize = newBufferSize;
      }
      memcpy(fileBuffer + bufferIndex, upload.buf, upload.currentSize);
      bufferIndex += upload.currentSize;
      Serial.println("File Out");
    } 
    else if (upload.status == UPLOAD_FILE_END) {
      Serial.println("File upload completed");
      Serial.print("File size =");
      Serial.println(bufferSize);
      server.send(200, "text/plain", "File Uploaded Successfully");
      if (decision) {SendFileRecivedACK();}
      printFileContent();
      if(Send_JUMP_TO_BOOT()){
      Send_CMD_Upload_Application(fileBuffer,bufferSize);
      Send_CMD_Jump_To_Application();
      }
    }
  }  // heap size = 80
}
void handleNotFound() 
{
  server.send(404, "text/plain", "404: Not found"); 
}

//=====================================Send Requests to external server=====================================
void SendFileRecivedACK() 
{
    if (WiFi.status() == WL_CONNECTED) {
      WiFiClient client;
      HTTPClient http;
      String serverURLPath = serverURL + "index/files/receive-success/";
      http.begin(client,serverURLPath);  

      http.addHeader("Content-Type", "application/x-www-form-urlencoded"); 

      
      String postData = "accept=1";

      int httpResponseCode = http.POST(postData);

      if (httpResponseCode > 0) {
        String response = http.getString();  
        Serial.println(httpResponseCode); 
        Serial.println(response);  
      } 
      else {
        Serial.print("Error on sending POST: ");
        Serial.println(httpResponseCode);
      }

      http.end();  
  } 
  else {
    Serial.println("WiFi not connected");
  }
}
void SendReport(uint8_t* reportText) 
{
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;
    String serverURLPath = serverURL + "index/files/reports/reports_incomeing";
    http.begin(client,serverURLPath);  

    http.addHeader("Content-Type", "application/x-www-form-urlencoded"); 

    String postData = "username=CAR1&report_text=" + String((char*)reportText);

    int httpResponseCode = http.POST(postData);

    if (httpResponseCode > 0) {
      String response = http.getString();  
      Serial.println(httpResponseCode); 
      Serial.println(response);  
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }

    http.end();  
  } else {
    Serial.println("WiFi not connected");
  }
}




//=====================================Print File for testing=====================================
void printFileContent() 
{
  for (size_t i = 0; i < bufferSize; i++) {
    Serial.print(fileBuffer[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
}