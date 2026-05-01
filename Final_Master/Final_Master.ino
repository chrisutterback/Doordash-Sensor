/* 
    ESP-NOW Broadcast Master
    Lucas Saavedra Vaz - 2024

    Copyright (C) 2015 Southern Storm Software, Pty Ltd. 
    created the encryption
*/

// #include <WiFi.h> // connecting to cellular data
// #include <WebSocketsServer.h> // allows for two-way communication

// const char* ssid = "LloydsPhone"; // Hotspot name
// const char* password = "password"; // Password to hotspot
/*  
  IOS must turn on Maximize Compatibility 
    because IOS defults to WPA3, and ESP32
    only works with WPA2 
*/

//Websocket Setup
// WebSocketsServer webSocket = WebSocketsServer(81);

// void webSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
//   if (type == WStype_CONNECTED) {
//     Serial.printf("Client %u connected\n", num);
//   }
// }

#include <Crypto.h>// encryption
#include <AES.h>
#include <string.h>

#include "ESP32_NOW.h"//espNow
#include "WiFi.h"

#include <esp_mac.h>  // For the MAC2STR and MACSTR macros

/* Definitions */

#define ESPNOW_WIFI_CHANNEL 6
const int sensor = 5; //signal pin of sensor to digital pin 5.  
int state = LOW;
int val = 0; 

/* Classes */

class ESP_NOW_Broadcast_Peer : public ESP_NOW_Peer {
public:
  // Constructor of the class using the broadcast address
  ESP_NOW_Broadcast_Peer(uint8_t channel, wifi_interface_t iface, const uint8_t *lmk) : ESP_NOW_Peer(ESP_NOW.BROADCAST_ADDR, channel, iface, lmk) {}

  // Destructor of the class
  ~ESP_NOW_Broadcast_Peer() {
    remove();
  }

  // Function to properly initialize the ESP-NOW and register the broadcast peer
  bool begin() {
    if (!ESP_NOW.begin() || !add()) {
      log_e("Failed to initialize ESP-NOW or register the broadcast peer");
      return false;
    }
    return true;
  }

  // Function to send a message to all devices within the network
  bool send_message(const uint8_t *data, size_t len) {
    if (!send(data, len)) {
      log_e("Failed to broadcast message");
      return false;
    }
    return true;
  }
};



// Create a broadcast peer object
ESP_NOW_Broadcast_Peer broadcast_peer(ESPNOW_WIFI_CHANNEL, WIFI_IF_STA, nullptr);

// start of encrytion code
//key[16] cotain 16 byte key(128 bit) for encryption
byte key[16]={0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};

//person1
byte mashaDash[16] ={'p', 'e', 'r', 's', 'o', 'n', 1, 'D', 0, 1, 0, 1, 0, 1, 0, 1};
byte mashaGrub[16] ={'p', 'e', 'r', 's', 'o', 'n', 1, 'G', 0, 1, 0, 1, 0, 1, 0, 1};
byte mashaWalmart[16] ={'p', 'e', 'r', 's', 'o', 'n', 1, 'W',0, 1, 0, 1, 0, 1, 0, 1};

//person2
byte katDash[16] ={'p', 'e', 'r', 's', 'o', 'n', 2, 'D', 0, 1, 0, 1, 0, 1, 0, 1};
byte katGrub[16] ={'p', 'e', 'r', 's', 'o', 'n', 2, 'G', 0, 1, 0, 1, 0, 1, 0, 1};
byte katWalmart[16] ={'p', 'e', 'r', 's', 'o', 'n', 2, 'W',0, 1, 0, 1, 0, 1, 0, 1};

//person3
byte bobDash[16] ={'p', 'e', 'r', 's', 'o', 'n', 3, 'D', 0, 1, 0, 1, 0, 1, 0, 1};
byte bobGrub[16] ={'p', 'e', 'r', 's', 'o', 'n', 3, 'G', 0, 1, 0, 1, 0, 1, 0, 1};
byte bobWalmart[16] ={'p', 'e', 'r', 's', 'o', 'n', 3, 'W',0, 1, 0, 1, 0, 1, 0, 1};

//cypher[16] stores the encrypted text
byte cypher[16];
//decryptedtext[16] stores decrypted text after decryption
byte decryptedtext[16];
//creating an object of AES128 class
AES128 aes128; 
 
void encryption() {
  aes128.setKey(key,16); // setting key for AES

}
 
// Main


void setup() {
  Serial.begin(115200);
  Serial1.begin(4800, SERIAL_8N1, 7, 6);
  pinMode(sensor, INPUT); // PIR motion sensor is determined is an input here.  


  // Initialize the Wi-Fi module
  WiFi.mode(WIFI_STA);
  WiFi.setChannel(ESPNOW_WIFI_CHANNEL);
  while (!WiFi.STA.started()) {
    delay(100);
  }

  // Register the broadcast peer
  if (!broadcast_peer.begin()) {
    delay(5000);
    //ESP.restart();
  }

}

void loop() {
  encryption(); // calls the encryption function 
  char data[16]; // holds the info to send

  //sensor
  val = digitalRead(sensor);
  Serial.println(val);
  if (val == HIGH) {  
    Serial.println("Motion detected"); 
    Serial1.println("SCAN_NOW");
    delay(2500);
  }else{
    Serial.println("The action/ motion has stopped"); 
    delay(100);
  }

  String UartData = "";
  if (Serial1.available()) {
    UartData = Serial1.readStringUntil('\n');
    UartData.trim();
    Serial.println(UartData);
  }
  Serial.println(UartData);


//person1

  if(UartData.equals("masha_D")){
    aes128.encryptBlock(cypher,mashaDash);
    Serial.write(mashaDash, 16);
    Serial.printf("d\n");
  }
  if(UartData.equals("masha_G")){
    aes128.encryptBlock(cypher,mashaGrub);
    Serial.write(mashaGrub, 16);
  }
  if(UartData.equals("masha_W")){
    aes128.encryptBlock(cypher,mashaWalmart);
    Serial.write(mashaWalmart, 16);
  }

//person2
   if(UartData.equals("kat_D")){
    aes128.encryptBlock(cypher,katDash);
    Serial.write(katDash, 16);
  }
  if(UartData.equals("kat_G")){
    aes128.encryptBlock(cypher,katGrub);
    Serial.write(katGrub, 16);
  }
  if(UartData.equals("kat_W")){
    aes128.encryptBlock(cypher,katWalmart);
    Serial.write(katWalmart, 16);
  }

//person3
  if(UartData.equals("Bob_D")){
    aes128.encryptBlock(cypher,bobDash);
    Serial.write(bobDash, 16);
  }
  if(UartData.equals("bob_G")){
    aes128.encryptBlock(cypher,bobGrub);
    Serial.write(bobGrub, 16);
  }
  if(UartData.equals("bob_W")){
   aes128.encryptBlock(cypher,bobWalmart);
   Serial.write(bobWalmart, 16);
  }




  for(int j=0; j<sizeof(cypher);j++){// assigns the encrypted info into data
    data[j] = cypher[j];
  }
  

  for(int i=0; i<sizeof(data); i++){// prints the decrypted text
    Serial.write(data[i]);
   } // prints what it will send
   Serial.printf("\n");
   


  broadcast_peer.send_message((uint8_t *)data, sizeof(data));

  delay(5000);

}
