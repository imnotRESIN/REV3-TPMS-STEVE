#ifndef ADXL345_HPP
#define ADXL345_HPP

#include <core/io/SPI.hpp>

#define DATAX0 0x32
#define DATAY0 0x34
#define DATAZ0 0x36
#define ADXL345_REG_POWER_CTRL 0x2D

namespace io = core::io;

namespace rampup {

/**
 * Handles reading acceleration data from the ADXL345 accelerometer.
 * Datasheet: datasheets/adxl345.pdf
 */
class ADXL345 {
public:
    // Public methods
    /**
     * Accelerometer constructor
     *
     * @param spi used to read acceleration data
     * */
    ADXL345(io::SPI& spi);

    /**
     * Reads the acceleration from the accelerometer
     *
     * @param[out] accX reading in x-axis acceleration in milli-gs (G-Force * 1000)
     * @param[out] accY reading in y-axis acceleration in milli-gs (G-Force * 1000)
     * @param[out] accZ reading in z-axis acceleration in milli-gs (G-Force * 1000)
     * @return SPIStatus indicating the success or failure of the read.
     */
    io::SPI::SPIStatus readAcc(uint16_t& accX, uint16_t& accY, uint16_t& accZ);

private:
    // Private member variables
    /** SPI instance */
    io::SPI& spi;
};

} // namespace rampup

#endif // ADXL345_HPP
