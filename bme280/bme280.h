/* -*- mode:c++ -*- */

#ifndef BME280_H
#define BME280_H

#if (ARDUINO >= 100)
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include "wirewrap.h"
#include "bme280_defs.h"


/**
 * Default I2C sensor address.
 */
#define BME280_ADDRESS                (0x77)



class BME280
{
public:
    enum sensor_sampling {
        SAMPLING_NONE = 0b000,
        SAMPLING_X1   = 0b001,
        SAMPLING_X2   = 0b010,
        SAMPLING_X4   = 0b011,
        SAMPLING_X8   = 0b100,
        SAMPLING_X16  = 0b101
    };
    
    enum sensor_mode {
        MODE_SLEEP  = 0b00,
        MODE_FORCED = 0b01,
        MODE_NORMAL = 0b11
    };
    
    enum sensor_filter {
        FILTER_OFF = 0b000,
        FILTER_X2  = 0b001,
        FILTER_X4  = 0b010,
        FILTER_X8  = 0b011,
        FILTER_X16 = 0b100
    };
    
    /* standby durations in ms */
    enum standby_duration {
        STANDBY_MS_0_5  = 0b000,
        STANDBY_MS_10   = 0b110,
        STANDBY_MS_20   = 0b111,
        STANDBY_MS_62_5 = 0b001,
        STANDBY_MS_125  = 0b010,
        STANDBY_MS_250  = 0b011,
        STANDBY_MS_500  = 0b100,
        STANDBY_MS_1000 = 0b101
    };
    
    BME280(I2CWireWrap *dev);
    
    /**
     * Initialise the sensor.
     *
     * This reads the chip id and the calibration data from the sensor. If
     * reading the chip ID does not return the expected value BME280_CHIP_ID,
     * the function returns false.
     *
     * @returns True if the initialization was successful.
     */
    bool begin();

    
    void setSampling(sensor_mode mode              = MODE_NORMAL,
                     sensor_sampling tempSampling  = SAMPLING_X16,
                     sensor_sampling pressSampling = SAMPLING_X16,
                     sensor_sampling humSampling   = SAMPLING_X16,
                     sensor_filter filter          = FILTER_OFF,
                     standby_duration duration     = STANDBY_MS_0_5
        );
                   
    void takeForcedMeasurement();
    
    void sample();
    float getTemperature();
    float getPressure();
    float getHumidity();
        
private:
    void readCoefficients(void);
    bool isReadingCalibration(void);
    void readTemperature(void);
    void readPressure(void);
    void readHumidity(void);
    
    uint8_t   _i2caddr;
    int32_t   _sensorID;
    int32_t   t_fine;
    
    float _temp;
    float _pres;
    float _hum;
    
    I2CWireWrap *_dev;
    
    bme280_calib_data _bme280_calib;
    
    /** The config register type. */
    struct ConfigRegister
    {
        /** Inactive duration (standby time) in normal mode. */
        standby_duration t_sb;
        
        /** Filter settings. */
        sensor_filter filter;

        /** SPI 3 wire enable. */
        uint8_t spi3w_en;
        
        uint8_t get()
        {
            return (t_sb << 5) | (filter << 3) | spi3w_en;
        }
    };
    ConfigRegister _configReg;

        
    /** The ctrl_meas register. */
    struct CtrlMeasRegister
    {
        /** Temperature oversampling setting. */
        sensor_sampling osrs_t;

        /** Pressure oversampling. */
        sensor_sampling osrs_p;

        /** Device mode. */
        sensor_mode mode;

        uint8_t get()
        {
            return (osrs_t << 5) | (osrs_p << 3) | mode;
        }
    };
    CtrlMeasRegister _measReg;

        
    /** The ctrl_hum register. */
    struct CtrlHumRegister
    {
        /** Humidity oversampling. */
        sensor_sampling osrs_h;
        
        uint8_t get()
        {
            return (osrs_h);
        }
    };
    CtrlHumRegister _humReg;
};

#endif /* BME280_H */
