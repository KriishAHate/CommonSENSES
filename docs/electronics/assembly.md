# Electronics Assembly

This page walks you through soldering the PCB, mounting all modules, connecting the sensors, and doing a final electrical check before closing the box.

---

## Tools you'll need

- Soldering iron + solder
- Wire strippers
- Multimeter
- Computer with Arduino IDE installed
- FTDI cable (for Arduino)
- USB-C cable (for Particle Boron)
- Particle account at [particle.io](https://particle.io)

---

## Step 1 — Solder the PCB

The custom PCB integrates the Arduino, Boron, RTC, ADC, and microSD module in one place. All modules plug into headers — so the first step is soldering those headers onto the board.

Components to solder onto the PCB:

- Female headers for Arduino Pro Mini
- Female headers for Particle Boron
- Female headers for RTC (DS3231)
- Female headers for ADC (ADS1115)
- Female headers for microSD breakout
- 2-position, 3-position, and 4-position screw terminals
- TN0702 MOSFET
- 1kΩ resistor

Instructions:

1. Start with the lowest-profile components first — the resistor and MOSFET.
2. Insert each component into its marked footprint on the PCB. Check orientation — the MOSFET has a flat face that must align with the silkscreen.
3. Solder one pin first, check alignment, then solder the rest.
4. Solder all female headers. Keep them flush and perpendicular to the board — misaligned headers are the most common assembly error.
5. Solder the screw terminals last.

!!! warning "Header alignment"
    A misaligned female header will prevent the module from seating properly and can cause intermittent electrical failures. After soldering the first pin, sight down the header before soldering the rest.

!!! tip "Tin your wires"
    Before inserting any wire into a screw terminal, twist the strands and apply a small amount of solder to the tip. This prevents fraying and ensures a solid, reliable connection.

---

## Step 2 — Mount modules onto the PCB

Once headers are soldered, seat each module:

1. Press the **Arduino Pro Mini** into its headers. Confirm it is fully seated.
2. Press the **Particle Boron** into its headers.
3. Seat the **DS3231 RTC**, **ADS1115 ADC**, and **microSD breakout** into their respective headers.
4. Insert the **CR1220 coin cell** into the RTC module — this keeps the clock running even when power is off.
5. Insert a formatted **microSD card** into the microSD breakout.

---

## Step 3 — Mount the PCB inside the enclosure

The enclosure has a built-in grid mesh surface for flexible PCB placement using standoffs.

1. Attach the four aluminum standoffs to the PCB mounting holes using 4-40 screws and nylon isolating washers — one washer above and one below the mesh at each standoff.
2. Position the PCB on the enclosure grid mesh and secure with 3 screws into the standoffs below the mesh and 3 screws into the standoffs above.
3. Mount the **sound sensor** to its bracket using 2 screws and 2 nuts, with 4 isolating washers. Position it behind the 6 mm hole in the enclosure wall.
4. Secure the sound sensor bracket to the mesh plate with 2 screws and 2 nuts.

!!! note
    The isolating washers electrically separate the PCB mounting hardware from the enclosure body. Do not skip them.

---

## Step 4 — Connect the sensors and power

1. Route the **temperature/humidity sensor** cable through the cable gland from outside. Connect it to the screw terminal on the PCB. Tighten the cable gland to seal.
2. Connect the **sound sensor** cable to its screw terminal on the PCB.
3. Route the **solar panel USB-C cable** through the cable gland entry and connect to the battery.
4. Mount the **battery** on the interior face of the enclosure door using Velcro tape.
5. Connect the battery output to the PCB power input screw terminal.
6. Apply silicone sealant around the cable gland entry point on the outside of the box.

---

## Step 5 — Tape the cellular antenna

1. Attach the Boron's LTE antenna to the inside wall of the enclosure using tape.
2. Make sure the antenna cable is not pinched or bent sharply.

---

## Step 6 — Cable management

Loose cables inside the box cause connector fatigue and failures in the field.

1. Use zip ties to bundle the temperature sensor cable and the sound sensor/solar cable together.
2. Connect the USB-C elbow connector to the battery's USB-C charging port.
3. Make sure no cables are under tension at any connector point.

---

## Step 7 — Flash the firmware and test

Before closing the box, verify every module works.

**Flash the Arduino:**

1. Connect the Arduino to your computer using the FTDI cable.
2. Open Arduino IDE and upload the main firmware from the repo.
3. Open the Serial Monitor and run each test script in order:

| Test | What to check |
|------|---------------|
| RTC | Serial Monitor prints correct timestamps |
| T&RH sensor | Readings update every 2 seconds and match a weather station |
| Noise sensor + ADC | Serial Monitor prints reasonable noise readings |
| SD module | Serial Monitor says "Writing to test.txt…done." and file appears on SD card |
| Arduino ↔ Boron | Particle console shows "Test" message every 2 seconds |

**Final stability test:**

1. Take the box outside to an area with good LTE signal.
2. Connect the battery USB cable to power the system on.
3. Confirm data uploads to the cloud for at least **60 minutes** (4 full upload cycles).

---

## Final check

Before sealing the box, verify:

- [ ] All modules seated and headers fully engaged
- [ ] Coin cell installed in RTC
- [ ] microSD card inserted
- [ ] PCB mounted with isolating washers at all standoffs
- [ ] Sound sensor mounted behind 6 mm hole with isolating washers
- [ ] All sensor cables connected to screw terminals with tinned ends
- [ ] Battery mounted on door with Velcro
- [ ] Solar cable routed and cable gland silicone sealed
- [ ] LTE antenna taped inside enclosure
- [ ] All zip ties installed, no loose cables
- [ ] All five module tests passed
- [ ] 60-minute cloud upload stability test passed
- [ ] Desiccant packet placed inside box before sealing

---

---

## QA Cards

Complete these QA cards after each assembly step.

- [Arduino Pro Mini QA Card](../assets/Arduinoqacard.pdf)
- [PCB Assembly QA Card](../assets/PCBqacard.pdf)
- [RTC Module QA Card](../assets/rtcqacard.pdf)
- [SD Card Module QA Card](../assets/SDcardqacard.pdf)
- [ADC Module QA Card](../assets/ADCmoduleqacard.pdf)