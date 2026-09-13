// ===== FINAL STABLE pH SENSOR CODE =====
// Hardware: RC filter added (1k + 0.1uF)
// Board: Arduino Nano (5V)

// Calibrated using normal water (pH = 7)
// Voltage observed ≈ 2.80 V
// calibration_value = 7 - (5.70 × 2.80) ≈ -8.96
float calibration_value = -8.96;

int buffer_arr[10], temp;
float ph_filtered = 7.0;

void setup() {
  Serial.begin(9600);
  Serial.println("pH Sensor Final Stable Output");
  Serial.println("------------------------------");
}

void loop() {

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

  // Serial output
  Serial.print("Voltage: ");
  Serial.print(voltage, 3);
  Serial.print(" V | pH: ");
  Serial.println(ph_filtered-1.5, 2);

  delay(1000);
}
