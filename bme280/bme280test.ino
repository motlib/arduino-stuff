/* -*- mode:c -*- */

/* Need to use Adafruit_Sensors and Adafruit_BME280 libraries. */
#include <Adafruit_BME280.h>
/* Need LowPower library to put controller to sleep. */
#include <LowPower.h>


/* I2C address epends on sensor module, either 0x76 or 0x77. */
#define BME280_ADDR 0x76

/* Use values from LowPower.h */
#define SLEEP_TIME SLEEP_4S


typedef struct {
    float temp;
    float hum;
    float pres;
} SensorValues_T;


/* Sensor instance. */
static Adafruit_BME280 bme; 


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
void printValues(SensorValues_T *values) {
    Serial.print("{\"temperature\": ");
    Serial.print(values->temp);
    Serial.print(", \"pressure\": ");
    Serial.print(values->pres);
    Serial.print(", \"humidity\": ");
    Serial.print(values->hum);
    Serial.println("}");
}


void setup() {
    bool status;
    SensorValues_T values;

    Serial.begin(9600);

    status = bme.begin(BME280_ADDR);
    if (!status) {
        Serial.println("{\"error\": \"Sensor not found.\"}");
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
}


void loop() {
    SensorValues_T values;

    getValues(&values);
    printValues(&values);

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

