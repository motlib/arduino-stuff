#ifndef WIREWRAP_H
#define WIREWRAP_H

#include <stdint.h>


/**
 * Wrapper for the Arduino Wire library for high-level access to read and write
 * registers of a sensor device.
 */
class I2CWireWrap {
public:
    /**
     * Initialize a new instance.
     *
     * @param addr The I2C device address to access.
     */
    I2CWireWrap(uint8_t addr);

    /**
     * Init / start access to the bus.
     */
    void begin();
    
    /**
     * Write an 8 bit value.
     */
    void write8(uint8_t reg, uint8_t value);

    /**
     * Read an 8 bit value.
     */
    uint8_t read8(uint8_t reg);

    /**
     * Reads a 16 bit value.
     */
    uint16_t read16(uint8_t reg);

    /**
     * Read an unsigned 16 bit value with byte swap.
     */
    uint16_t read16_LE(uint8_t reg);
 
    /**
     *  Reads a signed 16 bit value.
     */
    int16_t readS16(uint8_t reg);

    /**
     * Read a signed 16 bit value with byte swap.
     */    
    int16_t readS16_LE(uint8_t reg);

    /**
     * Read an unsigned 24 bit value.
     */
    uint32_t read24(uint8_t reg);

    /**
     * Read up to 32 bits over I2C.
     * 
     * @param reg The register to read.
     * @param cnt The number of bytes (1 to 4) to read.
     *
     * @returns The value read from I2C.
     */
    uint32_t read(uint8_t reg, uint8_t cnt);

private:
    uint8_t _addr;
};


#endif /* WIREWRAP_H */
