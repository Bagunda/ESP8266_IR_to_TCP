/*
 * Copyright (c) 2022.07.27, entrade.pro, Didenko Alexandr Ivanovich, +79802616369, t.me/Bagunda
 * Program recive IR command and send it to TCP Socket.
 * Create a TCP Server on ESP8266 NodeMCU on port 23. 
 * TCP Socket Server Send Received IR command
*/

#include <ESP8266WiFi.h>

#include <Arduino.h>
#include <IRrecv.h>

#define SendKey 0  //Button to send data Flash BTN on NodeMCU

int port = 23;  //TCP port number
WiFiServer server(port);

//Server connect to WiFi Network
const char *ssid = "MAC";  //Enter your wifi SSID
const char *password = "Entrade777";  //Enter your wifi Password


// An IR detector/demodulator is connected to GPIO pin 14(D5 on a NodeMCU
// board).
// Note: GPIO 16 won't work on the ESP8266 as it does not have interrupts.
// Note: GPIO 14 won't work on the ESP32-C3 as it causes the board to reboot.
#ifdef ARDUINO_ESP32C3_DEV
const uint16_t kRecvPin = 10;  // 14 on a ESP32-C3 causes a boot loop.
#else  // ARDUINO_ESP32C3_DEV
const uint16_t kRecvPin = 14;
#endif  // ARDUINO_ESP32C3_DEV

IRrecv irrecv(kRecvPin);

decode_results results;


//=======================================================================
//                    Power on setup
//=======================================================================
void setup() 
{
  Serial.begin(115200);
  
  irrecv.enableIRIn();  // Start the receiver
  
  Serial.println();
  Serial.print("IRrecvDemo is now running and waiting for IR message on Pin ");
  Serial.println(kRecvPin);
  
  
  pinMode(SendKey,INPUT_PULLUP);  //Btn to send data
  Serial.println();

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password); //Connect to wifi
 
  // Wait for connection  
  Serial.println("Connecting to Wifi");
  while (WiFi.status() != WL_CONNECTED) {   
    delay(500);
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  
  server.begin();
  Serial.print("Open Telnet and connect to IP:");
  Serial.print(WiFi.localIP());
  Serial.print(" on port ");
  Serial.println(port);
}

char convert_half_byte_to_char(byte mybyte){
  if (mybyte == 1) return '1';
  switch (mybyte){
    case 0:
      return '0';
      break;
    case 1:
      return '1';
      break;
    case 2:
      return '2';
      break;
    case 3:
      return '3';
      break;
    case 4:
      return '4';
      break;
    case 5:
      return '5';
      break;
    case 6:
      return '6';
      break;
    case 7:
      return '7';
      break;
    case 8:
      return '8';
      break;
    case 9:
      return '9';
      break;
    case 10:
      return 'A';
      break;
    case 11:
      return 'B';
      break;
    case 12:
      return 'C';
      break;
    case 13:
      return 'D';
      break;
    case 14:
      return 'E';
      break;
    case 15:
      return 'F';
      break;
  }
}


//=======================================================================
//                    Loop
//=======================================================================

void loop() 
{
  WiFiClient client = server.available();
  
  if (client) {
    if(client.connected())
    {
      Serial.println("Client Connected");
    }
    
    while(client.connected()){      
      while(client.available()>0){
        // read data from the connected client
        Serial.write(client.read()); 
      }
      //Send Data to connected client
      while(Serial.available()>0)
      {
        client.write(Serial.read());
      }
          
      if (irrecv.decode(&results)) {

        uint64_t number = results.value;
        byte half_byte1 = (byte)((number & 0xF0000000) >> 28 );
        byte half_byte2 = (byte)((number & 0x0F000000) >> 24 );
        byte half_byte3 = (byte)((number & 0x00F00000) >> 20 );
        byte half_byte4 = (byte)((number & 0x000F0000) >> 16 );
        byte half_byte5 = (byte)((number & 0x0000F000) >> 12 );
        byte half_byte6 = (byte)((number & 0x00000F00) >> 8 );
        byte half_byte7 = (byte)((number & 0x000000F0) >> 4 );
        byte half_byte8 = (byte)((number & 0x0000000F) );
        String hex2 = String(half_byte1, HEX) + String(half_byte2, HEX) + String(half_byte3, HEX) + String(half_byte4, HEX) + String(half_byte5, HEX) + String(half_byte6, HEX) + String(half_byte7, HEX) + String(half_byte8, HEX);

        char hex3[8];
        hex3[0] = convert_half_byte_to_char(half_byte1);
        hex3[1] = convert_half_byte_to_char(half_byte2);
        hex3[2] = convert_half_byte_to_char(half_byte3);
        hex3[3] = convert_half_byte_to_char(half_byte4);
        hex3[4] = convert_half_byte_to_char(half_byte5);
        hex3[5] = convert_half_byte_to_char(half_byte6);
        hex3[6] = convert_half_byte_to_char(half_byte7);
        hex3[7] = convert_half_byte_to_char(half_byte8);
        
        client.write(hex3);
        Serial.print(results.value, HEX);
        Serial.print(" - ");
        Serial.print(hex3);
        Serial.println("");
        irrecv.resume();  // Receive the next value
        delay(300);
      }
    }
    client.stop();
    Serial.println("Client disconnected");    
  }
}
//=======================================================================
