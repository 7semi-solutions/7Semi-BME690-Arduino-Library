/************************************************************
 * @file    Basic_I2C.ino
 * @author  7semi
 * @date    2025-07-09
 * @version 1.0.0
 * @license MIT
 * 
 * @brief Example sketch to demonstrate BME690_7semi library in I2C mode.
 * 
 * This sketch initializes the BME690 sensor using I2C interface,
 * reads temperature, pressure, humidity, and gas resistance,
 * and prints them to the Serial Monitor.
 * 
 * Usage:
 * - Connect BME690 sensor to I2C (SDA, SCL).
 * - Upload and open Serial Monitor at 115200 baud.
 * 
 * Arduino wrapper and enhancements by 7semi.
 * Based on Bosch BME69x official API.
 *************************************************************/

#include <Wire.h>                // Include the I2C communication library
#include <7semi_BME690.h>        // Include the BME690 sensor library

// Create a BME690 object using I2C mode with address 0x77
BME690_7semi bme(0x77, BME690_7semi::MODE_I2C);

void setup() {
  Serial.begin(115200);  // Start serial communication at 115200 baud rate

  // Initialize the BME690 sensor
  if (!bme.begin()) {
    Serial.println("BME690 initialization failed!");
    while (1);  // Stop here if sensor is not found
  }
}

void loop() {
  // Read sensor data and check if the reading was successful
  if (bme.readSensorData()) {

    // Print temperature value in degrees Celsius
    Serial.print("Temperature (°C): ");
    Serial.println(bme.getTemperature(), 2);  // 2 decimal places

    // Print pressure value in hectopascals (hPa)
    Serial.print("Pressure (hPa): ");
    Serial.println(bme.getPressure(), 2);

    // Print humidity value as a percentage
    Serial.print("Humidity (%): ");
    Serial.println(bme.getHumidity(), 2);

    // Print humidity with optional correction (e.g., 12% offset for compensation)
    Serial.print("Corrected Humidity (%): ");
    Serial.println(bme.getCorrectedHumidity(), 2);

    // Print gas resistance value in Ohms
    Serial.print("Gas Resistance (Ohms): ");
    Serial.println(bme.getGasResistance(), 2);

    // Separator for readability in serial monitor
    Serial.println("----------------------------");

  } else {
    // If reading fails, print an error message
    Serial.println("Failed to read sensor data.");
  }

  delay(2000);  // Wait for 2 seconds before reading again
}
