#include "arduino_secrets.h"

// Libraries
#include <Wire.h>
#include <RTClib.h>
#include <SD.h>
#include "AM2315C.h"

// Pin Definitions
#define SoundSensorPin A1
#define MOSFET_LTE 7
#define MOSFET_SD 8
#define BUFFER_SIZE 50 // 15 readings * 3 data points per reading + 2 data points for date&time

// Global Variables
RTC_DS3231 rtc;
File myFile;
char filename[15];
AM2315C DHT;
int lastDay = 0;
int buffer[BUFFER_SIZE]; // Buffer to store integer data
int bufferIndex = 0;     // Tracks the next empty position in the buffer
int durationLTE = 0;     // Tracks the LTE 'on' time
int hour, minute;
int temp, hum, dbValue;

char CharComma = ',';  // Define CharComma as a comma character

unsigned long interval = 5000;  // Sensor sampling interval in milliseconds (60,000 ms = 1 minute)
unsigned int delayTime = 1000;  // Every delay time to be 1 second

void setup() {
  pinMode(MOSFET_LTE, OUTPUT);
  pinMode(MOSFET_SD, OUTPUT);
  digitalWrite(MOSFET_LTE, LOW);
  digitalWrite(MOSFET_SD, LOW);

  Serial.begin(9600); // LTE and Arduino are using Serial port for communication
  if (!Serial) {
    logError("Serial not open");
  }

  // Initialize Sensors
  DHT.begin();
  delay(1000); // For T&RH sensor to setup
  Wire.begin();

  // ***if the T&RH sensor fail to open write the error message to SD
  if (DHT.read() == -1){
    logError("T&RH not found");
  }

  // Initialize RTC
  if (!rtc.begin()) {
    logError("RTC not found");
  }
  // ***if the RTC module fail to open write the error to SD
}

void loop() {
  unsigned int delayCount = 0;  // Variable to store times of delay for counting duration of each part

  // Collect 15 readings
  for (int readingCount = 0; readingCount < 15; readingCount++) {
    // Time interval between each reading
    delay(interval);
    
    // Update data and store it
    updateData();
    appendToBuffer(); 
  }

  // After collecting 15 readings, handle the upload and SD write
  digitalWrite(MOSFET_SD, HIGH);
  digitalWrite(MOSFET_LTE, HIGH);
  unsigned long startLTE = millis(); // Track LTE starting time

  // Wait until receive "$" from serial port, timeout after 120 seconds
  delayCount = 0;  // Initial the count
  while (delayCount < 120) {
    if (Serial.available() > 0) {
      char receivedChar = Serial.read(); // Read one byte of the incoming character
      if (receivedChar == '$') { // LTE module will send '$' (start) when it is ready to receive data
        sendSensorData();  // Send Sensor data
        break;  // Exit the loop once we have received '$' and send the data
      }
    }
    delay(delayTime);  // Short delay to avoid overwhelming the serial buffer
    delayCount++;
  }

  // Timeout error for LTE communication
  if (delayCount == 120) {
    logError("LTE connection timeout");
  }
  
  // Wait until receive "#" from serial port, then turn them off
  delayCount = 0;  // Initial the count
  while (delayCount < 20) {  // Timeout after 20 seconds
    if (Serial.available() > 0) {
      char receivedChar = Serial.read();  // Read the incoming character
      if (receivedChar == '#') { // when the LTE is done publishing data to the cloud, it will feedback a '#' (end)
        break;  // Exit the loop once we have received '#'
      }
    }
    delay(delayTime);  // Short delay to avoid overwhelming the serial buffer
    delayCount++;
  }

  // Timeout error for LTE response
  if (delayCount == 20) {
    logError("LTE receiving timeout");
  }

  // Wait until SD module is ready, timeout after 10 seconds
  delayCount = 0;  // Initial the count
  while (!SD.begin()) {
    if (delayCount > 10) {
      //Serial.println(F("SD timeout;"));
      break;  // Exit if SD module is not ready within 10 seconds
    }
    delay(delayTime);  // Short delay before trying again
    delayCount++;
  }

  writeToSD();  // Call writeToSD to save data to the SD card
  bufferIndex = 0;  // Clear the buffer

  // Turn off LTE and SD modules after data processing
  digitalWrite(MOSFET_SD, LOW);
  digitalWrite(MOSFET_LTE, LOW);
  durationLTE = (millis() - startLTE) / 1000; // Duration of the LTE 'on' time convert to seconds
}

void updateDT() {
  DateTime now = rtc.now();
  hour = now.hour();
  minute = now.minute();
}

int readTemperatureOne() {
  DHT.read();
  int temp = DHT.getTemperature() * 100; // The sensor output data with two decimal digits, keep the digits and convert to int
  return isnan(temp) ? 9999 : temp; // Handle invalid data
}

int readHumidity() {
  DHT.read();
  int hum = DHT.getHumidity() * 100; // Same as above
  return isnan(hum) ? 9999 : hum; // Handle invalid data
}

int readSound() {
  //int voltageValue = analogRead(SoundSensorPin) / 1024.0 * 5 * 50 * 100; //5 is reference voltage, 50 is converting factor for voltage to dB
  int voltageValue = analogRead(SoundSensorPin); // Keep raw signal and convert to int
  return voltageValue;
}

void getFileName() {
  DateTime now = rtc.now();
  if (now.day() != lastDay) {
    sprintf(filename, "%04d%02d%02d.txt", now.year(), now.month(), now.day());
    lastDay = now.day();
  }
}

void updateData() {
  updateDT();
  temp = readTemperatureOne();
  hum = readHumidity();
  dbValue = readSound();
}

void appendToBuffer() {
  // Ensure bufferIndex does not exceed the buffer size
  if (bufferIndex + 5 <= BUFFER_SIZE) { // Updated to account for new parameters
    if (bufferIndex == 0) { // Only append date and time on the first entry
      buffer[bufferIndex++] = hour;
      buffer[bufferIndex++] = minute;
    }
    buffer[bufferIndex++] = temp;
    buffer[bufferIndex++] = hum;
    buffer[bufferIndex++] = dbValue;
  } else {
    //Serial.println(F("Buffer overflow!"));
  }
}

void writeToSD() {
  getFileName(); // Call to update the filename

  myFile = SD.open(filename, FILE_WRITE);
  if (myFile) {
    // Write all the data in the buffer as a single line, separated by comma
    for (int i = 0; i < bufferIndex; i++) {
      myFile.print(buffer[i]);
      myFile.print(CharComma); // Add comma after each value
    }

    myFile.println(durationLTE); // Log LTE duration time
    myFile.close();
  } else {
    //Serial.println(F("E:open SD."));
  }
}

//*** define a logError() function to log error messages to SD card. file name should be 'Error_Log.txt'. every error message should start with date time stamp into minute will be sufficient not need to be accurate to seconds
// Function to log error messages to SD card
void logError(const char* message) {
  DateTime now = rtc.now();
  char errorMessage[100];
  sprintf(errorMessage, "%04d-%02d-%02d %02d:%02d ERROR: %s", 
          now.year(), now.month(), now.day(), now.hour(), now.minute(), message);

  File errorFile = SD.open("Error_Log.txt", FILE_WRITE);
  if (errorFile) {
    errorFile.println(errorMessage);
    errorFile.close();
  } else {
    // Serial.println("Error logging failed.");
  }
}

void sendSensorData() {
  if (bufferIndex == 0) return; // No data to send

  // Send all the data in the buffer as a single line, separated by comma
  for (int i = 0; i < bufferIndex; i++) {
    Serial.flush();
    Serial.print(buffer[i]);
    Serial.print(CharComma); // Add comma after each value
  }
  
  Serial.print(durationLTE); // Log LTE duration time
  Serial.println(";"); // End of data indication
}
