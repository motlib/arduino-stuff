#include "wirewrap.h"

#include <Wire.h>

I2CWireWrap::I2CWireWrap(uint8_t addr)
{
    _addr = addr;
}


void I2CWireWrap::begin()
{
    Wire.begin();
}


void I2CWireWrap::write8(uint8_t reg, uint8_t value)
{
    Wire.beginTransmission(_addr);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission();
}


uint8_t I2CWireWrap::read8(uint8_t reg)
{
    return read(reg, 1);
}


uint16_t I2CWireWrap::read16(uint8_t reg)
{
    return (uint16_t)read(reg, 2);
}


uint16_t I2CWireWrap::read16_LE(uint8_t reg) {
    uint16_t temp = read(reg, 2);
    
    return (temp >> 8) | (temp << 8);
}


int16_t I2CWireWrap::readS16(uint8_t reg)
{
    return (int16_t)read(reg, 2);
}


int16_t I2CWireWrap::readS16_LE(uint8_t reg)
{
    return (int16_t)read16_LE(reg);
}


uint32_t I2CWireWrap::read(uint8_t reg, uint8_t cnt)
{
    uint32_t value = 0;

    Wire.beginTransmission(_addr);
    Wire.write(reg);
    Wire.endTransmission();
    Wire.requestFrom(_addr, cnt);

    for(uint8_t i = 0; i < cnt; ++i)
    {
        value <<= 8;
        value |= (uint8_t)Wire.read();
    }

    return value;
}


/**
 *   Reads a 24 bit value over I2C
 */
uint32_t I2CWireWrap::read24(uint8_t reg)
{
    return read(reg, 3);
}
