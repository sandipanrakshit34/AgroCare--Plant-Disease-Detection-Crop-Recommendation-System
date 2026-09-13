#include <WiFi.h>

const char* ssid = "PRASANTA";      // <-- double-check this
const char* password = "p9874714020";  // <-- and this

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\nStarting WiFi connection test...");

  WiFi.mode(WIFI_STA);
  WiFi.disconnect(true, true); // clear old settings
  delay(1000);

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  unsigned long startAttemptTime = millis();

  // Try for 15 seconds
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 15000) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("✅ WiFi connected successfully!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("❌ WiFi connection failed!");
    Serial.print("Status code: ");
    Serial.println(WiFi.status());
  }
}

void loop() {
}
