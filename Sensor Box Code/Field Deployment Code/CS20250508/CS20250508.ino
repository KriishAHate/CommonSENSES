#include "arduino_secrets.h"

//**************************************
//V2.7 do:
//1. Sample data from sensors every 1 minute
//2. Log sensor data to SD card and upload to cloud via LTE module every 15 minutes (15 samples)
//3. Count how much time it takes for the LTE module to upload data every round and log and upload the time with the sensor data
//4. Log error messages to SD card

//V2.8 change:
//1. Add uploading error message to cloud and log to SD
//2. error message lookup table: 
//Error Number | Error Description
//E1           | Serial Port not open
//E2           | T&RH sensor not found
//E3           | RTC not found
//E4           | LTE module connection to cloud timeout
//E5           | LTE module receiving data from Arduino timeout
//E6           | SD module failure
//E7           | Buffer overflow

//V20250508 change:
//1. add ADC module

//Note:
//1. After turning on the SD module, you should use SD.begin() to start the SD module. For LTE module no initiallization is needed.
//2. When the RTC is missing, the logError() cannot get RTC time, so the time will be wrong, but the error code will be correct.
//3. T&RH sensor need 5 seconds to setup
//4. If T&RH sensor missing, T_RH.read() function will stuck the whole program
//
//Ruifeng Song 04/15/2025
//**************************************

// Libraries
#include <Wire.h> // For I2C communication
#include <RTClib.h> // For RTC module
#include <SD.h> // For SD module
#include "AM2315C.h" // For T&RH sensor
#include <Adafruit_ADS1X15.h> // For ADC, convert noise sensor analog signal to digital

// Pin Definitions
#define MOSFET_LTE 7
#define MOSFET_SD 8
#define BUFFER_SIZE 50 // 15 readings * 3 data points per reading + 2 data points for date&time

#define sampleNum 15 // log and upload after collecting 15 samples
#define interval 60000 // Sensor sampling interval in milliseconds (60,000 ms = 1 minute)
#define delayTime 1000 // Set delay_time to be 1 second
#define LTE_conn_time 120 // LTE connection timeout 120 seconds
#define LTE_recv_time 20 // LTE data receiving timeout 20 seconds
#define SD_open_time 10 // SD_module open_file timeout 10 seconds
#define ErrorNum 9999 // Error number for T&RH sensor
#define T_RH_setup_time 5000 // 5 seconds to_setup T&RH sensor
#define one_minute 60000

// Global Variables
RTC_DS3231 rtc;
Adafruit_ADS1115 ads1115; // Create ADC instance
File myFile; // Data file in SD
File errorFile; // Error file in SD
char filename[15];
AM2315C T_RH; // T&RH sensor
int lastDay = 0;
int buffer[BUFFER_SIZE]; // Buffer to store integer data
int bufferIndex = 0;     // Tracks the next position to put data in the buffer
int hour, minute;
int temp, hum, dbValue;

char CharComma = ',';  // Define CharComma as a comma character
char LTEreadyMark = '$'; // LTE will send this mark when it connected to cloud and ready to get message
char LTEreceivedMark = '#'; // LTE will send this mark when cloud successfully received the whole message
unsigned int delayCount = 0;  // Variable to count duration of each part

void setup() {
  pinMode(MOSFET_LTE, OUTPUT);
  pinMode(MOSFET_SD, OUTPUT);
  digitalWrite(MOSFET_LTE, LOW);
  digitalWrite(MOSFET_SD, LOW);

  Serial.begin(9600); // LTE and Arduino are using Serial port for communication

  // Initialize Sensors
  Wire.begin();
  T_RH.begin();
  delay(T_RH_setup_time); // For T&RH sensor to setup

  // Initialize RTC
  rtc.begin();

  // Initialize ADC
  ads1115.begin();
  
}//end setup()

void loop() {
  // Check serial port, T&RH sensor, and RTC
  arePartsReady();
  
  // Collect N = sampleNum readings
  collectSamples();

  // After collecting readings, handle the upload and SD write
  digitalWrite(MOSFET_SD, HIGH);
  digitalWrite(MOSFET_LTE, HIGH);

  // Check if the LTE is connected to LTE and ready to send messages
  if (!isLTEready()) {
    isSDready(); // Setup SD module
    logError(4); // Log E4: LTE module connecting to cloud timeout
  } else {
    Data2Cloud();  // Send Sensor data if LTE is ready
  }//end if()
  
  // Check if the cloud successfully received all messages
  if (!isLTEreceived()) {
    isSDready();
    logError(5); // Log E5: The cloud receiving data timeout
  }

  // Check if SD module is ready
  if (!isSDready()) {
    isLTEready(); // Setup LET module
    Error2Cloud(6); // Log E6: SD module timeout
    isLTEreceived();
  } else {
    write2SD();  // Call write2SD to save data to the SD card
  } // End else()

  bufferIndex = 0;  // Overwrite the buffer from the first position

  // Turn off LTE and SD modules after data processing
  digitalWrite(MOSFET_SD, LOW);
  digitalWrite(MOSFET_LTE, LOW);
  
}//end loop()

void updateDT() {
  DateTime now = rtc.now();
  hour = now.hour();
  minute = now.minute();
}//end updateDT()

int readTemperatureOne() {
  T_RH.read();
  int temp = round(T_RH.getTemperature() * 100); // The sensor output data with two decimal digits, keep the digits and convert to int
  return isnan(temp) ? ErrorNum : temp; // Handle invalid data
}//end readTemperatureOne()

int readHumidity() {
  T_RH.read();
  int hum = round(T_RH.getHumidity() * 100); // Same as above
  return isnan(hum) ? ErrorNum : hum; // Handle invalid data
}//end readHumidity()

int readSound() {
  //int voltageValue = analogRead(SoundSensorPin) / 1024.0 * 5 * 50 * 100; //5 is reference voltage, 50 is converting factor for voltage to dB
  //int voltageValue = analogRead(SoundSensorPin); // Keep raw signal and convert to int
  // Code above is for analog input, for ADC converted digital input, use the following:
  int voltageValue = ads1115.readADC_SingleEnded(0);
  return voltageValue;
}//end readSound()

void getFileName() {
  DateTime now = rtc.now();
  if (now.day() != lastDay) {
    sprintf(filename, "%04d%02d%02d.txt", now.year(), now.month(), now.day());
    lastDay = now.day();
  }//end if()
}//end getFileName()

void updateData() {
  updateDT();
  temp = readTemperatureOne();
  hum = readHumidity();
  dbValue = readSound();
}//end updateData()

void appendToBuffer() {
  // Ensure bufferIndex does not exceed the buffer size
  if (bufferIndex + 5 <= BUFFER_SIZE) { // Updated to account for new parameters
    if (bufferIndex == 0) { // Only append date and time on the first entry
      buffer[bufferIndex++] = hour;
      buffer[bufferIndex++] = minute;
    }//end if()
    buffer[bufferIndex++] = temp;
    buffer[bufferIndex++] = hum;
    buffer[bufferIndex++] = dbValue;
  } else {
    isLTEready();
    Error2Cloud(7);
    isLTEreceived();
    
    isSDready();
    logError(7); // Log E7: Buffer overflow
  }//end else()
}//end appendToBuffer()

void collectSamples() {
  // Collect N = sampleNum readings
  for (int readingCount = 0; readingCount < sampleNum; readingCount++) {
    // Update data and store it
    updateData();
    appendToBuffer();
    // Time interval between each reading
    delay(interval);
  }//end for()
} // End collectSamples()

void write2SD() {
  getFileName(); // Call to update the filename

  myFile = SD.open(filename, FILE_WRITE);
  if (myFile) {
    // Write all the data in the buffer as a single line, separated by comma
    for (int i = 0; i < bufferIndex; i++) {
      myFile.print(CharComma); // Add comma to seperate each value
      myFile.print(buffer[i]);
    }//end for()

    myFile.println();
    myFile.close();
  } else {
    //Serial.println(F("E:open SD."));
  }//end else()
}//end write2SD()

//*** define a logError() function to log error messages to SD card. file name should be 'ErrorLog.txt'. every error message should start with date time stamp into minute will be sufficient not need to be accurate to seconds
// Function to log error messages to SD card
void logError(int ErrorCode) {
  DateTime now = rtc.now();
  char errorMessage[20];
  sprintf(errorMessage, "%04d%02d%02d %02d:%02d %01d", 
          now.year(), now.month(), now.day(), now.hour(), now.minute(), ErrorCode);

  errorFile = SD.open("ErrorLog.txt", FILE_WRITE);
  if (errorFile) {
    errorFile.println(errorMessage);
    errorFile.close();
  } else {
    // Serial.println("Error logging failed.");
  }//end else()
}//end logError()

void Data2Cloud() {
  if (bufferIndex == 0) return; // No data to send

  // Send all the data in the buffer as a single line, separated by comma
  for (int i = 0; i < bufferIndex; i++) {
    Serial.flush();
    Serial.print(CharComma); // Add comma to seperate each value
    Serial.print(buffer[i]);
  }//end for() 
  
  Serial.println(";"); // End of data indication
}//end Data2Cloud()

void Error2Cloud(int ErrorCode) {
  Serial.flush();
  Serial.print("E");
  Serial.print(ErrorCode);
  Serial.println(";"); // End of sending indication
} // End Error2Cloud()

bool isLTEready() {
  // Check if the LTE is ready after turn it on
  // Wait until receive LTEreadyMark from serial port, timeout after 120 seconds
  delayCount = 0;  // Initial the count
  while (delayCount < LTE_conn_time) {
    if (Serial.available() > 0) {
      char receivedChar = Serial.read(); // Read one byte of the incoming character
      if (receivedChar == LTEreadyMark) { // LTE module will send LTEreadyMark (start) when it is ready to receive data
        return true;  // LTE is ready
      }//end if()
    }//end if()
    delay(delayTime);  // Short delay to avoid overwhelming the serial buffer
    delayCount++;
  }//end while

  // Timeout error for LTE communication
  return false; // LTE connecting timeout
} // End isLTEready()

bool isLTEreceived() {
  // Check if the cloud successfully received data
  // Wait until receive "#" from serial port, then turn them off
  delayCount = 0;  // Initial the count
  while (delayCount < LTE_recv_time) {  // Timeout after 20 seconds
    if (Serial.available() > 0) {
      char receivedChar = Serial.read();  // Read the incoming character
      if (receivedChar == LTEreceivedMark) { // when the LTE is done publishing data to the cloud, it will feedback a LTEreceivedMark (end)
        return true;  // return true once we have received LTEreceivedMark, meaning the cloud received whole message
      }//end if()
    }//end if()
    delay(delayTime);  // Help to define the timeout for receiving
    delayCount++;
  }//end while

  // Timeout error for LTE response, cloud fail to receive whole message, return false
  return false; // LTE receiveing timeout
} // End isLTEreceived()

bool isSDready() {
  // Check if the SD module is ready
  // Wait until SD module is ready, timeout after 10 seconds
  delayCount = 0;  // Initial the count
  while (!SD.begin()) {
    if (delayCount > SD_open_time) {
      return false;  // Exit if SD module is not ready within 10 seconds, SD timeout
    }//end if()
    delay(delayTime);  // Short delay before trying again
    delayCount++;
  }//end while

  return true; // SD module is ready
} // End isSDready()

void arePartsReady() {
  if (!Serial) {
    digitalWrite(MOSFET_SD, HIGH);
    digitalWrite(MOSFET_LTE, HIGH);
    isLTEready();
    Error2Cloud(1);
    isLTEreceived();
    
    isSDready();
    logError(1);      // E1: Serial Port not open
    digitalWrite(MOSFET_SD, LOW);
    digitalWrite(MOSFET_LTE, LOW);
  }//end if()

  // Initialize RTC
  if (!rtc.begin()) {
    digitalWrite(MOSFET_SD, HIGH);
    digitalWrite(MOSFET_LTE, HIGH);
    isLTEready();
    Error2Cloud(3);
    isLTEreceived();
    
    isSDready();
    logError(3);      // E3: RTC not found
    digitalWrite(MOSFET_SD, LOW);
    digitalWrite(MOSFET_LTE, LOW);
  }//end if()

  // ***if the T&RH sensor fail to open write the error message to SD
  if (!T_RH.begin()){
    digitalWrite(MOSFET_SD, HIGH);
    digitalWrite(MOSFET_LTE, HIGH);
    isLTEready();
    Error2Cloud(2);
    isLTEreceived();
    
    isSDready();
    logError(2);      // E2: T&RH sensor not found
    digitalWrite(MOSFET_SD, LOW);
    digitalWrite(MOSFET_LTE, LOW);
  }//end if()

} // End arePartsReady()