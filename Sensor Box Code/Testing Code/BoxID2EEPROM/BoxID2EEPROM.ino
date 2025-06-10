#include <EEPROM.h>

int Box_ID = 3;

void setup() {
  // Store the ID at EEPROM address 0
  EEPROM.write(0, Box_ID);

  // Optional: confirm write
  Serial.begin(9600);
  Serial.println("ID written to EEPROM.");
}

void loop() {
  // Nothing here
}
