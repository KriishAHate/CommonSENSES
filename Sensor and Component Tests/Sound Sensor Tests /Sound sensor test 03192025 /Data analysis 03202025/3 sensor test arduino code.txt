// ===================== Required Libraries =====================
// Library for I2C communication, required for ADS1115
#include <Wire.h>

// Library for interfacing with the ADS1115 ADC module
#include <Adafruit_ADS1X15.h>

// Library for handling SD card operations
#include <SD.h>

// ===================== Pin Definitions =====================
// ADS1115 Channel A0 is used for Sound Sensor 1
#define SOUND_SENSOR_1 0  

// ADS1115 Channel A1 is used for Sound Sensor 2
#define SOUND_SENSOR_2 1  

// ADS1115 Channel A2 is used for Sound Sensor 3
#define SOUND_SENSOR_3 2  

// Digital pin 8 controls the MOSFET that powers the SD module
#define MOSFET_SD 8  

// ===================== Global Variable Declarations =====================
// File object for handling SD card file operations
File myFile;  

// File name where data will be stored on the SD card
char filename[] = "log.txt";  

// Number of samples to store before writing to SD card
#define BUFFER_SIZE 10  

// Array to store timestamp values (in milliseconds)
unsigned long sensorTimestamps[BUFFER_SIZE];  

// Array to store readings from Sensor 1
int sensor1Readings[BUFFER_SIZE];  

// Array to store readings from Sensor 2
int sensor2Readings[BUFFER_SIZE];  

// Array to store readings from Sensor 3
int sensor3Readings[BUFFER_SIZE];  

// Object for interfacing with the ADS1115 ADC module
Adafruit_ADS1115 ads;  

// Counter variable used for initialization loops
int count;  

// Maximum attempts to initialize the SD card
#define SD_INIT_COUNT 10000  

// Maximum attempts to initialize the ADC module
#define ADC_INIT_COUNT 50000  

// Delay between reading data and writing it to the SD card (currently 0 ms)
#define RD_TO_SD 0  

// Delay between writing data and the next reading cycle (currently 0 ms)
#define SD_TO_NXT_RD 0  

// Buffer size for storing formatted data before writing to SD
#define SPRINTF_BUF_SIZE 64  

// Character buffer for storing formatted data
char sd_buffer[SPRINTF_BUF_SIZE];  

// ===================== SETUP FUNCTION =====================
void setup() {
  // Initialize serial communication at 9600 baud rate for debugging
  Serial.begin(9600); 

  // ===================== SD CARD POWER CONTROL =====================
  // Set MOSFET control pin as OUTPUT
  pinMode(MOSFET_SD, OUTPUT); 

  // Keep SD card powered ON all the time
  digitalWrite(MOSFET_SD, HIGH); 

  // ===================== SD CARD INITIALIZATION =====================
  // Print initialization message to serial monitor
  Serial.println("Initializing SD card...");

  // Initialize counter
  count = 0;

  // Try initializing SD card up to SD_INIT_COUNT times
  while ((!SD.begin()) && (count < SD_INIT_COUNT)) {
      // Print error message if SD card is not detected
      Serial.println(F("SD ERROR: Module not detected.")); 
      count++;
  }

  // Print final error if SD initialization fails
  if (count == SD_INIT_COUNT) {
        Serial.println(F("SD ERROR: Module not detected.")); 
  } else {
        // Print success message if SD card initializes successfully
        Serial.println(F("SD card initialized.")); 
  }

  // ===================== ADS1115 (EXTERNAL ADC) INITIALIZATION =====================
  // Reset counter
  count = 0;

  // Try initializing ADS1115 up to ADC_INIT_COUNT times
  while ((!ads.begin()) && (count < ADC_INIT_COUNT)) {
      // Print error message if ADC module is not detected
      Serial.println(F("ADS1115 ERROR: Not detected.")); 
      count++;
  }

  // Print final error if ADC initialization fails
  if (count == ADC_INIT_COUNT) {
        Serial.println(F("ADC ERROR: Module not detected.")); 
  } else {
        // Print success message if ADC initializes successfully
        Serial.println(F("ADC initialized successfully.")); 
  }

  // Set ADC gain to 2/3 for a max input voltage of 6.144V
  ads.setGain(GAIN_TWOTHIRDS); 

  // Print final success message for both SD card and ADC initialization
  Serial.println(F("SD and ADS1115 INITIALIZATION SUCCESS")); 
}

// ===================== LOOP FUNCTION =====================
void loop() {
  // Read and store sensor data into buffers
  for (int i = 0; i < BUFFER_SIZE; i++) {
    // Store current time in milliseconds
    sensorTimestamps[i] = millis(); 

    // Read ADC value from Sensor 1
    sensor1Readings[i] = ads.readADC_SingleEnded(SOUND_SENSOR_1);  

    // Read ADC value from Sensor 2
    sensor2Readings[i] = ads.readADC_SingleEnded(SOUND_SENSOR_2);  

    // Read ADC value from Sensor 3
    sensor3Readings[i] = ads.readADC_SingleEnded(SOUND_SENSOR_3);  
  }
    
  // Delay before writing to SD (currently 0 ms)
  delay(RD_TO_SD); 

  // Call function to write data to SD card
  writeToSD(); 

  // Delay before next sensor reading cycle (currently 0 ms)
  delay(SD_TO_NXT_RD); 
}

// ===================== Function to Write Buffered Data to SD Card =====================
void writeToSD() {
  // Open the file for writing (appending data)
  myFile = SD.open(filename, FILE_WRITE); 

  // Check if file opened successfully
  if (!myFile) {
    // Print error message if file cannot be opened
    Serial.println(F("SD WRITE ERROR: Unable to open file.")); 
    return; // Exit function if file opening fails
  }

  // Write buffered data to SD card
  for (int i = 0; i < BUFFER_SIZE; i++) {
    // Format data into a string buffer
    sprintf(sd_buffer, "%lu, %d, %d, %d\n", sensorTimestamps[i], sensor1Readings[i], sensor2Readings[i], sensor3Readings[i]); 

    // Write formatted string to the SD file
    myFile.println(sd_buffer); 
  }

  // Close the file to ensure data is saved properly
  myFile.close(); 

  // Print success message after writing to SD
  Serial.println(F("SD WRITE SUCCESS: Data saved.")); 
}
