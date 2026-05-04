//IP Address: 172.20.10.12
//Must have flutter device connected to the hotspot that the ESP32 is using

/*
WiFi.begin(ssid, pass): Initiates the connection to the specified access point.
WiFi.status(): Returns the current connection status; WL_CONNECTED indicates success.
WiFi.localIP(): Retrieves the local IP address assigned by your router.
WiFi.RSSI(): Measures the signal strength (Received Signal Strength Indicator).
*/
#include <WiFi.h> // connecting to cellular data
#include <WebSocketsServer.h> // allows for two-way communication(check if needs library)

const char* ssid = "LloydsPhone";
const char* password = "password";
#define RXD1 16
#define TXD1 17

WebSocketsServer webSocket = WebSocketsServer(81);
void webSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
  if (type == WStype_CONNECTED) {
    Serial.printf("Client %u connected\n", num);
  }
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); //check baud

  // Setting WiFi mode to station
  WiFi.mode(WIFI_STA);

  // Starts the connection from ESP32 to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  //Wait until connection is successful
  while(WiFi.status() != WL_CONNECTED) {
    delay(2500);
    Serial.print(".");
  }

  //Successful connection
  Serial.println("\nConnected");
  Serial.println(WiFi.localIP()); //Print the IP adress
  Serial1.begin(115200, SERIAL_8N1, RXD1, TXD1);

  webSocket.begin();               
  webSocket.onEvent(webSocketEvent);

}

// List of fake deliveries to cycle through
String fakeDeliveries[] = {"DoorDash", "Uber Eats", "Amazon"};
int deliveryIndex = 0;
String receivedData;
void loop() {
  // put your main code here, to run repeatedly:
  webSocket.loop();
//Testing data
  // Every 5 seconds send a fake delivery notification
  delay(5000);
  String receivedData;
  if (Serial1.available() > 0) {
    
    // Read the incoming data until a newline character '\n' is detected
    receivedData = Serial1.readStringUntil('\n');
    
    // Trim any invisible whitespace or carriage returns
    receivedData.trim();
    
    // If the string isn't empty, handle it
    if (receivedData.length() > 0) {
      Serial.print("Data Received: ");
      Serial.println(receivedData);
    }
  }
  //String message = "DELIVERED:" + fakeDeliveries[deliveryIndex];

  webSocket.broadcastTXT(receivedData);
  Serial.println("Sent: " + receivedData);

  // Move to next delivery, loop back to start after Amazon
  //deliveryIndex = (deliveryIndex + 1) % 3;

  // // Send data every second
  // String data = "SensorValue:" + String(random(0,100)); //sent to flutter app(change depending on what needs to be recieved)
  
  
  // //webSocket.broadcastTXT(data);
  // webSocket.broadcastTXT("DELIVERED:DoorDash");
  // delay(1000);

}
