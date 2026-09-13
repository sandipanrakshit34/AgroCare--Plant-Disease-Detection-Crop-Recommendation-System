// ===================== SOIL MOISTURE + TEMPERATURE =====================

// #include <SPI.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Define the pin for the DS18B20 temperature sensor
#define ONE_WIRE_BUS 2

// Soil moisture sensor calibration values
const int AirValue = 645;    // Value in dry air
const int WaterValue = 254; // Value in water
int soilMoistureValue = 0;
int soilmoisturepercent = 0;

// Temperature variable
float temperature;

// Initialize the DS18B20 sensor
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Define the data structure
struct MyVariable
{
  byte soilmoisturepercent;
  float temperature;
};
MyVariable variable;


// ===================== NPK SENSOR =====================

#include <SoftwareSerial.h>
#include <Wire.h>

// RS485 control pins
#define RE 8
#define DE 7

// Modbus RTU requests
const byte nitro[] = {0x01,0x03, 0x00, 0x1e, 0x00, 0x01, 0xe4, 0x0c};
const byte phos[]  = {0x01,0x03, 0x00, 0x1f, 0x00, 0x01, 0xb5, 0xcc};
const byte pota[]  = {0x01,0x03, 0x00, 0x20, 0x00, 0x01, 0x85, 0xc0};

byte values[11];

// SoftwareSerial for NPK sensor
SoftwareSerial mod(10, 11);


// ===================== SETUP =====================

void setup()
{
  Serial.begin(9600);

  // Start temperature sensor
  sensors.begin();

  // Start RS485 communication
  mod.begin(9600);

  pinMode(RE, OUTPUT);
  pinMode(DE, OUTPUT);

  delay(500);
}


// ===================== LOOP =====================

void loop()
{
  // -------- Soil Moisture --------
  soilMoistureValue = analogRead(A0);
  soilmoisturepercent = map(soilMoistureValue, AirValue, WaterValue, 0, 100);

  if (soilmoisturepercent > 100) soilmoisturepercent = 100;
  else if (soilmoisturepercent < 0) soilmoisturepercent = 0;

  // -------- Temperature --------
  sensors.requestTemperatures();
  temperature = sensors.getTempCByIndex(0);

  variable.soilmoisturepercent = soilmoisturepercent;
  variable.temperature = temperature;

  Serial.print("Soil Moisture: ");
  Serial.print(variable.soilmoisturepercent);
  Serial.println("%");

  Serial.print("Temperature: ");
  Serial.print(variable.temperature);
  Serial.println(" *C");

  // -------- NPK Sensor --------
  byte val1, val2, val3;

  val1 = nitrogen();
  delay(250);
  val2 = phosphorous();
  delay(250);
  val3 = potassium();
  delay(250);

  Serial.print("Nitrogen: ");
  Serial.print(val1);
  Serial.println(" mg/kg");

  Serial.print("Phosphorous: ");
  Serial.print(val2);
  Serial.println(" mg/kg");

  Serial.print("Potassium: ");
  Serial.print(val3);
  Serial.println(" mg/kg");

  Serial.println("--------------------");
  delay(5000);
}


// ===================== NPK FUNCTIONS =====================

byte nitrogen()
{
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(10);

  if (mod.write(nitro, sizeof(nitro)) == 8)
  {
    digitalWrite(DE, LOW);
    digitalWrite(RE, LOW);

    for (byte i = 0; i < 7; i++)
    {
      values[i] = mod.read();
      // Serial.print(values[i], HEX);
    }
    // Serial.println();
  }
  return values[4];
}

byte phosphorous()
{
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(10);

  if (mod.write(phos, sizeof(phos)) == 8)
  {
    digitalWrite(DE, LOW);
    digitalWrite(RE, LOW);

    for (byte i = 0; i < 7; i++)
    {
      values[i] = mod.read();
      // Serial.print(values[i], HEX);
    }
    // Serial.println();
  }
  return values[4];
}

byte potassium()
{
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(10);

  if (mod.write(pota, sizeof(pota)) == 8)
  {
    digitalWrite(DE, LOW);
    digitalWrite(RE, LOW);

    for (byte i = 0; i < 7; i++)
    {
      values[i] = mod.read();
      // Serial.print(values[i], HEX);
    }
    // Serial.println();
  }
  return values[4];
}
