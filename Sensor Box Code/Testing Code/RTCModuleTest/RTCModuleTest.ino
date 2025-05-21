#include <Wire.h>
#include <RTClib.h>
 
RTC_DS3231 rtc;
 
void setup() {
  Serial.begin(9600);
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
 
  //rtc.adjust(DateTime(2024, 10, 29, 13, 01, 40)); // Set the date and time to YYYY-MM-DD HH:MM:SS
}
 
void loop() {
  DateTime now = rtc.now();
 
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();
 
  delay(1000);
}