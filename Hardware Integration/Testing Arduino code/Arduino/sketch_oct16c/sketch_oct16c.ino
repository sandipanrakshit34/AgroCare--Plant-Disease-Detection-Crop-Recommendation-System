#include <WiFi.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// --- Wi-Fi and ThingSpeak Configuration ---
String apiKey = "JKR1I6CP7FW8E3VK";
const char* ssid = "PRASANTA";
const char* password = "p9874714020";
const char* server = "api.thingspeak.com";
WiFiClient client;

// --- nRF24L01 Configuration ---
RF24 radio(4, 5); // CE, CSN pins
const uint64_t address = 0xF0F0F0F0E1LL;

// --- Data Structure ---
// This MUST exactly match the struct on the transmitter
struct MyVariable
{
  byte soilmoisturepercent;
  float temperature;
};
MyVariable variable;

void setup() 
{
  Serial.begin(115200);

  // Start the nRF24L01 radio
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MAX); // Use the same PA level as the transmitter
  radio.startListening();
  
  Serial.println("Receiver Started. Waiting for data...");
  
  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
}

void loop()
{
  // Check if data is available from the transmitter
  if (radio.available()) 
  {
    radio.read(&variable, sizeof(MyVariable));

    // Print received data to Serial Monitor
    Serial.println("--------------------");
    Serial.println("Data Received:");
    Serial.print("Soil Moisture: ");
    Serial.print(variable.soilmoisturepercent);
    Serial.println("%");
    Serial.print("Temperature: ");
    Serial.print(variable.temperature);
    Serial.println("*C");
    
    // Send data to ThingSpeak
    if (client.connect(server, 80)) 
    {
        // Construct the data payload for ThingSpeak
        String postStr = apiKey;
        postStr += "&field1=";
        postStr += String(variable.soilmoisturepercent);
        postStr += "&field2="; // Fixed: Added field for temperature
        postStr += String(variable.temperature);
        
        // Make the HTTP POST request
        client.print("POST /update HTTP/1.1\n");
        client.print("Host: api.thingspeak.com\n");
        client.print("Connection: close\n");
        client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
        client.print("Content-Type: application/x-www-form-urlencoded\n");
        client.print("Content-Length: ");
        client.print(postStr.length());
        client.print("\n\n");
        client.print(postStr);
        
        Serial.println("Data sent to ThingSpeak!");
        Serial.println("--------------------");
     }
     client.stop();
  }
}