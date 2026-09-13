#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(9,10); // CE, CSN

void setup() {
  Serial.begin(9600);
  pinMode(10, OUTPUT);             // keep hardware SS as OUTPUT
  Serial.println("Checking nRF24...");

  if (!radio.begin()) {
    Serial.println("❌ nRF24 not responding. Recheck 3.3V, GND, CE/CSN, SPI.");
    while(1);
  }

  radio.setPALevel(RF24_PA_MIN);   // start gentle
  radio.setDataRate(RF24_250KBPS); // most robust
  radio.setChannel(100);           // avoid default
  radio.printDetails();
}

void loop() {}
