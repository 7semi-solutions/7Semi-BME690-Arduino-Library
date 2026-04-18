/**
 * 7Semi_BME69X Arduino Library
 *
 * Supports:
 * - BME680
 * - BME688
 * - BME69X
 *
 * This library is a wrapper around the official Bosch Sensortec
 * BME69X Sensor API.
 *
 * Bosch Sensor API:
 * https://github.com/boschsensortec/BME69X-Sensor-API
 *
 * Copyright (c) Bosch Sensortec GmbH
 *
 * Wrapper implementation:
 * Copyright (c) 2026 7Semi
 *
 * License:
 * MIT License
 */

#include "7Semi_BME690.h"

BME69X_7Semi::BME69X_7Semi() {}

/**
 * Initialize BME69X sensor using I2C
 *
 * - Configures I2C interface
 * - Verifies sensor presence
 * - Initializes Bosch driver
 * - Applies default configuration
 *
 * Parameters
 * - address   : I2C address of sensor (0x76 or 0x77)
 * - i2cPort   : Wire interface object
 * - i2cSpeed  : I2C clock speed (default 100kHz)
 * - sda       : custom SDA pin (ESP32 only)
 * - scl       : custom SCL pin (ESP32 only)
 *
 * Returns
 * - true  : sensor initialized successfully
 * - false : sensor not detected or initialization failed
 */
bool BME69X_7Semi::begin(uint8_t address, TwoWire &i2cPort, uint32_t i2cSpeed, uint8_t sda, uint8_t scl) {
  /** Store pointer to I2C interface */
  i2c = &i2cPort;

#if defined(ESP32)

  /** Initialize I2C with optional custom pins */
  if (sda != 255 && scl != 255)
    i2c->begin(sda, scl);
  else
    i2c->begin();

#else

  /** Suppress unused warnings for non-ESP32 platforms */
  (void)sda;
  (void)scl;

  /** Start I2C with default pins */
  i2c->begin();

#endif

  /** Set I2C clock speed */
  i2c->setClock(i2cSpeed);

  /**
   * Verify device responds on I2C bus
   *
   * - Sends empty transmission
   * - If device does not ACK → sensor not present
   */
  i2c->beginTransmission(address);
  if (i2c->endTransmission() != 0)
    return false;

  /** Small startup delay */
  delay(BME_STARTUP_DELAY);

  /** Mark active interface */
  // iface = I2C_BUS;

  /**
   * Configure Bosch driver I2C handle
   *
   * - Stores Wire pointer
   * - Stores device address
   */
  i2c_h.i2c = i2c;
  i2c_h.address = address;

  /**
   * Configure Bosch driver interface
   *
   * - Select I2C communication
   * - Register read/write callbacks
   * - Register delay function
   */
  BME69X_dev.intf = BME69X_I2C_INTF;
  BME69X_dev.intf_ptr = &i2c_h;
  BME69X_dev.read = i2cRead;
  BME69X_dev.write = i2cWrite;
  BME69X_dev.delay_us = delayUs;
  BME69X_dev.amb_temp = 25.00;

  /**
   * Initialize Bosch BME69X driver
   *
   * Loads calibration data from sensor.
   */
  status = bme69x_init(&BME69X_dev);
  if (status != BME69X_OK)
    return false;

  /**
   * Default sensor configuration
   *
   * - temperature oversampling : 8x
   * - pressure oversampling    : 4x
   * - humidity oversampling    : 2x
   * - IIR filter               : size 3
   * - output data rate         : disabled
   */
  conf.filter = BME69X_FILTER_SIZE_3;
  conf.odr = BME69X_ODR_NONE;
  conf.os_hum = BME69X_OS_2X;
  conf.os_pres = BME69X_OS_4X;
  conf.os_temp = BME69X_OS_8X;
  

  status = bme69x_set_conf(&conf, &BME69X_dev);
  if (status != BME69X_OK)
    return false;

  /**
   * Default heater configuration
   *
   * - heater enabled
   * - temperature : 320 °C
   * - duration    : 150 ms
   * - profile length : single step
   */
  heatr.enable = BME69X_ENABLE;
  heatr.heatr_temp = 320;
  heatr.heatr_dur = 150;
  heatr.profile_len = 1;

  bme69x_set_heatr_conf(BME69X_FORCED_MODE, &heatr, &BME69X_dev);

  /** Initialization successful */
  return true;
}



/**
 * Initialize BME69X sensor using SPI
 *
 * - Configures SPI interface
 * - Sets chip select pin
 * - Initializes Bosch driver
 *
 * Parameters
 * - csPin   : SPI chip select pin
 * - spiPort : SPI interface object
 * - spiFreq : SPI clock frequency
 * - sck     : custom SCK pin (ESP32 only)
 * - miso    : custom MISO pin (ESP32 only)
 * - mosi    : custom MOSI pin (ESP32 only)
 *
 * Returns
 * - true  : sensor initialized successfully
 * - false : initialization failed
 */
bool BME69X_7Semi::beginSPI(uint8_t csPin, SPIClass &spiPort, uint32_t spiFreq,
                            uint8_t sck, uint8_t miso, uint8_t mosi) {
  /** Store SPI interface pointer */
  spi = &spiPort;

#if defined(ESP32)

  /**
   * Initialize SPI
   *
   * Allows custom SPI pins on ESP32.
   */
  if (sck != 255 && miso != 255 && mosi != 255)
    spi->begin(sck, miso, mosi);
  else
    spi->begin();

#else

  /** Suppress unused warnings */
  (void)sck;
  (void)miso;
  (void)mosi;

  /** Start SPI with default pins */
  spi->begin();

#endif

  /**
   * Configure chip select pin
   *
   * Idle state = HIGH
   */
  pinMode(csPin, OUTPUT);
  digitalWrite(csPin, HIGH);

  /** Mark active interface */
  // iface = SPI_BUS;

  /**
   * Configure SPI handle for Bosch driver
   */
  spi_h.spi = spi;
  spi_h.csPin = csPin;
  spi_h.frequency = spiFreq;

  /**
   * Configure Bosch driver interface
   *
   * - select SPI interface
   * - register read/write callbacks
   */
  BME69X_dev.intf = BME69X_SPI_INTF;
  BME69X_dev.intf_ptr = &spi_h;
  BME69X_dev.read = spiRead;
  BME69X_dev.write = spiWrite;
  BME69X_dev.delay_us = delayUs;
  BME69X_dev.amb_temp = 25.00;

  /**
   * Initialize Bosch BME69X driver
   *
   * Loads calibration data from sensor.
   */
  status = bme69x_init(&BME69X_dev);
  if (status != BME69X_OK)
    return false;

  /**
   * Default sensor configuration
   *
   * - temperature oversampling : 8x
   * - pressure oversampling    : 4x
   * - humidity oversampling    : 2x
   * - IIR filter               : size 3
   * - output data rate         : disabled
   */
  conf.filter = BME69X_FILTER_SIZE_3;
  conf.odr = BME69X_ODR_NONE;
  conf.os_hum = BME69X_OS_2X;
  conf.os_pres = BME69X_OS_4X;
  conf.os_temp = BME69X_OS_8X;

  status = bme69x_set_conf(&conf, &BME69X_dev);
  if (status != BME69X_OK)
    return false;

  /**
   * Default heater configuration
   *
   * - heater enabled
   * - temperature : 320 °C
   * - duration    : 150 ms
   * - profile length : single step
   */
  heatr.enable = BME69X_ENABLE;
  heatr.heatr_temp = 320;
  heatr.heatr_dur = 150;
  heatr.profile_len = 1;

  bme69x_set_heatr_conf(BME69X_FORCED_MODE, &heatr, &BME69X_dev);

  /** Initialization successful */
  return true;
}

/**
 * Read sensor chip ID
 *
 * Parameters
 * - id : reference where detected chip ID will be stored
 *
 * Returns
 * - true  : valid sensor detected
 */
bool BME69X_7Semi::getChipID(uint8_t &id) {

  /**
   * Read chip ID register
   *
   * - Uses Bosch driver read callback
   * - Returns error if communication fails
   */
  id = BME69X_dev.chip_id ;

  /** Chip ID valid */
  return true;
}

/**
 * Perform sensor soft reset
 *
 * - Resets internal sensor state
 * - Reloads calibration data
 *
 * Useful when
 * - sensor becomes unresponsive
 * - configuration must be restarted
 *
 * Returns
 * - true  : reset successful
 * - false : reset failed
 */
bool BME69X_7Semi::softReset()
{
  status = bme69x_soft_reset(&BME69X_dev);

  return (status == BME69X_OK);
}



/**
 * Set sensor operation mode
 *
 * Modes supported
 * - BME69X_SLEEP_MODE
 * - BME69X_FORCED_MODE
 * - BME69X_PARALLEL_MODE
 *
 * Parameters
 * - op_mode : desired sensor mode
 *
 * Returns
 * - true  : mode set successfully
 * - false : failed to change mode
 */
bool BME69X_7Semi::setOperationMode(uint8_t op_mode)
{
  status = bme69x_set_op_mode(op_mode, &BME69X_dev);

  return (status == BME69X_OK);
}

/**
 * Read current sensor operation mode
 *
 * Parameters
 * - op_mode : reference where mode will be stored
 *
 * Returns
 * - true  : mode read successfully
 * - false : read failed
 */
bool BME69X_7Semi::getOperationMode(uint8_t &op_mode)
{
  status = bme69x_get_op_mode(&op_mode, &BME69X_dev);

  return (status == BME69X_OK);
}

/**
 * Configure heater for forced mode measurement
 *
 * - enable   : enables or disables heater
 * - temp     : heater temperature in °C
 * - duration : heater duration in milliseconds
 *
 * Notes
 * - BME69X gas sensor requires heater for gas measurement
 * - Forced mode uses a single heater profile
 *
 * Returns
 * - true  : configuration successful
 * - false : failed to set configurations
 */
bool BME69X_7Semi::setHeater(bool enable, uint16_t temp, uint16_t duration) {

  /** Configure heater parameters */
  heatr.enable = enable;
  heatr.heatr_temp = temp;
  heatr.heatr_dur = duration;
  heatr.profile_len = 1;  // single-step profile

  /** Apply configuration using Bosch API */
  status = bme69x_set_heatr_conf(BME69X_FORCED_MODE, &heatr, &BME69X_dev);

  return (status == BME69X_OK);
}

/**
 * Read current heater configuration
 *
 * Returns the heater parameters currently configured
 * inside the BME69X sensor.
 *
 * Output parameters
 * - enable   : heater enabled state
 * - temp     : heater temperature (°C)
 * - duration : heater duration (ms)
 *
 * Returns
 * - true  : heater configuration read successfully
 * - false : failed to read configuration
 */
bool BME69X_7Semi::getHeater(bool &enable, uint16_t &temp, uint16_t &duration) {

  /** Read heater configuration from sensor */
  status = bme69x_get_heatr_conf(&heatr, &BME69X_dev);

  if (status != BME69X_OK)
    return false;

  /** Copy configuration to output variables */
  enable = heatr.enable;
  temp = heatr.heatr_temp;
  duration = heatr.heatr_dur;

  return true;
}

/**
 * Configure heater profile by index
 *
 * Allows storing multiple heater profiles locally
 * and applying them individually.
 *
 * Parameters
 * - profile  : heater profile index
 * - temp     : heater temperature (°C)
 * - duration : heater duration (ms)
 * - enable   : heater enabled state
 *
 * Notes
 * - Maximum profiles defined by MAX_HEATER_PROFILES
 * - Used mainly for multi-step gas sensing applications
 *
 * Returns
 * - true  : configuration applied successfully
 * - false : invalid profile index or Bosch API error
 */
bool BME69X_7Semi::setHeaterProfile(uint8_t profile, uint16_t temp, uint16_t duration, bool enable) {

  /** Validate profile index */
  if (profile >= MAX_HEATER_PROFILES)
    return false;

  /** Store profile configuration locally */
  heaterProfiles[profile].enable = enable;
  heaterProfiles[profile].heatr_temp = temp;
  heaterProfiles[profile].heatr_dur = duration;
  heaterProfiles[profile].profile_len = 1;

  /** Apply configuration to sensor */
  status = bme69x_set_heatr_conf(BME69X_FORCED_MODE, &heaterProfiles[profile], &BME69X_dev);

  return (status == BME69X_OK);
}

/**
 * Retrieve heater profile configuration
 *
 * Reads stored heater profile parameters
 * from local profile storage.
 *
 * Parameters
 * - profile  : heater profile index
 * - temp     : returned heater temperature
 * - duration : returned heater duration
 * - enable   : returned heater enable state
 *
 * Returns
 * - true  : profile exists and values returned
 * - false : invalid profile index
 */
bool BME69X_7Semi::getHeaterProfile(uint8_t profile, uint16_t &temp, uint16_t &duration, bool &enable) {

  /** Validate profile index */
  if (profile >= MAX_HEATER_PROFILES)
    return false;

  /** Return stored profile values */
  temp = heaterProfiles[profile].heatr_temp;
  duration = heaterProfiles[profile].heatr_dur;
  enable = heaterProfiles[profile].enable;

  return true;
}

/**
 * Directly set heater configuration using Bosch API structure
 *
 * Advanced users can configure heater using the
 * native Bosch driver structure.
 *
 * Parameter
 * - conf : pointer to Bosch heater configuration structure
 *
 * Returns
 * - Bosch driver status code
 */
int8_t BME69X_7Semi::setHeater(const struct bme69x_heatr_conf *conf) {
  return bme69x_set_heatr_conf(BME69X_FORCED_MODE, conf, &BME69X_dev);
}

/**
 * Directly read heater configuration using Bosch API structure
 *
 * Parameter
 * - conf : pointer to structure where configuration will be stored
 *
 * Returns
 * - Bosch driver status code
 */
int8_t BME69X_7Semi::getHeater(struct bme69x_heatr_conf *conf) {
  return bme69x_get_heatr_conf(conf, &BME69X_dev);
}

/**
 * Configure sensor oversampling and filter settings
 *
 * - temp          : temperature oversampling setting
 * - hum           : humidity oversampling setting
 * - pres          : pressure oversampling setting
 * - filterSetting : IIR filter size
 *
 * Notes
 * - Higher oversampling improves accuracy but increases measurement time
 * - Filter helps smooth pressure readings
 *
 * Returns
 * - true  : configuration successful
 * - false : configuration failed
 */
bool BME69X_7Semi::setSampling(BME69X_sampling temp,
                               BME69X_sampling hum,
                               BME69X_sampling pres,
                               BME69X_filter filterSetting)
{
  /** Apply oversampling configuration */
  conf.os_temp = static_cast<uint8_t>(temp);
  conf.os_hum  = static_cast<uint8_t>(hum);
  conf.os_pres = static_cast<uint8_t>(pres);

  /** Apply IIR filter configuration */
  conf.filter = static_cast<uint8_t>(filterSetting);

  /** Send configuration to sensor */
  status = bme69x_set_conf(&conf, &BME69X_dev);

  return (status == BME69X_OK);
}

/**
 * Read current oversampling and filter configuration
 *
 * Output parameters
 * - temp          : temperature oversampling
 * - hum           : humidity oversampling
 * - pres          : pressure oversampling
 * - filterSetting : IIR filter size
 *
 * Returns
 * - true  : configuration read successfully
 * - false : read failed
 */
bool BME69X_7Semi::getSampling(BME69X_sampling &temp,
                               BME69X_sampling &hum,
                               BME69X_sampling &pres,
                               BME69X_filter &filterSetting)
{
  /** Read configuration from sensor */
  status = bme69x_get_conf(&conf, &BME69X_dev);

  if (status != BME69X_OK)
    return false;

  /** Copy configuration values */
  temp          = static_cast<BME69X_sampling>(conf.os_temp);
  hum           = static_cast<BME69X_sampling>(conf.os_hum);
  pres          = static_cast<BME69X_sampling>(conf.os_pres);
  filterSetting = static_cast<BME69X_filter>(conf.filter);

  return true;
}

/**
 * Set output data rate (ODR)
 *
 * - odr : measurement interval setting
 *
 * Notes
 * - Controls how often the sensor performs measurements
 * - Used mainly in continuous measurement modes
 *
 * Returns
 * - true  : configuration successful
 * - false : configuration failed
 */
bool BME69X_7Semi::setODR(BME69X_odr odr)
{
  /** Store selected output data rate */
  conf.odr = static_cast<uint8_t>(odr);

  /** Apply configuration */
  status = bme69x_set_conf(&conf, &BME69X_dev);

  return (status == BME69X_OK);
}

/**
 * Get currently configured output data rate
 *
 * Output parameter
 * - odr : current measurement interval setting
 *
 * Returns
 * - true : value returned successfully
 */
bool BME69X_7Semi::getODR(BME69X_odr &odr) const
{
  /** Return stored ODR configuration */
  odr = static_cast<BME69X_odr>(conf.odr);

  return true;
}

/**
 * Calculate measurement duration
 *
 * - op_mode  : sensor operating mode
 * - duration : returned measurement time (microseconds)
 *
 * Notes
 * - Measurement duration depends on oversampling settings
 * - Used to determine how long to wait before reading data
 *
 * Returns
 * - true : duration calculated successfully
 */
bool BME69X_7Semi::getMeasurementDuration(uint8_t op_mode, uint32_t &duration) {
  duration = bme69x_get_meas_dur(op_mode, &conf, &BME69X_dev);
  return true;
}

/**
 * Set sensor to forced measurement mode
 *
 * Notes
 * - Sensor performs one measurement cycle
 * - Sensor returns to sleep mode after measurement
 * - Used for manual measurement triggering
 *
 * Returns
 * - true  : mode set successfully
 * - false : configuration failed
 */
bool BME69X_7Semi::setForcedMode() {
  status = bme69x_set_op_mode(BME69X_FORCED_MODE, &BME69X_dev);
  return status == BME69X_OK;
}

/**
 * Start measurement (non-blocking)
 * - triggers forced mode
 * - stores timestamp
 */
bool BME69X_7Semi::startMeasurement() {
    if (bme69x_set_op_mode(BME69X_FORCED_MODE, &BME69X_dev) != BME69X_OK)
        return false;

    measurementStart = millis();
    measurementDuration = bme69x_get_meas_dur(BME69X_FORCED_MODE, &conf, &BME69X_dev) / 1000 + 1;

    return true;
}

/**
 * Check if measurement is ready
 */
bool BME69X_7Semi::isMeasurementReady() {
    return (millis() - measurementStart) >= measurementDuration;
}

/**
 * Read data after measurement is ready
 */
bool BME69X_7Semi::readMeasurement(bme69x_data &outData) {
    if (!isMeasurementReady())
        return false;

    uint8_t fields = 0;
    if (bme69x_get_data(BME69X_FORCED_MODE, &BME69X_Data, &fields, &BME69X_dev) != BME69X_OK)
        return false;

    if (fields == 0)
        return false;

    outData = BME69X_Data;
    return true;
}

/**
 * Perform measurement and return full sensor data
 *
 * Steps performed
 * - Trigger forced measurement
 * - Calculate measurement time
 * - Wait for measurement completion
 * - Read sensor data
 *
 * Output
 * - outData : structure containing all measurement values
 *
 * Returns
 * - true  : data read successfully
 * - false : measurement or read failed
 */
bool BME69X_7Semi::getData(bme69x_data &outData) {

  uint32_t measure_duration = 0;

  /** Start forced measurement */
  setForcedMode();

  /** Calculate measurement duration */
  getMeasurementDuration(BME69X_FORCED_MODE, measure_duration);

  /** Wait for measurement completion */
  delay(measure_duration / 1000 + 1);

  /** Read sensor data */
  status = bme69x_get_data(BME69X_FORCED_MODE, &BME69X_Data, &data_size, &BME69X_dev);

  if (status != BME69X_OK)
    return false;

  /** Copy result to output structure */
  outData = BME69X_Data;

  return true;
}

/**
 * Update internal sensor data
 *
 * Notes
 * - Reads data and stores it inside class variable
 * - Used internally by other helper functions
 *
 * Returns
 * - true  : data updated successfully
 * - false : measurement failed
 */
bool BME69X_7Semi::getData() {
  return getData(BME69X_Data);
}

/**
 * Read sensor data as floating values
 *
 * Output parameters
 * - t : temperature (°C)
 * - h : humidity (%)
 * - p : pressure (hPa)
 * - g : gas resistance (Ohms)
 *
 * Returns
 * - true  : values read successfully
 * - false : measurement failed
 */
bool BME69X_7Semi::getData(float &t, float &h, float &p, float &g) {

  if (!getData())
    return false;

  t = BME69X_Data.temperature;
  h = BME69X_Data.humidity;
  p = BME69X_Data.pressure / 100.0;
  g = BME69X_Data.gas_resistance;

  return true;
}

/**
 * Read temperature
 *
 * Returns
 * - temperature in °C
 * - NAN if measurement failed
 */
float BME69X_7Semi::getTemperature() {

  if (!getData())
    return NAN;

  return BME69X_Data.temperature;
}

/**
 * Read pressure
 *
 * Returns
 * - pressure in hPa
 * - NAN if measurement failed
 */
float BME69X_7Semi::getPressure() {

  if (!getData())
    return NAN;

  return BME69X_Data.pressure / 100.0;
}

/**
 * Read humidity
 *
 * Returns
 * - relative humidity (%)
 * - NAN if measurement failed
 */
float BME69X_7Semi::getHumidity() {

  if (!getData())
    return NAN;

  return BME69X_Data.humidity;
}

/**
 * Read gas resistance
 *
 * Returns
 * - gas resistance value (Ohms)
 * - NAN if measurement failed
 */
float BME69X_7Semi::getGasResistance() {

  if (!getData())
    return NAN;

  return BME69X_Data.gas_resistance;
}

/**
 * Read sensor data using forced mode
 *
 * Operation sequence
 * - Set sensor to forced measurement mode
 * - Calculate measurement duration
 * - Wait for measurement completion
 * - Read sensor data
 *
 * Parameters
 * - outData : structure where measurement results will be stored
 *
 * Returns
 * - true  : measurement successful
 * - false : measurement failed
 *
 * Notes
 * - Forced mode performs a single measurement cycle
 * - Sensor returns to sleep mode automatically after measurement
 */
bool BME69X_7Semi::readForced(bme69x_data &outData)
{
  /** Set sensor to forced mode */
  if (bme69x_set_op_mode(BME69X_FORCED_MODE, &BME69X_dev) != BME69X_OK)
    return false;

  /** Calculate measurement duration */
  uint32_t measure_duration = bme69x_get_meas_dur(BME69X_FORCED_MODE, &conf, &BME69X_dev);

  /** Wait for measurement completion */
  delay(measure_duration / 1000 + 1);  // convert µs to ms with safety margin

  /** Read measurement data */
  uint8_t n_fields_local = 0;

  if (bme69x_get_data(BME69X_FORCED_MODE, &BME69X_Data, &n_fields_local, &BME69X_dev) != BME69X_OK)
    return false;

  /** Copy result to output structure */
  outData = BME69X_Data;

  return true;
}

/**
 * Read sensor data using parallel measurement mode
 *
 * Operation sequence
 * - Switch sensor to parallel mode
 * - Calculate measurement duration
 * - Wait for measurement completion
 * - Read all available measurement fields
 *
 * Parameters
 * - dataArray : array where measurement results will be stored
 * - fields    : number of valid measurement fields returned
 *
 * Returns
 * - true  : data read successfully
 * - false : measurement failed or no data available
 *
 * Notes
 * - Parallel mode allows multiple heater profiles
 * - Useful for advanced gas sensing applications
 */
bool BME69X_7Semi::readParallel(bme69x_data *dataArray, uint8_t &fields)
{
  /** Validate output buffer */
  if (dataArray == nullptr)
    return false;

  /** Set sensor to parallel measurement mode */
  status = bme69x_set_op_mode(BME69X_PARALLEL_MODE, &BME69X_dev);

  if (status != BME69X_OK)
    return false;

  /** Calculate measurement duration */
  uint32_t meas_dur = bme69x_get_meas_dur(BME69X_PARALLEL_MODE, &conf, &BME69X_dev);

  /** Wait for measurement completion */
  delay(meas_dur / 1000 + 5);

  /** Reset field counter */
  fields = 0;

  /** Read all available measurement data */
  status = bme69x_get_data(BME69X_PARALLEL_MODE, dataArray, &fields, &BME69X_dev);

  if (status != BME69X_OK || fields == 0)
    return false;

  return true;
}

/**
 * Get last status from Bosch driver
 *
 * - returns last operation result
 * - useful for debugging failures
 */
int8_t BME69X_7Semi::getLastStatus() {
    return status;
}

/**
 * Read data from sensor using I2C
 *
 * Parameters
 * - reg      : register address to read from
 * - data     : buffer where received data will be stored
 * - len      : number of bytes to read
 * - intf_ptr : pointer to I2C handle structure
 *
 * Returns
 * - 0  : read successful
 * - -1 : transmission error
 * - -2 : received byte count mismatch
 */
int8_t BME69X_7Semi::i2cRead(uint8_t reg, uint8_t *data, uint32_t len, void *intf_ptr) {

  if (!intf_ptr) return -1;

  /** Retrieve I2C handle */
  i2c_handle *port = (i2c_handle *)intf_ptr;

  /** Send register address */
  port->i2c->beginTransmission(port->address);
  port->i2c->write(reg);

  /** Restart condition for read */
  if (port->i2c->endTransmission(false) != 0)
    return -1;

  /** Request bytes from device */
  uint8_t n = port->i2c->requestFrom(port->address, (uint8_t)len);

  /** Validate received length */
  if (n != len)
    return -2;

  /** Read incoming data */
  for (uint32_t i = 0; i < len; i++)
    data[i] = port->i2c->read();

  return 0;
}

/**
 * Write data to sensor using I2C
 *
 * Parameters
 * - reg      : register address to write
 * - data     : pointer to data buffer
 * - len      : number of bytes to write
 * - intf_ptr : pointer to I2C handle structure
 *
 * Returns
 * - transmission result
 */
int8_t BME69X_7Semi::i2cWrite(uint8_t reg, const uint8_t *data, uint32_t len, void *intf_ptr) {

  /** Retrieve I2C handle */
  i2c_handle *port = (i2c_handle *)intf_ptr;

  /** Start transmission */
  port->i2c->beginTransmission(port->address);

  /** Send register address */
  port->i2c->write(reg);

  /** Write data bytes */
  for (uint32_t i = 0; i < len; i++)
    port->i2c->write(data[i]);

  /** End transmission */
  return port->i2c->endTransmission();
}

/**
 * Read data from sensor using SPI
 *
 * Parameters
 * - reg      : register address to read
 * - data     : buffer to store received data
 * - len      : number of bytes to read
 * - intf_ptr : pointer to SPI handle structure
 *
 * Returns
 * - 0 : read successful
 */
int8_t BME69X_7Semi::spiRead(uint8_t reg, uint8_t *data, uint32_t len, void *intf_ptr)
{
  /** Retrieve SPI handle */
  spi_handle *port = (spi_handle *)intf_ptr;

  /** Start SPI transaction */
  port->spi->beginTransaction(SPISettings(port->frequency, MSBFIRST, SPI_MODE0));

  /** Select device */
  digitalWrite(port->csPin, LOW);

  /** Send register address with read flag */
  port->spi->transfer(reg | 0x80);

  /** Read requested bytes */
  for (uint32_t i = 0; i < len; i++)
    data[i] = port->spi->transfer(0);

  /** Release device */
  digitalWrite(port->csPin, HIGH);

  /** End transaction */
  port->spi->endTransaction();

  return 0;
}

/**
 * Write data to sensor using SPI
 *
 * Parameters
 * - reg      : register address to write
 * - data     : pointer to data buffer
 * - len      : number of bytes to write
 * - intf_ptr : pointer to SPI handle structure
 *
 * Returns
 * - 0 : write successful
 */
int8_t BME69X_7Semi::spiWrite(uint8_t reg, const uint8_t *data, uint32_t len, void *intf_ptr) {

  /** Retrieve SPI handle */
  spi_handle *port = (spi_handle *)intf_ptr;

  /** Configure SPI settings */
  SPISettings settings(port->frequency, MSBFIRST, SPI_MODE0);

  /** Start SPI transaction */
  port->spi->beginTransaction(settings);

  /** Select device */
  digitalWrite(port->csPin, LOW);

  /** Send register address with write flag */
  port->spi->transfer(reg & 0x7F);

  /** Send data bytes */
  for (uint32_t i = 0; i < len; i++)
    port->spi->transfer(data[i]);

  /** Release device */
  digitalWrite(port->csPin, HIGH);

  /** End SPI transaction */
  port->spi->endTransaction();

  return 0;
}

/**
 * Microsecond delay helper
 *
 * Used internally when short timing delays
 * are required during sensor communication.
 *
 * Parameters
 * - period : delay duration in microseconds
 */
void BME69X_7Semi::delayUs(uint32_t period, void *) {
  delayMicroseconds(period);
}