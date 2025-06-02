//**************************************
//This is to test the communication between Arduino and LTE module.
//
//Ruifeng Song 06/02/2025
//**************************************

// Libraries

// Pin Definitions
#define MOSFET_LTE 7

void setup() {
  pinMode(MOSFET_LTE, OUTPUT);
  digitalWrite(MOSFET_LTE, HIGH);

  Serial.begin(9600); // LTE and Arduino are using Serial port for communication

  delay(120000); // Wait for LTE to connect to internet

  Serial.flush();
  Serial.print("Test;"); // Send data to LTE, ';' means the end of the meassage
  
}//end setup()

void loop() {
   
}//end loop()
