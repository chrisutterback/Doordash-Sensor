/*
  ESP-NOW Broadcast Slave
    Lucas Saavedra Vaz - 2024
*/




#include "ESP32_NOW.h"
#include "WiFi.h"

#include <esp_mac.h>  // For the MAC2STR and MACSTR macros

#include <vector>

#include <Crypto.h> // encryption
#include <AES.h>
#include <string.h>

/* Definitions */

#define ESPNOW_WIFI_CHANNEL 6

/* Classes */


//key[16] cotain 16 byte key(128 bit) for encryption
byte key[16]={0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
byte decryptedtext[16];
//cypher[16] stores the encrypted text
byte cypher[16];
//creating an object of AES128 class
AES128 aes128;

//person1
byte person1Dash[16] ={'p', 'e', 'r', 's', 'o', 'n', '1', 'D', 0, 1, 0, 1, 0, 1, 0, 1};
byte person1Grub[16] ={'p', 'e', 'r', 's', 'o', 'n', '1', 'G', 0, 1, 0, 1, 0, 1, 0, 1};
byte person1Walmart[16] ={'p', 'e', 'r', 's', 'o', 'n', '1', 'W',0, 1, 0, 1, 0, 1, 0, 1};

//person2
byte person2Dash[16] ={'p', 'e', 'r', 's', 'o', 'n', '2', 'D', 0, 1, 0, 1, 0, 1, 0, 1};
byte person2Grub[16] ={'p', 'e', 'r', 's', 'o', 'n', '2', 'G', 0, 1, 0, 1, 0, 1, 0, 1};
byte person2Walmart[16] ={'p', 'e', 'r', 's', 'o', 'n', '2', 'W',0, 1, 0, 1, 0, 1, 0, 1};

//person3
byte person3Dash[16] ={'p', 'e', 'r', 's', 'o', 'n', '3', 'D', 0, 1, 0, 1, 0, 1, 0, 1};
byte person3Grub[16] ={'p', 'e', 'r', 's', 'o', 'n', '3', 'G', 0, 1, 0, 1, 0, 1, 0, 1};
byte person3Walmart[16] ={'p', 'e', 'r', 's', 'o', 'n', '3', 'W',0, 1, 0, 1, 0, 1, 0, 1};

byte nobody[16] ={'p', 'e', 'r', 's', 'o', 'n', '4', 'N',0, 1, 0, 1, 0, 1, 0, 1};

byte lastSend[16] ={'0', '0', '0', '0', '0', '0', 0, '0',0, 1, 0, 1, 0, 1, 0, 1};

byte compare[16];

void encryption() {// decrypts the data
  Serial.begin(9600);
  aes128.setKey(key,16);// Setting Key for AES
}

// Creating a new class that inherits from the ESP_NOW_Peer class is required.

class ESP_NOW_Peer_Class : public ESP_NOW_Peer {
public:
  // Constructor of the class
  ESP_NOW_Peer_Class(const uint8_t *mac_addr, uint8_t channel, wifi_interface_t iface, const uint8_t *lmk) : ESP_NOW_Peer(mac_addr, channel, iface, lmk) {}

  // Destructor of the class
  ~ESP_NOW_Peer_Class() {}

  // Function to register the master peer
  bool add_peer() {
    if (!add()) {
      log_e("Failed to register the broadcast peer");
      return false;
    }
    return true;
  }

  // Function to print the received messages from the master
  void onReceive(const uint8_t *data, size_t len, bool broadcast) {
    int ledCounter = 0; // counts the values to check if it should turn on led
    Serial.printf("Received a message from master " MACSTR " (%s)\n", MAC2STR(addr()), broadcast ? "broadcast" : "unicast");
    for(int i=0; i<sizeof(decryptedtext); i++){// takes the recieved info and puts it into a cypher
      cypher[i] = data[i];
   }
   for(int i=0; i<sizeof(cypher); i++){// prints the decrypted text
    Serial.write(cypher[i]);
   }
    encryption();//decrypts the information
    aes128.decryptBlock(decryptedtext,cypher);

    if(decryptedtext[6] == person1Dash[6]){// if its person1
          if(decryptedtext[7] == person1Dash[7]){
            digitalWrite(4, HIGH);
          }
          else if(decryptedtext[7] == person1Grub[7]){
            digitalWrite(5, HIGH);
          }
          else if(decryptedtext[7] == person1Walmart[7]){
            digitalWrite(6, HIGH);
          }
      }
      if(decryptedtext[6] == person3Dash[6]){// if its person2
          if(decryptedtext[7] == person3Dash[7]){
            digitalWrite(8, HIGH);
          }
          else if(decryptedtext[7] == person3Grub[7]){
            digitalWrite(10, HIGH);
          }
          else if(decryptedtext[7] == person3Walmart[7]){
            digitalWrite(11, HIGH);
          }
      }
      if(decryptedtext[6] == person2Dash[6]){// if its person2
          if(decryptedtext[7] == person3Dash[7]){
            digitalWrite(7, HIGH);
          }
          else if(decryptedtext[7] == person2Grub[7]){
            digitalWrite(0, HIGH);
          }
          else if(decryptedtext[7] == person2Walmart[7]){
            digitalWrite(1, HIGH);
          }
      }

    Serial.printf("Message: ");

    for(int i=0; i<sizeof(decryptedtext); i++){// prints the decrypted text
    Serial.write(decryptedtext[i]);
   }
   Serial.println("");
    for(int i=0; i<sizeof(compare); i++){// prints the decrypted text
    Serial.write(compare[i]);
   }
   Serial.println("");

   Serial.printf("\n");
   if((decryptedtext[6] != compare[6]) || (decryptedtext[7] != compare[7]) || (decryptedtext[6] == nobody[6])){
     //person1 pins
    digitalWrite(4, LOW);
    digitalWrite(5, LOW);
    digitalWrite(6, LOW);

    //person2 pins
    digitalWrite(7, LOW);
    digitalWrite(0, LOW);
    digitalWrite(1, LOW);

    //person3 pins
    digitalWrite(8, LOW);
    digitalWrite(10, LOW);
    digitalWrite(11, LOW);
    }
    for(int i=0; i<sizeof(decryptedtext); i++){// takes the decrytped text and puts it into the compare value
      compare[i] = decryptedtext[i];
   }
   for(int i=0; i<sizeof(compare); i++){// prints the decrypted text
    Serial.write(compare[i]);
   }
   Serial.println("");
   
  }

};



/* Global Variables */
std::vector<ESP_NOW_Peer_Class *> masters;

// Callback called when an unknown peer sends a message
void register_new_master(const esp_now_recv_info_t *info, const uint8_t *data, int len, void *arg) {
  if (memcmp(info->des_addr, ESP_NOW.BROADCAST_ADDR, 6) == 0) {

    ESP_NOW_Peer_Class *new_master = new ESP_NOW_Peer_Class(info->src_addr, ESPNOW_WIFI_CHANNEL, WIFI_IF_STA, nullptr);
    if (!new_master->add_peer()) {
      delete new_master;
      return;
    }
    masters.push_back(new_master);
  } else {
    // The slave will only receive broadcast messages
    log_v("Received a unicast message from " MACSTR, MAC2STR(info->src_addr));
    log_v("Igorning the message");
  }
}

// Main

void setup() {
  Serial.begin(115200);

//readys pins

  //person1 pins
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);

  //person2 pins
  pinMode(7, OUTPUT);
  pinMode(0, OUTPUT);
  pinMode(1, OUTPUT);

  //person3 pins
  pinMode(8, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);

  // Initialize the Wi-Fi module
  WiFi.mode(WIFI_STA);
  WiFi.setChannel(ESPNOW_WIFI_CHANNEL);
  while (!WiFi.STA.started()) {
    delay(100);
  }

  if (!ESP_NOW.begin()) {
    Serial.println("Failed to initialize ESP-NOW");
    Serial.println("Reeboting in 5 seconds...");
    delay(5000);
    ESP.restart();
  }

  ESP_NOW.onNewPeer(register_new_master, nullptr);

}

void loop() {
  delay(100);



}
