/************************************************************
 * @file    Basic_spi.ino
 * @author  7semi
 * @date    2025-07-09
 * @version 1.0.0
 * @license MIT
 * 
 * @brief Example sketch to demonstrate BME690_7semi library in SPI mode.
 * 
 * This sketch initializes the BME690 sensor using the SPI interface,
 * reads temperature, pressure, humidity, and gas resistance,
 * and prints them to the Serial Monitor.
 * 
 * Usage:
 * - Connect BME690 sensor to SPI (use CS pin 10 in this example).
 * - Upload and open Serial Monitor at 115200 baud.
 * 
 * Arduino wrapper and enhancements by 7semi.
 * Based on Bosch BME69x official API.
 *************************************************************/
#include <7semi_BME690.h>  // Include the BME690 sensor library

// Create BME690 object in SPI mode using chip select (CS) pin 10
BME690_7semi bme(10, BME690_7semi::MODE_SPI);

void setup() {
  Serial.begin(115200);  // Start serial communication at 115200 baud rate
  delay(100);            // Short delay to allow serial to initialize

  // Initialize the BME690 sensor
  if (!bme.begin()) {
    Serial.println("BME690 SPI init failed!");  // Print error if sensor not found
    while (1);  // Stop here if initialization fails
  }

  Serial.println("BME690 initialized via SPI.");  // Success message
}

void loop() {
  // Read all sensor values (temperature, pressure, humidity, gas)
  if (bme.readSensorData()) {
    
    // Print temperature in degrees Celsius
    Serial.print("Temperature (°C): ");
    Serial.println(bme.getTemperature(), 2);  // 2 decimal places

    // Print atmospheric pressure in hPa (hectopascals)
    Serial.print("Pressure (hPa): ");
    Serial.println(bme.getPressure(), 2);

    // Print relative humidity in percentage
    Serial.print("Humidity (%): ");
    Serial.println(bme.getHumidity(), 2);

    // Print humidity with optional correction offset (e.g., +12%)
    Serial.print("Corrected Humidity (%): ");
    Serial.println(bme.getCorrectedHumidity(), 2);

    // Print gas resistance in Ohms, useful for air quality sensing
    Serial.print("Gas Resistance (Ohms): ");
    Serial.println(bme.getGasResistance(), 2);

    // Print separator for easier reading in Serial Monitor
    Serial.println("----------");

  } else {
    // If sensor reading fails
    Serial.println("Sensor read failed!");
  }

  delay(2000);  // Wait for 2 seconds before taking the next reading
}
