#include <SPI.h>
#include <OneWire.h>
#include <DallasTemperature.h>
// #include <nRF24L01.h>
// #include <RF24.h>

// Initialize the nRF24L01 radio
// RF24 radio(9, 10); // CE, CSN
// const uint64_t address = 0xF0F0F0F0E1LL;

// Define the pin for the DS18B20 temperature sensor
#define ONE_WIRE_BUS 2

// Soil moisture sensor calibration values
const int AirValue = 645;   // Value in dry air
const int WaterValue = 254;  // Value in water
int soilMoistureValue = 0;
int soilmoisturepercent = 0;

// Temperature variable
float temperature;

// Initialize the DS18B20 sensor
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Define the data structure to be sent
// NPK fields have been REMOVED
struct MyVariable
{
  byte soilmoisturepercent;
  float temperature;
};
MyVariable variable;


void setup()
{
  Serial.begin(9600);

  // Start the radio
  // radio.begin();
  // radio.openWritingPipe(address);
  // radio.setPALevel(RF24_PA_MAX); // Set to max for better range
  // radio.stopListening();

  // Start the temperature sensor
  sensors.begin();
}

void loop()
{
  // Read soil moisture
  soilMoistureValue = analogRead(A0);
  soilmoisturepercent = map(soilMoistureValue, AirValue, WaterValue, 0, 100);

  // Constrain the moisture percentage value between 0 and 100
  if (soilmoisturepercent > 100)
  {
    soilmoisturepercent = 100;
  }
  else if (soilmoisturepercent < 0)
  {
    soilmoisturepercent = 0;
  }

  // Read temperature
  sensors.requestTemperatures();
  temperature = sensors.getTempCByIndex(0);

  // Assign values to the data structure
  variable.soilmoisturepercent = soilmoisturepercent;
  variable.temperature = temperature;
  
  // Print values to the Serial Monitor for debugging
  Serial.print("Soil Moisture: ");
  Serial.print(variable.soilmoisturepercent);
  Serial.println("%");

  Serial.print("Temperature: ");
  Serial.print(variable.temperature);
  Serial.println("*C");
  Serial.println("--------------------");

  // // Send the data packet
  // if (radio.write(&variable, sizeof(MyVariable))) {
  //   Serial.println("Packet Sent Successfully");
  // } else {
  //   Serial.println("!!! Packet Delivery FAILED !!!");
  // }

  Serial.println("");
  delay(5000); // Send data every 5 seconds
}