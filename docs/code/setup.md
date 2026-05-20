# Code Setup

This page walks you through setting up the Arduino IDE, running the test scripts to verify each module, and flashing the final deployment code.

---

## What you'll need

- Computer with [Arduino IDE](https://www.arduino.cc/en/software) installed
- FTDI cable (for Arduino)
- USB-C cable (for Particle Boron)
- A [Particle account](https://particle.io) with the Boron registered to it

---

## Libraries to install

Open Arduino IDE, go to **Sketch → Include Library → Manage Libraries** and install:

| Library | Used for |
|---------|----------|
| `RTClib` | DS3231 real-time clock |
| `AM2315C` | Temperature & humidity sensor |
| `Adafruit ADS1X15` | ADC module |
| `SD` | microSD card |
| `Wire` | I2C communication |
| `EEPROM` | Box ID storage |

---

## Step 1 — Set the Box ID

!!! note "Coming soon"
    A dedicated script to write the Box ID to EEPROM will be added here. Each box must have a unique ID set before flashing the deployment code.

---

## Step 2 — Run the test scripts

Before flashing the deployment code, verify each module works individually. Connect the Arduino to your computer using the FTDI cable.

For each test:

1. Download the script and open it in Arduino IDE
2. Go to **Tools → Board** and select **Arduino Pro Mini**
3. Go to **Tools → Port** and select the port corresponding to your FTDI cable
4. Click **Upload**
5. Open **Serial Monitor** at **9600 baud**
6. Check the expected output

---

### Test 1 — RTC

Checks that the real-time clock is working and keeping accurate time. Before running the test, sync the RTC to your computer's current time using the Python script below. It connects to the Arduino over serial and continuously sends the current timestamp until you stop it.

1. Make sure you have Python installed with the `pyserial` library (`pip install pyserial`)
2. Download and open the script below in Jupyter or run it directly in Python
3. Change `COM5` to match the port your FTDI cable is on
4. Run the script — it will start sending the current time to the Arduino
5. Stop it after a few seconds with **Ctrl+C**

<a href="/CommonSENSES/assets/RTC_sync.ipynb" target="_blank">Download RTC_sync.ipynb</a>

Then flash the RTC test script to verify the time was set correctly:

<a href="/CommonSENSES/assets/rtc_test.txt" target="_blank">Download rtc_test.txt</a>

**Pass:** Serial Monitor prints the correct current timestamp every second.

---

### Test 2 — Temperature & Humidity Sensor

Checks that the AM2315C sensor is wired correctly and returning valid readings. The sensor needs 5 seconds to stabilize after power on before it starts reporting.

<a href="/CommonSENSES/assets/temp_rh_test.txt" target="_blank">Download temp_rh_test.txt</a>

**Pass:** Readings update every 2 seconds and match a reference thermometer.

---

### Test 3 — SD Card

Checks that the SD module is powered, initialized, and able to write a file. The SD card must be 32 GB or less and formatted as FAT32.

<a href="/CommonSENSES/assets/sd_test.txt" target="_blank">Download sd_test.txt</a>

**Pass:** Serial Monitor prints "Writing to test.txt…done." and the file appears on the SD card.

---

### Test 4 — LTE Communication

Checks that the Arduino and Particle Boron can communicate over serial. The Boron must already be flashed and powered on before running this test — see Step 3 below.

<a href="/CommonSENSES/assets/lte_test.txt" target="_blank">Download lte_test.txt</a>

**Pass:** The [Particle Console](https://console.particle.io) shows a "Test" event appearing under your device.

!!! warning
    The LTE module takes up to **2 minutes** to connect. Do not unplug during this time.

---

## Step 3 — Flash the Particle Boron

The Boron runs its own firmware that handles one job — receive data packets from the Arduino over serial and publish them to the Particle Cloud. It uses a handshake system: it sends `$` to tell the Arduino it is ready, receives the data, publishes it, then sends `#` to tell the Arduino it is done.

1. Connect the Boron to your computer via USB-C
2. Log into [Particle Web IDE](https://build.particle.io)
3. Create a new app and paste the contents of the file below into it
4. Click **Flash**

<a href="/CommonSENSES/assets/cellular_module.txt" target="_blank">Download cellular_module.txt</a>

The Boron will publish a "Setup Done" event to the Particle Console when it boots successfully.

---

## Step 4 — Flash the deployment code

The main firmware runs the full data collection and upload loop. Here is what it does:

- Reads temperature, humidity, and noise every **1 minute**
- Buffers 15 readings, then uploads them to the cloud every **15 minutes** via the Boron
- Saves every reading locally to the SD card as a backup
- On startup, sends a setup message to the cloud with the Box ID and timestamp
- If the LTE upload fails, data is saved to SD only
- If the SD fails, data is uploaded to the cloud only
- If both fail, the loop restarts and tries again next cycle
- All errors are logged with a timestamp and error code to `ErrorLog.txt` on the SD card
- The Box ID is read from EEPROM on startup — make sure Step 1 is done first

!!! tip "Customizing the sampling and upload intervals"
    The sampling interval and number of samples before upload can be changed at the top of the code. `interval` controls how often a reading is taken (default: 60,000 ms = 1 minute). `sampleNum` controls how many readings are buffered before uploading (default: 15, meaning upload every 15 minutes). Adjust these to suit your deployment needs.

1. Download the file below and open it in Arduino IDE
2. Go to **Tools → Board** and select **Arduino Pro Mini**
3. Go to **Tools → Port** and select the port corresponding to your FTDI cable
4. Click **Upload**

<a href="/CommonSENSES/assets/deployment.txt" target="_blank">Download deployment.txt</a>

---

## Error codes

If something goes wrong the box logs an error code to `ErrorLog.txt` on the SD card and uploads it to the cloud.

| Code | Description |
|------|-------------|
| E1 | Serial port not open |
| E2 | T&RH sensor not found |
| E3 | RTC not found |
| E4 | LTE connection timeout |
| E5 | LTE receiving data timeout |
| E6 | SD module failure |
| E7 | Buffer overflow |
| E8 | SD file open failure |
| E9 | SD error logging failure |
| E10 | ADC not found |

---

## Final stability test

1. Take the box outside to an area with good LTE signal.
2. Connect the battery to power the system on.
3. Confirm data appears on the [Particle Console](https://console.particle.io) for at least **60 minutes** (4 full upload cycles).
4. If all tests pass, place a sticker inside the box with the date and your initials and move it to inventory.