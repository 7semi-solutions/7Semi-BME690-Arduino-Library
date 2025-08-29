#include "7semi_BME690.h"

BME690_7semi::BME690_7semi(uint8_t address_or_cs, InterfaceMode mode)
  : _mode(mode), _i2cAddress(0x77), _csPin(10), _wire(&Wire), _spi(&SPI) {
  if (mode == MODE_I2C)
    _i2cAddress = address_or_cs;
  else
    _csPin = address_or_cs;
}

bool BME690_7semi::begin(TwoWire &wirePort, SPIClass &spiPort) {
  _wire = &wirePort;
  _spi = &spiPort;

  if (_mode == MODE_I2C) {
    _wire->begin();
    dev.intf = BME69X_I2C_INTF;
    dev.read = i2cRead;
    dev.write = i2cWrite;
    dev.intf_ptr = &_i2cAddress;
  } else {
    pinMode(_csPin, OUTPUT);
    digitalWrite(_csPin, HIGH);
    _spi->begin();
    dev.intf = BME69X_SPI_INTF;
    dev.read = spiRead;
    dev.write = spiWrite;
    dev.intf_ptr = this; // Pass whole class to access _spi and _csPin
  }

  dev.delay_us = delayUs;

  if (bme69x_init(&dev) != BME69X_OK || dev.chip_id != BME69X_CHIP_ID)
    return false;

  bme69x_soft_reset(&dev);
  bme69x_set_op_mode(BME69X_SLEEP_MODE, &dev);

  conf.os_temp = BME69X_OS_8X;
  conf.os_pres = BME69X_OS_4X;
  conf.os_hum  = BME69X_OS_8X;
  conf.filter  = BME69X_FILTER_OFF;
  conf.odr     = BME69X_ODR_NONE;

  bme69x_set_conf(&conf, &dev);

  heatr_conf.enable = BME69X_ENABLE;
  heatr_conf.heatr_temp = 300;
  heatr_conf.heatr_dur  = 200;

  return (bme69x_set_heatr_conf(BME69X_FORCED_MODE, &heatr_conf, &dev) == BME69X_OK);
}

bool BME690_7semi::readSensorData() {
  if (bme69x_set_op_mode(BME69X_FORCED_MODE, &dev) != BME69X_OK) return false;

  dev.delay_us(bme69x_get_meas_dur(BME69X_FORCED_MODE, &conf, &dev), dev.intf_ptr);
  delay(100);

  if (bme69x_get_data(BME69X_FORCED_MODE, &data, &n_data, &dev) != BME69X_OK) return false;

  temperature = data.temperature;
  pressure = data.pressure / 100.0;
  humidity = data.humidity;
  gas_resistance = data.gas_resistance;

  return true;
}

float BME690_7semi::getTemperature()      { return temperature; }
float BME690_7semi::getPressure()         { return pressure; }
float BME690_7semi::getHumidity()         { return humidity; }
float BME690_7semi::getCorrectedHumidity(float offset) { return humidity + offset; }
float BME690_7semi::getGasResistance()    { return gas_resistance; }

// I2C
int8_t BME690_7semi::i2cRead(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr) {
  uint8_t addr = *(uint8_t*)intf_ptr;
  Wire.beginTransmission(addr);
  Wire.write(reg_addr);
  Wire.endTransmission(false);
  Wire.requestFrom(addr, (uint8_t)len);
  for (uint32_t i = 0; i < len && Wire.available(); i++) reg_data[i] = Wire.read();
  return 0;
}

int8_t BME690_7semi::i2cWrite(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr) {
  uint8_t addr = *(uint8_t*)intf_ptr;
  Wire.beginTransmission(addr);
  Wire.write(reg_addr);
  for (uint32_t i = 0; i < len; i++) Wire.write(reg_data[i]);
  return Wire.endTransmission();
}

// SPI
int8_t BME690_7semi::spiRead(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr) {
  BME690_7semi *self = (BME690_7semi*)intf_ptr;
  reg_addr |= 0x80;

  self->_spi->beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  digitalWrite(self->_csPin, LOW);
  self->_spi->transfer(reg_addr);
  for (uint32_t i = 0; i < len; i++) {
    reg_data[i] = self->_spi->transfer(0x00);
  }
  digitalWrite(self->_csPin, HIGH);
  self->_spi->endTransaction();
  return 0;
}

int8_t BME690_7semi::spiWrite(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr) {
  BME690_7semi *self = (BME690_7semi*)intf_ptr;
  reg_addr &= 0x7F;

  self->_spi->beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  digitalWrite(self->_csPin, LOW);
  self->_spi->transfer(reg_addr);
  for (uint32_t i = 0; i < len; i++) {
    self->_spi->transfer(reg_data[i]);
  }
  digitalWrite(self->_csPin, HIGH);
  self->_spi->endTransaction();
  return 0;
}

void BME690_7semi::delayUs(uint32_t period, void *intf_ptr) {
  delayMicroseconds(period);
}
