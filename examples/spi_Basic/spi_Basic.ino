/**
 * ---
 * 7Semi BME690 Library Example
 * ---
 * SPI Communication Example
 *
 * Demonstrates how to communicate with the BME690 sensor
 * using the SPI interface.
 *
 * The example initializes the sensor through SPI and reads
 * environmental measurements from the device.
 *
 * Measured Values
 * Temperature
 * Humidity
 * Pressure
 * Gas resistance
 *
 * Sensor Connection (SPI)
 * VCC -> 3.3V
 * GND -> GND
 * CS  -> Chip Select pin
 * SCK -> SPI Clock
 * SDO -> SPI MISO
 * SDI -> SPI MOSI
 *
 * Supported Boards
 * Arduino (Uno / Mega / Nano)
 * ESP32
 */

#include <7Semi_BME690.h>

/**

* ---
* SPI Pin Configuration
* ---
* Select the pin configuration depending on the board
* used in the project.
*/

/**
 * Arduino SPI Pins
 */
#define CS 10
#define MOSI 11
#define MISO 12
#define SCK 13

/**
 * ESP32 SPI Pins
 * Uncomment if using ESP32
 */
// #define CS   5
// #define MOSI 23
// #define MISO 19
// #define SCK  18

/**
 * SPI Clock Speed
 *
 * Defines communication speed between the
 * microcontroller and the BME690 sensor.
 */
#define SPI_SPEED 1000000

/**
 * Sensor Object
 *
 * Creates an instance of the BME690 sensor
 * used to control communication and access
 * measurement data.
 */
BME69X_7Semi sensor;

void setup() {
  Serial.begin(115200);

  /**
     * Initialize Sensor Using SPI
     *
     * Parameters
     * CS pin
     *
     * Optional parameters supported by the library
     * SPI interface
     * SPI clock speed
     * custom SCK / MISO / MOSI pins
     */
  if (!sensor.beginSPI(CS)) {
    Serial.println("SPI initialization failed");

    /**
         * Stop execution if sensor is not detected
         */
    while (1)
      ;
  }

  Serial.println("BME690 SPI sensor initialized");
}

void loop() {
  float temperature;
  float humidity;
  float pressure;
  float gas;

  /**
     * Read Sensor Data
     *
     * Retrieves environmental measurements
     * from the BME690 sensor.
     */
  if (sensor.getData(temperature, humidity, pressure, gas)) {
    Serial.print("Temperature: ");
    Serial.print(temperature);

    Serial.print("  Humidity: ");

    Serial.print(humidity);

    Serial.print("  Pressure: ");
    Serial.print(pressure);

    Serial.print("  Gas: ");
    Serial.println(gas);
  }

  delay(1000);
}