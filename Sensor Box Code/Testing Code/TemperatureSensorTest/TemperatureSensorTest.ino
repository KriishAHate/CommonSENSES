#include <Wire.h> // For I2C communication
#include "AM2315C.h" // For T&RH sensor

AM2315C T_RH; // T&RH sensor

void setup() {

  Serial.begin(9600); // LTE and Arduino are using Serial port for communication

  // Initialize Sensors
  Wire.begin();
  T_RH.begin();
  delay(5000); // For T&RH sensor to setup
  
}//end setup()

void loop(){
  T_RH.read();
  Serial.print("Temperature: ");
  Serial.print(T_RH.getTemperature());
  Serial.print(", Relative Humidity: ");
  Serial.println(T_RH.getHumidity());
  delay(2000);
}