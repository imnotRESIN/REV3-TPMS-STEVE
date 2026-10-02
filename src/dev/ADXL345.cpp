#include <dev/ADXL345.hpp>

#include <sys/types.h>

namespace rampup {

// Implementations here
/*
 * The "spi(spi)" on line 11 is the c++ constructor initializer list.
 * This is effectively just saying to do spi (the class variable) = spi (the one passed in as a parameter)
 */

ADXL345::ADXL345(io::SPI& spi) : spi(spi) {}

io::SPI::SPIStatus ADXL345::readAcc(uint16_t& accX, uint16_t& accY, uint16_t& accZ) {
    // Create variables to hold the register variables (the DATA values defined in the ADXL345.hpp)
    uint8_t byte = 0;
    uint8_t dataX0 = DATAX0;
    uint8_t dataY0 = DATAY0;
    uint8_t dataZ0 = DATAZ0;
    uint8_t powerCTRL = ADXL345_REG_POWER_CTRL;

    // Create a buffer to hold the acceleration data from the ADXL
    uint8_t outputX[2];
    uint8_t outputY[2];
    uint8_t outputZ[2];

    // Confirm that the SPI is properly connected to the ADXL
    io::SPI::SPIStatus status = spi.readReg(0, 0x00 | 0x80, &byte);
    if (status != io::SPI::SPIStatus::OK) {
        return io::SPI::SPIStatus::ERROR;
    } else {
        spi.writeReg(0, powerCTRL, 0x08);

        int16_t combinedX;
        int16_t combinedY;
        int16_t combinedZ;

        // Do a multibyte spi read for each axis as we need 2 bytes back (acceleration register is 2 bytes)
        spi.startTransmission(0);
        spi.write(dataX0 | 0x80 | 0x40);
        spi.read(outputX, 2);
        combinedX = outputX[0];
        combinedX <<= 8;
        combinedX |= outputX[1];
        spi.write(dataY0 | 0x80 | 0x40);
        spi.read(outputY, 2);
        combinedY = outputY[0];
        combinedY <<= 8;
        combinedY |= outputY[1];
        spi.write(dataZ0 | 0x80 | 0x40);
        spi.read(outputZ, 2);
        combinedZ = outputZ[0];
        combinedZ <<= 8;
        combinedZ |= outputZ[1];
        spi.endTransmission(0);

        // Create temporary variables (u32 or u64) to do conversion math in
        int32_t xVar;
        int32_t yVar;
        int32_t zVar;

        // Convert; The raw output is 1 unit = 3.9 mg (= 0.0039 gs)
        xVar = (static_cast<int32_t>(combinedX) * 39)/(10);
        yVar = (static_cast<int32_t>(combinedY) * 39)/(10);
        zVar = (static_cast<int32_t>(combinedZ) * 39)/(10);

        // Assign the value of the converted temp to the temperature variable
        accX = static_cast<int16_t>(xVar);
        accY = static_cast<int16_t>(yVar);
        accZ = static_cast<int16_t>(zVar);
        // Return the spi read status
        return io::SPI::SPIStatus::OK; // temporary placeholder to be replaced
    }
}

} // namespace rampup
