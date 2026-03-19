/* 
    ESP-NOW Broadcast Master
    Lucas Saavedra Vaz - 2024

    Copyright (C) 2015 Southern Storm Software, Pty Ltd. 
    created the encryption
*/

#include <Crypto.h>// encryption
#include <AES.h>
#include <string.h>

#include "ESP32_NOW.h"//espNow
#include "WiFi.h"

#include <esp_mac.h>  // For the MAC2STR and MACSTR macros

/* Definitions */

#define ESPNOW_WIFI_CHANNEL 6

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
byte person1Dash[16] ={'p', 'e', 'r', 's', 'o', 'n', 1, 'D', 0, 1, 0, 1, 0, 1, 0, 1};
byte person1Grub[16] ={'p', 'e', 'r', 's', 'o', 'n', 1, 'G', 0, 1, 0, 1, 0, 1, 0, 1};
byte person1Walmart[16] ={'p', 'e', 'r', 's', 'o', 'n', 1, 'W',0, 1, 0, 1, 0, 1, 0, 1};

//person2
byte person2Dash[16] ={'p', 'e', 'r', 's', 'o', 'n', 2, 'D', 0, 1, 0, 1, 0, 1, 0, 1};
byte person2Grub[16] ={'p', 'e', 'r', 's', 'o', 'n', 2, 'G', 0, 1, 0, 1, 0, 1, 0, 1};
byte person2Walmart[16] ={'p', 'e', 'r', 's', 'o', 'n', 2, 'W',0, 1, 0, 1, 0, 1, 0, 1};

//person3
byte person3Dash[16] ={'p', 'e', 'r', 's', 'o', 'n', 3, 'D', 0, 1, 0, 1, 0, 1, 0, 1};
byte person3Grub[16] ={'p', 'e', 'r', 's', 'o', 'n', 3, 'G', 0, 1, 0, 1, 0, 1, 0, 1};
byte person3Walmart[16] ={'p', 'e', 'r', 's', 'o', 'n', 3, 'W',0, 1, 0, 1, 0, 1, 0, 1};

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

  // Initialize the Wi-Fi module
  WiFi.mode(WIFI_STA);
  WiFi.setChannel(ESPNOW_WIFI_CHANNEL);
  while (!WiFi.STA.started()) {
    delay(100);
  }

  // Register the broadcast peer
  if (!broadcast_peer.begin()) {
    delay(5000);
    ESP.restart();
  }


}

  String input = "";

void loop() {
  encryption(); // calls the encryption function 
  char data[32]; // holds the info to send


  for(int j=0; j<sizeof(cypher);j++){// assigns the encrypted info into data
    data[j] = cypher[j];
  }


  Serial.available();
  while(input == ""){
    input = Serial.readString();
  }

//person1
  Serial.printf("%s\n", input);
  if(input == "person1 doordash"){
    aes128.encryptBlock(cypher,person1Dash);
    Serial.printf("%s\n", person1Dash);
  }
  if(input == "person1 grubhub"){
    aes128.encryptBlock(cypher,person1Grub);
    Serial.printf("%s\n", person1Grub);
  }
  if(input == "person1 grubhub"){
    aes128.encryptBlock(cypher,person1Walmart);
    Serial.printf("%s\n", person1Walmart);
  }

//person2
   if(input == "person2 doordash"){
    aes128.encryptBlock(cypher,person2Dash);
    Serial.printf("%s\n", person2Dash);
  }
  if(input == "person1 grubhub"){
    aes128.encryptBlock(cypher,person2Grub);
    Serial.printf("%s\n", person2Grub);
  }
  if(input == "person1 grubhub"){
    for(int j=0; j<sizeof(cypher);j++){// assigns the encrypted info into data
      data[j] = person2Walmart[j];
    }
    Serial.printf("%s\n", person2Walmart);
  }

  

  Serial.printf("%s\n", data); // prints what it will send

  broadcast_peer.send_message((uint8_t *)data, sizeof(data));

  delay(5000);

}
