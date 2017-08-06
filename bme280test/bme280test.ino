/* -*- mode:c -*- */

/* Need to use Adafruit_Sensors and Adafruit_BME280 libraries. */
#include <Adafruit_BME280.h>
/* Needs LowPower library to put controller to sleep. */
#include <LowPower.h>


/* Seems to depend on sensor module if it is 0x76 or 0x77. */
#define BME280_ADDR 0x76

/* Use values from LowPower.h */
#define SLEEP_TIME SLEEP_4S


typedef struct {
    float temp;
    float hum;
    float pres;
} SensorValues_T;


/* Sensor instance. */
Adafruit_BME280 bme; 


/**
 * Read the sensor values.
 */
SensorValues_T getValues() {
    SensorValues_T values;
    
    values.temp = bme.readTemperature();
    values.pres = bme.readPressure() / 100.0f;
    values.hum = bme.readHumidity();

    return values;
}


/**
 * Print result on serial interface.
 */
void printValues() {
    SensorValues_T values;

    values = getValues();

    Serial.print("{\"temperature\": ");
    Serial.print(values.temp);
    Serial.print(", \"pressure\": ");
    Serial.print(values.pres);
    Serial.print(", \"humidity\": ");
    Serial.print(values.hum);
    Serial.println("}");
}


void setup() {
    bool status;

    Serial.begin(38400);

    status = bme.begin(BME280_ADDR);
    if (!status) {
        Serial.println("{\"error\": \"Sensor not found.\"}");
        while (1);
    }

    /* Read first time to discard invalid values. */
    getValues();
}


void loop() { 
    printValues();

    /* Wait a bit before power-down, to have time to send out serial data. */
    delay(50);
    LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_OFF);
}




