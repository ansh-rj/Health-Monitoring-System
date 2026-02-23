#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Define pins
#define ONE_WIRE_BUS 21     // DS18B20 on GPIO21
#define SD_CS_PIN 5         // CS pin for SD card module

// Sensor objects
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature tempSensor(&oneWire);

// File system variables
File dataFile;
const char* filename = "health_data.csv";

// Heart rate simulation variables (since MAX30105.h wasn't available)
unsigned long lastHeartRateUpdate = 0;
const int HEART_RATE_INTERVAL = 1000; // 1 second
int heartRate = 70; // Simulated heart rate (would come from sensor in real implementation)
int heartRateRange = 10;

// Timing variables
unsigned long previousMillis = 0;
const unsigned long LOG_INTERVAL = 2000; // Log every 2 seconds

void setup() {
  Serial.begin(115200);
  while (!Serial); // Wait for serial connection

  // Initialize temperature sensor
  tempSensor.begin();

  // Initialize SD card
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("SD card initialization failed!");
    while (1); // Halt if SD card fails
  }

  // Create or open the CSV file and write headers
  dataFile = SD.open(filename, FILE_WRITE);
  if (dataFile) {
    dataFile.println("Timestamp(ms),HeartRate(BPM),Temperature(C)");
    dataFile.close();
  } else {
    Serial.println("Error opening file!");
  }

  Serial.println("Starting real-time monitoring...");
  Serial.println("----------------------------------");
  Serial.println("Time(ms)\tBPM\tTemp(°C)");
  Serial.println("----------------------------------");
}

void loop() {
  unsigned long currentMillis = millis();

  // Simulate heart rate variability (replace with actual sensor reading)
  if (currentMillis - lastHeartRateUpdate >= HEART_RATE_INTERVAL) {
    lastHeartRateUpdate = currentMillis;
    heartRate = 70 + random(-heartRateRange, heartRateRange); // Random variation around 70 BPM
  }

  // Log data to SD card and serial monitor at regular intervals
  if (currentMillis - previousMillis >= LOG_INTERVAL) {
    previousMillis = currentMillis;

    // Read temperature
    tempSensor.requestTemperatures();
    float tempC = tempSensor.getTempCByIndex(0);

    // Validate readings
    if (!isnan(tempC) && heartRate > 30) {
      // Display to serial monitor
      Serial.print(currentMillis);
      Serial.print("\t");
      Serial.print(heartRate);
      Serial.print("\t");
      Serial.println(tempC, 1);

      // Write to SD card
      dataFile = SD.open(filename, FILE_WRITE);
      if (dataFile) {
        dataFile.print(currentMillis);
        dataFile.print(",");
        dataFile.print(heartRate);
        dataFile.print(",");
        dataFile.println(tempC, 1);
        dataFile.close();
      } else {
        Serial.println("Error opening file for writing!");
      }
    } else {
      Serial.println("Waiting for valid readings...");
    }
  }
}
