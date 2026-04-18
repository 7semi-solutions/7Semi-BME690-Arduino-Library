# 7Semi BME690 Arduino Library

Arduino driver for the **Bosch BME690 environmental sensor**.

The **BME690** is a high-performance environmental sensor capable of measuring **temperature, humidity, pressure, and gas resistance**. It is widely used for **air quality monitoring, environmental sensing, and IoT applications**.

This library provides an easy interface for configuring the sensor and reading environmental measurements using **I²C or SPI communication**.

---

## Features

* Temperature measurement
* Humidity measurement
* Barometric pressure measurement
* Gas resistance measurement
* I²C communication support
* SPI communication support
* Heater configuration for gas sensing
* Oversampling configuration
* IIR filter configuration
* Forced mode measurements
* Parallel mode measurements
* Multiple heater profile support

---

## Supported Platforms

* Arduino UNO / Mega
* ESP32
* ESP8266
* Any board supporting **Wire (I²C)** or **SPI**

---

## Hardware

Supported sensor:
**7Semi Environmental Sensor Breakout - BME690**

---

## Connection

The **BME690 supports both I²C and SPI communication**.

---

## I²C Connection

| BME690 Pin | MCU Pin | Description  |
| ---------- | ------- | ------------ |
| VCC        | 3.3V    | Sensor power |
| GND        | GND     | Ground       |
| SDA        | SDA     | I²C data     |
| SCL        | SCL     | I²C clock    |

### I²C Notes

Default sensor address:

```cpp
0x77
```

Alternative address:

```cpp
0x76
```

Recommended I²C speed:

```cpp
100 kHz – 400 kHz
```

---

## SPI Connection

| BME690 Pin | MCU Pin     | Description     |
| ---------- | ----------- | --------------- |
| VCC        | 3.3V        | Sensor power    |
| GND        | GND         | Ground          |
| CS         | Chip Select | SPI chip select |
| SCK        | SPI Clock   | SPI clock       |
| SDI        | MOSI        | SPI data input  |
| SDO        | MISO        | SPI data output |

---

## Installation

## Arduino Library Manager

1. Open **Arduino IDE**
2. Go to **Library Manager**
3. Search for **7Semi BME690**
4. Click **Install**

---

## Manual Installation

1. Download this repository as ZIP
2. Arduino IDE → **Sketch → Include Library → Add .ZIP Library**

---

## Example

```cpp
#include <7Semi_BME690.h>

BME69X_7Semi sensor;

void setup()
{
  Serial.begin(115200);

  if(!sensor.begin())
  {
    Serial.println("BME690 not detected");
    while(1);
  }
}

void loop()
{
  float temperature, humidity, pressure, gas;

  if(sensor.getData(temperature, humidity, pressure, gas))
  {
    Serial.print("Temperature: ");
    Serial.println(temperature);

    Serial.print("Humidity: ");
    Serial.println(humidity);

    Serial.print("Pressure: ");
    Serial.println(pressure);

    Serial.print("Gas: ");
    Serial.println(gas);
  }

  delay(2000);
}
```

---

## Library Overview

## Reading Environmental Data

```cpp
float temperature, humidity, pressure, gas;

sensor.getData(temperature, humidity, pressure, gas);

Serial.println(temperature);
Serial.println(humidity);
Serial.println(pressure);
Serial.println(gas);
```

Returns sensor measurements.

---

## Reading Temperature

```cpp
float temperature = sensor.getTemperature();
Serial.println(temperature);
```

Returns **temperature in °C**.

---

## Reading Humidity

```cpp
float humidity = sensor.getHumidity();
Serial.println(humidity);
```

Returns **relative humidity (%)**.

---

## Reading Pressure

```cpp
float pressure = sensor.getPressure();
Serial.println(pressure);
```

Returns **barometric pressure in Pa**.

---

## Reading Gas Resistance

```cpp
float gas = sensor.getGasResistance();
Serial.println(gas);
```

Returns **gas resistance value in ohms**.

---

## Sensor Configuration

## Oversampling Configuration

```cpp
sensor.setSampling(OS8, OS2, OS4, FILTER_3);
```

Parameters:

```cpp
Temperature oversampling
Humidity oversampling
Pressure oversampling
Filter configuration
```

Oversampling options:

```cpp
SKIPPED
OS1
OS2
OS4
OS8
OS16
```

---

## IIR Filter Configuration

Available filter settings:

```cpp
FILTER_OFF
FILTER_1
FILTER_3
FILTER_7
FILTER_15
FILTER_31
FILTER_63
FILTER_127
```

Filters help reduce measurement noise.

---

## Heater Configuration

The BME690 contains a **gas sensing heater** that can be configured for different gas measurement conditions.

Example:

```cpp
sensor.setHeater(true, 300, 100);
```

Parameters:

```cpp
enable heater
heater temperature (°C)
heater duration (ms)
```

---

## Heater Profiles

Multiple heater profiles can be stored for advanced gas sensing.

Example:

```cpp
sensor.setHeaterProfile(0, 200, 100);
sensor.setHeaterProfile(1, 300, 150);
```

Profiles allow different heater configurations during operation.

---

## Measurement Modes

### Forced Mode

Triggers a single measurement.

```cpp
sensor.setForcedMode();
```

---

### Parallel Mode

Used for advanced gas sensing measurements.

```cpp
BME690_data data[3];
uint8_t fields;

sensor.readParallel(data, fields);
```

---

## Example Applications

Typical applications include:

* Air quality monitoring
* Indoor environmental monitoring
* Smart home sensors
* IoT environmental sensing
* Weather stations
* Industrial environmental monitoring

---

## License

MIT License

## Notice

This library includes the **Bosch BME690 Sensor API**.

Bosch Sensortec retains copyright ownership of the original driver.

The 7Semi library provides a C++ wrapper and additional functionality
for Arduino platforms.

### Original driver

- Bosch Sensortec BME690 Sensor API
- https://github.com/boschsensortec/BME690_SensorAPI

---

## Author

7Semi
