/**
 *
 * @file
 *
 * This is a library for the BME280 humidity, temperature and pressure
 * sensor. It is based on the BM280 library by Adafruit.
 *
 * The library is simplified, so it uses less resources (time- and memory-wise).
 */

#include "bme280.h"


BME280::BME280(I2CWireWrap *dev)
{
    _dev = dev;
}


/**
 * Initialise the sensor.
 *
 * This reads the chip id and the calibration data from the sensor. If reading
 * the chip ID does not return the expected value BME280_CHIP_ID, the function
 * returns false.
 */
bool BME280::begin()
{
    // check if sensor, i.e. the chip ID is correct
    if (_dev->read8(BME280_REGISTER_CHIPID) != BME280_CHIP_ID)
    {
        return false;
    }

    /* Reset the device using soft-reset. This makes sure the IIR is off,
     * etc. */
    _dev->write8(BME280_REGISTER_SOFTRESET, BME280_RESET_CMD);

    /* Wait for chip to wake up. */
    delay(BME280_WAKEUP_TIME_MS);

    /* If chip is still reading calibration, delay. */
    while (isReadingCalibration())
    {
          delay(BME280_CAL_READ_WAIT_MS);
    }

    /* Read trimming parameters, see DS 4.2.2. */
    readCoefficients();

    /* Use default sampling configuration. */
    setSampling();

    return true;
}


/**
 * @brief Setup sensor with given parameters / settings.
 *
 *
 * This is simply a overload to the normal begin()-function, so SPI users don't
 * get confused about the library requiring an address.
 */
void BME280::setSampling(sensor_mode       mode,
		 sensor_sampling   tempSampling,
		 sensor_sampling   pressSampling,
		 sensor_sampling   humSampling,
		 sensor_filter     filter,
		 standby_duration  duration) {
    _measReg.mode     = mode;
    _measReg.osrs_t   = tempSampling;
    _measReg.osrs_p   = pressSampling;


    _humReg.osrs_h    = humSampling;
    _configReg.filter = filter;
    _configReg.t_sb   = duration;

    /* You must make sure to also set REGISTER_CONTROL after setting the
     * CONTROLHUMID register, otherwise the values won't be applied (see DS
     * 5.4.3) */
    _dev->write8(BME280_REGISTER_CONTROLHUMID, _humReg.get());
    _dev->write8(BME280_REGISTER_CONFIG, _configReg.get());
    _dev->write8(BME280_REGISTER_CONTROL, _measReg.get());
}


/**************************************************************************/
/*!
    @brief  Take a new measurement (only possible in forced mode)
*/
/**************************************************************************/
void BME280::takeForcedMeasurement()
{
    // If we are in forced mode, the BME sensor goes back to sleep after each
    // measurement and we need to set it to forced mode once at this point, so
    // it will take the next measurement and then return to sleep again.
    // In normal mode simply does new measurements periodically.
    if (_measReg.mode == MODE_FORCED) {
        // set to forced mode, i.e. "take next measurement"
        _dev->write8(BME280_REGISTER_CONTROL, _measReg.get());
        // wait until measurement has been completed, otherwise we would read
        // the values from the last measurement
        while(_dev->read8(BME280_REGISTER_STATUS) & 0x08)
        {
            delay(1);
        }
    }
}


/**
 * @brief Reads the factory-set coefficients
 */
void BME280::readCoefficients(void)
{
    _bme280_calib.dig_T1 = _dev->read16_LE(BME280_REGISTER_DIG_T1);
    _bme280_calib.dig_T2 = _dev->readS16_LE(BME280_REGISTER_DIG_T2);
    _bme280_calib.dig_T3 = _dev->readS16_LE(BME280_REGISTER_DIG_T3);
    _bme280_calib.dig_P1 = _dev->read16_LE(BME280_REGISTER_DIG_P1);
    _bme280_calib.dig_P2 = _dev->readS16_LE(BME280_REGISTER_DIG_P2);
    _bme280_calib.dig_P3 = _dev->readS16_LE(BME280_REGISTER_DIG_P3);
    _bme280_calib.dig_P4 = _dev->readS16_LE(BME280_REGISTER_DIG_P4);
    _bme280_calib.dig_P5 = _dev->readS16_LE(BME280_REGISTER_DIG_P5);
    _bme280_calib.dig_P6 = _dev->readS16_LE(BME280_REGISTER_DIG_P6);
    _bme280_calib.dig_P7 = _dev->readS16_LE(BME280_REGISTER_DIG_P7);
    _bme280_calib.dig_P8 = _dev->readS16_LE(BME280_REGISTER_DIG_P8);
    _bme280_calib.dig_P9 = _dev->readS16_LE(BME280_REGISTER_DIG_P9);
    _bme280_calib.dig_H1 = _dev->read8(BME280_REGISTER_DIG_H1);
    _bme280_calib.dig_H2 = _dev->readS16_LE(BME280_REGISTER_DIG_H2);
    _bme280_calib.dig_H3 = _dev->read8(BME280_REGISTER_DIG_H3);
    _bme280_calib.dig_H4 = (_dev->read8(BME280_REGISTER_DIG_H4) << 4)
        | (_dev->read8(BME280_REGISTER_DIG_H4+1) & 0xF);
    _bme280_calib.dig_H5 = (_dev->read8(BME280_REGISTER_DIG_H5+1) << 4)
        | (_dev->read8(BME280_REGISTER_DIG_H5) >> 4);
    _bme280_calib.dig_H6 = (int8_t)_dev->read8(BME280_REGISTER_DIG_H6);
}


/**
 * Check if the chip is busy reading the calibration data.
 *
 * @returns True if the chip is currently loading its calibration data.
 */
bool BME280::isReadingCalibration(void)
{
  uint8_t const rStatus = _dev->read8(BME280_REGISTER_STATUS);

  return (rStatus & (1 << 0)) != 0;
}


/**
 * Read the temperature value from the sensor.
 */
void BME280::readTemperature(void)
{
    int32_t var1, var2;

    int32_t adc_T = _dev->read24(BME280_REGISTER_TEMPDATA);
    if (adc_T == 0x800000)
    {
        // value in case temp measurement was disabled
        _temp = NAN;
        return;
    }

    adc_T >>= 4;

    var1 = ((((adc_T>>3) - ((int32_t)_bme280_calib.dig_T1 <<1))) *
            ((int32_t)_bme280_calib.dig_T2)) >> 11;

    var2 = (((((adc_T>>4) - ((int32_t)_bme280_calib.dig_T1)) *
              ((adc_T>>4) - ((int32_t)_bme280_calib.dig_T1))) >> 12) *
            ((int32_t)_bme280_calib.dig_T3)) >> 14;

    t_fine = var1 + var2;

    _temp = ((t_fine * 5 + 128) >> 8) / 100.0;
}


/**
 * Read the pressure from the sensor.
 *
 * @pre This function depends on the reading of the temperature sensor, so
 *   readTemperature() must be called before calling this function.
 */
void BME280::readPressure(void)
{
    int64_t var1, var2, p;

    int32_t adc_P = _dev->read24(BME280_REGISTER_PRESSUREDATA);
    if (adc_P == 0x800000)
    {
        // value in case pressure measurement was disabled
        _pres = NAN;
        return;
    }

    adc_P >>= 4;

    var1 = ((int64_t)t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)_bme280_calib.dig_P6;
    var2 = var2 + ((var1*(int64_t)_bme280_calib.dig_P5)<<17);
    var2 = var2 + (((int64_t)_bme280_calib.dig_P4)<<35);
    var1 = ((var1 * var1 * (int64_t)_bme280_calib.dig_P3)>>8) +
           ((var1 * (int64_t)_bme280_calib.dig_P2)<<12);
    var1 = (((((int64_t)1)<<47)+var1))*((int64_t)_bme280_calib.dig_P1)>>33;

    if (var1 == 0)
    {
        /* avoid exception caused by division by zero */
        _pres = 0.0;
        return;
    }

    p = 1048576 - adc_P;
    p = (((p<<31) - var2)*3125) / var1;
    var1 = (((int64_t)_bme280_calib.dig_P9) * (p>>13) * (p>>13)) >> 25;
    var2 = (((int64_t)_bme280_calib.dig_P8) * p) >> 19;

    p = ((p + var1 + var2) >> 8) + (((int64_t)_bme280_calib.dig_P7)<<4);

    _pres = (float)(p / 256) / 100.0;
}


/**
 * Reads the humidity from the sensor.
 *
 * @pre This function depends on the reading of the temperature sensor, so
 *   readTemperature() must be called before calling this function.
 */
void BME280::readHumidity(void)
{
    int32_t adc_H = _dev->read16(BME280_REGISTER_HUMIDDATA);
    if (adc_H == 0x8000)
    {
        // value in case humidity measurement was disabled
        _hum = NAN;
        return;
    }

    int32_t v_x1_u32r;

    v_x1_u32r = (t_fine - ((int32_t)76800));

    v_x1_u32r = (((((adc_H << 14) - (((int32_t)_bme280_calib.dig_H4) << 20) -
                    (((int32_t)_bme280_calib.dig_H5) * v_x1_u32r)) + ((int32_t)16384)) >> 15) *
                 (((((((v_x1_u32r * ((int32_t)_bme280_calib.dig_H6)) >> 10) *
                      (((v_x1_u32r * ((int32_t)_bme280_calib.dig_H3)) >> 11) + ((int32_t)32768))) >> 10) +
                    ((int32_t)2097152)) * ((int32_t)_bme280_calib.dig_H2) + 8192) >> 14));

    v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) *
                               ((int32_t)_bme280_calib.dig_H1)) >> 4));

    v_x1_u32r = (v_x1_u32r < 0) ? 0 : v_x1_u32r;
    v_x1_u32r = (v_x1_u32r > 419430400) ? 419430400 : v_x1_u32r;

    _hum = (v_x1_u32r >> 12) / 1024.0;
}


/**
 * The last sampled temperature reading from the sensor.
 *
 * Call sample() to actually read the sensor before calling this function.
 */
float BME280::getTemperature(void) {
    return _temp;
}


/**
 * The last sampled pressure reading from the sensor.
 *
 * Call sample() to actually read the sensor before calling this function.
 */
float BME280::getPressure(void) {
    return _pres;
}


/**
 * The last sampled humidity reading from the sensor.
 *
 * Call sample() to actually read the sensor before calling this function.
 */
float BME280::getHumidity(void) {
    return _hum;
}


/**
 * Sample all sensor values from the sensor.
 *
 * After calling this function, you can retrieve the sensor values by calling
 * getTemperature(), getHumidity() and getPressure().
 */
void BME280::sample(void) {
    readTemperature();
    readPressure();
    readHumidity();
}
