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
#define RE 7
#define DE 8

// Modbus RTU requests
const byte nitro[] = {0x01,0x03, 0x00, 0x1e, 0x00, 0x01, 0xe4, 0x0c};
const byte phos[]  = {0x01,0x03, 0x00, 0x1f, 0x00, 0x01, 0xb5, 0xcc};
const byte pota[]  = {0x01,0x03, 0x00, 0x20, 0x00, 0x01, 0x85, 0xc0};

byte values[11];

// SoftwareSerial for NPK sensor
SoftwareSerial mod(11, 10);


// ===================== pH SENSOR =====================

// ===== FINAL STABLE pH SENSOR CODE =====
// Hardware: RC filter added (1k + 0.1uF)
// Board: Arduino Nano (5V)

// Calibrated using normal water (pH = 7)
// Voltage observed ≈ 2.80 V
// calibration_value = 7 - (5.70 × 2.80) ≈ -8.96
float calibration_value = -8.96;

int buffer_arr[10], temp;
float ph_filtered = 7.0;


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

  Serial.println("pH Sensor Final Stable Output");
  Serial.println("------------------------------");

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


  // -------- pH SENSOR --------

  // Take 10 ADC samples
  for (int i = 0; i < 10; i++) {
    buffer_arr[i] = analogRead(A2);
    delay(20);
  }

  // Sort samples (noise rejection)
  for (int i = 0; i < 9; i++) {
    for (int j = i + 1; j < 10; j++) {
      if (buffer_arr[i] > buffer_arr[j]) {
        temp = buffer_arr[i];
        buffer_arr[i] = buffer_arr[j];
        buffer_arr[j] = temp;
      }
    }
  }

  // Average middle 6 values
  unsigned long avgval = 0;
  for (int i = 2; i < 8; i++) {
    avgval += buffer_arr[i];
  }

  // Convert ADC to voltage (5V reference)
  float voltage = (float)avgval * 5.0 / 1024.0 / 6.0;

  // Raw pH calculation (NON-INVERTING module)
  float ph_raw = 5.70 * voltage + calibration_value;

  // Smooth output (essential for pH probes)
  ph_filtered = (ph_filtered * 0.9) + (ph_raw * 0.1);

  Serial.print("Voltage: ");
  Serial.print(voltage, 3);
  Serial.print(" V | pH: ");
  Serial.println(ph_filtered, 2);


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
    }
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
    }
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
    }
  }
  return values[4];
}