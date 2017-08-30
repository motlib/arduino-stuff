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



class BME280 {
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

        // standby durations in ms 
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

        // The config register
        struct config {
            // inactive duration (standby time) in normal mode
            // 000 = 0.5 ms
            // 001 = 62.5 ms
            // 010 = 125 ms
            // 011 = 250 ms
            // 100 = 500 ms
            // 101 = 1000 ms
            // 110 = 10 ms
            // 111 = 20 ms
            unsigned int t_sb : 3;

            // filter settings
            // 000 = filter off
            // 001 = 2x filter
            // 010 = 4x filter
            // 011 = 8x filter
            // 100 and above = 16x filter
            unsigned int filter : 3;

            // unused - don't set
            unsigned int none : 1;
            unsigned int spi3w_en : 1;

            unsigned int get() {
                return (t_sb << 5) | (filter << 3) | spi3w_en;
            }
        };
        config _configReg;

        
        // The ctrl_meas register
        struct ctrl_meas {
            // temperature oversampling
            // 000 = skipped
            // 001 = x1
            // 010 = x2
            // 011 = x4
            // 100 = x8
            // 101 and above = x16
            unsigned int osrs_t : 3;

            // pressure oversampling
            // 000 = skipped
            // 001 = x1
            // 010 = x2
            // 011 = x4
            // 100 = x8
            // 101 and above = x16
            unsigned int osrs_p : 3;

            // device mode
            // 00       = sleep
            // 01 or 10 = forced
            // 11       = normal
            unsigned int mode : 2;

            unsigned int get() {
                return (osrs_t << 5) | (osrs_p << 3) | mode;
            }
        };
        ctrl_meas _measReg;

        
        // The ctrl_hum register
        struct ctrl_hum {
            // unused - don't set
            unsigned int none : 5;

            // pressure oversampling
            // 000 = skipped
            // 001 = x1
            // 010 = x2
            // 011 = x4
            // 100 = x8
            // 101 and above = x16
            unsigned int osrs_h : 3;

            unsigned int get() {
                return (osrs_h);
            }
        };
        ctrl_hum _humReg;
};

#endif /* BME280_H */
