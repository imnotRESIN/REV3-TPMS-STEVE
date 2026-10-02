#ifndef MS5637_HPP
#define MS5637_HPP

#include <core/io/I2C.hpp>
#include <core/utils/time.hpp>
#include <cmath>

#define COEF_REG1 0xA2
#define COEF_REG2 0xA4
#define COEF_REG3 0xA6
#define COEF_REG4 0xA8
#define COEF_REG5 0xAA
#define COEF_REG6 0xAC
#define PRES_REG 0x40
#define TEMP_REG 0x50
#define ADC_REG 0x00

namespace io = core::io;

namespace TPMS {

/**
 * Handles reading pressure and temperature data from the MS5637-30BA pressure sensor.
 * Datasheet: datasheets/ENG_DS_MS5637-30BA_A3.pdf
 */
class MS5637 {
public:
    // Public methods
    /**
     * Pressure sensor constructor
     *
     * @param i2c used to read pressure
     * @param[in] i2cSlaveAddress address to identify the sensor on the I2C bus
     * */
    MS5637(io::I2C& i2c, uint8_t i2cSlaveAddress);

    /**
     * Reads the pressure from the pressure sensor
     *
     * @param[out] pressure reading from the I2C in millibars (Bars * 1000)
     * @return I2CStatus indicating the success or failure of the read.
     */
    io::I2C::I2CStatus readPressure(uint32_t& pressure);

private:
    /** Device ID */
    uint8_t i2cSlaveAddress;

    /** I2C instance */
    io::I2C& i2c;
};

} // namespace TPMS

#endif // MS5637_HPP
