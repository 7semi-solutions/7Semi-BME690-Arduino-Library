#ifndef _7SEMI_BME690_H_
#define _7SEMI_BME690_H_

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

extern "C" {
  #include "bme69x.h"
  #include "bme69x_defs.h"
}

class BME690_7semi {
public:
  enum InterfaceMode {
    MODE_I2C,
    MODE_SPI
  };

  BME690_7semi(uint8_t address_or_cs, InterfaceMode mode = MODE_I2C);

  bool begin(TwoWire &wirePort = Wire, SPIClass &spiPort = SPI);
  bool readSensorData();

  float getTemperature();
  float getPressure();
  float getHumidity();
  float getCorrectedHumidity(float offset = 12.0);
  float getGasResistance();

private:
  // Communication
  static int8_t i2cRead(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr);
  static int8_t i2cWrite(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr);
  static int8_t spiRead(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr);
  static int8_t spiWrite(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr);
  static void delayUs(uint32_t period, void *intf_ptr);

  InterfaceMode _mode;
  uint8_t _i2cAddress;
  uint8_t _csPin;
  TwoWire *_wire;
  SPIClass *_spi;

  struct bme69x_dev dev;
  struct bme69x_conf conf;
  struct bme69x_heatr_conf heatr_conf;
  struct bme69x_data data;
  uint8_t n_data;

  float temperature, pressure, humidity, gas_resistance;
};

#endif
