/**
 * 7Semi BME690 Library Example
 *
 * Basic Sensor Reading
 *
 * Demonstrates how to initialize the sensor and read
 * environmental measurements.
 *
 * Measured Values
 * - Temperature
 * - Humidity
 * - Pressure
 * - Gas resistance
 *
 * Sensor Connection (I2C)
 * - VCC  -> 3.3V
 * - GND  -> GND
 * - SDA  -> MCU SDA pin
 * - SCL  -> MCU SCL pin
 */

#include <7Semi_BME690.h>

BME69X_7Semi sensor;

void setup()
{
  Serial.begin(115200);

  /**
   * Initialize Sensor
   *
   * Uses default I2C address 0x77
   */
  if (!sensor.begin(0x77))
  {
    Serial.println("BME690 sensor not detected");
    while (1);
  }

  Serial.println("Sensor initialized");
  uint8_t id = 0;
  sensor.getChipID(id);
}

void loop()
{
  float temperature, humidity, pressure, gas;

  /**
   * Read sensor measurements
   */
  if (sensor.getData(temperature, humidity, pressure, gas))
  {
    Serial.print("Temperature: ");
    Serial.println(temperature);

    Serial.print("Humidity: ");
    Serial.println(humidity);

    Serial.print("Pressure: ");
    Serial.println(pressure);

    Serial.print("Gas Resistance: ");
    Serial.println(gas);

    Serial.println();
  }

  delay(1000);
}