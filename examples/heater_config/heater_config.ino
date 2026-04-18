/**
 * 7Semi BME690 Library Example
 * 
 * Heater Configuration
 *
 * Demonstrates how to configure the internal
 * gas sensor heater.
 *
 * The heater improves gas sensing by heating
 * the sensing element to a controlled temperature.
 *
 * Sensor Connection (I2C)
 * - VCC  -> 3.3V
 * - GND  -> GND
 * - SDA  -> SDA
 * - SCL  -> SCL
 */

#include <7Semi_BME690.h>

BME69X_7Semi sensor;

void setup()
{
  Serial.begin(115200);

  if (!sensor.begin())
  {
    Serial.println("Sensor not found");
    while (1);
  }

  /**
   * Configure Heater
   *
   * Parameters
   * - enable
   * - temperature (°C)
   * - duration (ms)
   */
  sensor.setHeater(true, 300, 200);
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

  delay(500);
}