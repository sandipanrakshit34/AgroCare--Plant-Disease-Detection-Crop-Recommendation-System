#include <WiFi.h>
#include <HTTPClient.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// ==========================
// WiFi & ThingSpeak Settings
// ==========================
const char* ssid = "Souvik";
const char* password = "12345678";
String serverName = "http://api.thingspeak.com/update?api_key=9QL97JFXQQKSN1E3";

// ==========================
// RF24 Setup
// ==========================
RF24 radio(4, 5); // CE=4, CSN=5 for ESP32
const uint64_t address = 0xF0F0F0F0E1LL;

// Must match transmitter struct exactly
struct MyVariable {
  byte soilmoisturepercent;
  float temperature;  // Transmitter sends float
};
MyVariable variable;

// ==========================
// Timer and WiFi Setup
// ==========================
unsigned long lastTime = 0;
const unsigned long timerDelay = 3000; // 3 seconds delay

WiFiClient client;
HTTPClient http;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("Initializing nRF24 Receiver...");

  // Start RF24 Receiver
  if (!radio.begin()) {
    Serial.println("nRF24 initialization failed! Check wiring & power.");
    while (1);
  }

  radio.openReadingPipe(0, address);
  radio.setChannel(76);           // Must match transmitter
  radio.setDataRate(RF24_1MBPS);  // Must match transmitter
  radio.setPALevel(RF24_PA_LOW);  // Use LOW or MAX depending on range
  radio.startListening();

  if (!radio.isChipConnected()) {
    Serial.println("nRF24 module not detected! Check connections.");
  } else {
    Serial.println("nRF24 module detected and listening...");
  }

  // Connect to WiFi
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Connected to WiFi. IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println("--------------------------------------------");
  Serial.println("Receiver ready. Waiting for data...");
}

// Function to receive data from RF24
bool recvData() {
  if (radio.available()) {
    radio.read(&variable, sizeof(MyVariable));
    return true;
  }
  return false;
}

void loop() {
  // Run every 3 seconds
  if (millis() - lastTime >= timerDelay) {
    if (WiFi.status() == WL_CONNECTED) {

      // Try to receive data
      if (recvData()) {
        float tempC = variable.temperature;
        float tempF = (tempC * 9.0 / 5.0) + 32.0;

        Serial.println("\n--- Data Received from RF24 ---");
        Serial.print("Soil Moisture: ");
        Serial.print(variable.soilmoisturepercent);
        Serial.println("%");

        Serial.print("Temperature: ");
        Serial.print(tempC, 2);
        Serial.print("°C  |  ");
        Serial.print(tempF, 2);
        Serial.println("°F");
        Serial.println("----------------------------------");

        // Send data to ThingSpeak
        String serverPath = serverName + "&field1=" + String(variable.soilmoisturepercent) +
                            "&field2=" + String(tempC, 2);

        http.begin(client, serverPath.c_str());
        int httpResponseCode = http.GET();

        if (httpResponseCode > 0) {
          Serial.print("ThingSpeak HTTP Response: ");
          Serial.println(httpResponseCode);
          String payload = http.getString();
          Serial.println("Response: " + payload);
        } else {
          Serial.print("HTTP Error code: ");
          Serial.println(httpResponseCode);
        }

        http.end();
      } 
      else {
        Serial.println("No RF24 data received. Waiting...");
      }

    } else {
      Serial.println("WiFi disconnected. Reconnecting...");
      WiFi.begin(ssid, password);
    }

    lastTime = millis(); // Update timer
  }
}
