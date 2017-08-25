

/* -*- mode:c -*- */

/* Need to use Adafruit_Sensors and Adafruit_BME280 libraries. */
#include <Adafruit_BME280.h>
/* Need LowPower library to put controller to sleep. */
//#include <LowPower.h>
#include <EEPROM.h>
#include <CRC32.h>

/* I2C address epends on sensor module, either 0x76 or 0x77. */
#define BME280_ADDR 0x76

/* Use values from LowPower.h */
#define SLEEP_TIME SLEEP_4S

#define EEP_EXTREMES_ADDR 0

typedef struct {
    float temp;
    float hum;
    float pres;
} SensorValues_T;


typedef struct {
  SensorValues_T min;
  SensorValues_T max;
  uint32_t crc;
} ExtremValues_T;

bool extrem_init = false;

/* Sensor instance. */
static Adafruit_BME280 bme; 


static ExtremValues_T extrem;

CRC32 crc;

/**
 * Read the sensor values.
 *
 * Takes ca. 4 to 6ms on 8MHz Pro Micro.
 */
void getValues(SensorValues_T *values) {
    values->temp = bme.readTemperature();
    values->pres = bme.readPressure() / 100.0f;
    values->hum = bme.readHumidity();
}


/**
 * Print result on serial interface.
 */
void printValues(SensorValues_T *values, String type) {
    Serial.print(F("{\"type\": \""));
    Serial.print(type);
    Serial.print(F("\", \"temperature\": "));
    Serial.print(values->temp);
    Serial.print(F(", \"pressure\": "));
    Serial.print(values->pres);
    Serial.print(F(", \"humidity\": "));
    Serial.print(values->hum);
    Serial.println(F("}"));
}

void loadExtremesFromEeprom() {
  uint32_t crcval;
  EEPROM.get(EEP_EXTREMES_ADDR, extrem);

  crcval = crc.calculate(&extrem, sizeof(extrem) - 4);

  if(crcval != extrem.crc) {
    extrem_init = true;
  } else {
    extrem_init = true;
  }
}


void setup() {
    bool status;
    SensorValues_T values;

    Serial.begin(9600);

    status = bme.begin(BME280_ADDR);
    if (!status) {
        Serial.println(F("{\"error\": \"Sensor not found.\"}"));
        while (1);
    }

    bme.setSampling(
        Adafruit_BME280::MODE_NORMAL,  /* operation mode */
        Adafruit_BME280::SAMPLING_X16, /* temp */
        Adafruit_BME280::SAMPLING_X16, /* pres */
        Adafruit_BME280::SAMPLING_X16, /* hum */
        Adafruit_BME280::FILTER_X4,    /* pres filter */
        Adafruit_BME280::STANDBY_MS_0_5);

    /* Read first time to discard invalid values. */
    getValues(&values);
    getValues(&values);

  extrem_init = true;
}


void loop() {
    SensorValues_T values;

    getValues(&values);

    if(extrem_init == true) {
      extrem_init = false;
      extrem.min = values;
      extrem.max = values;
    } else {
      bool changed = false;
      /* Update extreme values. */
      if(extrem.min.temp > values.temp) {
        extrem.min.temp = values.temp;
        changed = true;
      }
      if(extrem.min.hum > values.hum) {
        extrem.min.hum = values.hum;
        changed = true;
      }
      if(extrem.min.pres > values.pres) {
        extrem.min.pres = values.pres;
        changed = true;
      }
      if(extrem.max.temp < values.temp) {
        extrem.max.temp = values.temp;
        changed = true;
      }
      if(extrem.max.hum < values.hum) {
        extrem.max.hum = values.hum;
        changed = true;
      }
      if(extrem.max.pres < values.pres) {
        extrem.max.pres = values.pres;
        changed = true;
      }

      if(changed == true) {
        extrem.crc = crc.calculate(&extrem, sizeof(extrem) - 4);
      }
    }
    
    printValues(&values, F("current"));
    printValues(&extrem.min, F("min"));
    printValues(&extrem.max, F("max"));

#if defined(ARDUINO_AVR_PROMICRO)
    /* Problem with bootloader and USB based CDC connection. So do not go to
     * sleep, but just delay. */
    delay(1000);
    
#elif defined(ARDUINO_AVR_PRO)
    /* Wait before power-down to send out serial data. */
    Serial.flush();

    /* Go to low-power sleep mode. */
    LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_OFF);
#endif
}

