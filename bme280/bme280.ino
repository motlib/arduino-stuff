/* -*- mode:c -*- */

/* Need to use Adafruit_Sensors and Adafruit_BME280 libraries. */
#include "bme280.h"
/* Need LowPower library to put controller to sleep. */
//#include <LowPower.h>
#include "json_helper.h"

/**
 * I2C address epends on sensor module, either 0x76 or 0x77 for BME280.
 */
#define BME280_ADDR 0x76

/* Use values from LowPower.h */
#define SLEEP_TIME SLEEP_4S

/* Sensor instance. */
static BME280 bme; 


void setup() {
    Serial.begin(38400);

    if (!bme.begin(BME280_ADDR)) {
        JSON_DICT_START;
        JSON_DICT_STR("error", "Sensor not found");
        JSON_DICT_END;

        while (1);
    }

    bme.setSampling(
        BME280::MODE_NORMAL,  /* operation mode */
        BME280::SAMPLING_X16, /* temp */
        BME280::SAMPLING_X16, /* pres */
        BME280::SAMPLING_X16, /* hum */
        BME280::FILTER_X4,    /* pres filter */
        BME280::STANDBY_MS_0_5);

    /* Read first time to discard invalid values. */
    bme.sample();
}


void loop() {
    bme.sample();

    JSON_DICT_START;
    JSON_DICT_FLOAT("t", bme.getTemperature());
    JSON_SEP;
    JSON_DICT_FLOAT("p", bme.getPressure());
    JSON_SEP;
    JSON_DICT_FLOAT("h", bme.getHumidity());
    JSON_DICT_END;
    
    /* Wait before power-down to send out serial data. */
    Serial.flush();
    
#if defined(ARDUINO_AVR_PROMICRO)
    /* Problem with bootloader and USB based CDC connection. So do not go to
     * sleep, but just delay. */
    delay(1000);
#elif defined(ARDUINO_AVR_PRO)

    /* Go to low-power sleep mode. */
    LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_OFF);
#endif
}
