#include <Wire.h>
#include <Adafruit_ADS1X15.h>

Adafruit_ADS1115 ads;  // Create ADS1115 instance
// ADS1115 full-scale range for gain = 2/3 is ±6.144V (i.e., 0–6.144V in single-ended mode)
// It outputs a 16-bit signed integer, so max value = 32767
// Each bit (LSB) = 6.144V / 32767 = 0.1875 mV

// To estimate the ADC value for a known input like 5V:
// 5V / 6.144V = 0.813  → this is the fraction of the full-scale input
// 0.813 × 32767 ≈ 26636 → this is the expected raw ADC value for a 5V input
void setup() {
  Serial.begin(9600);
  ads.begin(); 
}

void loop() {
  int16_t adc0 = ads.readADC_SingleEnded(0);  // Read from A0
  float voltage = (float)adc0 * 0.1875 / 1000.0; //  cast 

  Serial.print("ADC Reading: "); Serial.print(adc0);
  Serial.print(" | Voltage: "); Serial.print(voltage); Serial.println(" V");

  delay(1000);
}
