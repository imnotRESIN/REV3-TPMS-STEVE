#include <dev/MS5637.hpp>

namespace TPMS {

/*
 * The "slave(slave), i2c(i2c)" on line 9 are the c++ constructor initializer list.
 * This is effectively just saying to do i2c (the class variable) = i2c (the one passed in as a parameter) and the same
 * for slave
 */
MS5637::MS5637(io::I2C& i2c, uint8_t i2cSlaveAddress) : i2cSlaveAddress(i2cSlaveAddress), i2c(i2c) {}

io::I2C::I2CStatus MS5637::readPressure(uint32_t& pressure) {
    // Create variable to hold the coefficient registers (the COEF_REG values defined in the MS5637.hpp)
    uint8_t coefREG[6] = {COEF_REG1, COEF_REG2, COEF_REG3, COEF_REG4, COEF_REG5, COEF_REG6};
    // Create variable to hold the pressure register variable (the PRES_REG value defined in the MS5637.hpp)
    uint8_t presREG = PRES_REG;
    // Create variable to hold the temperature register variable (the TEMP_REG value defined in the MS5637.hpp)
    uint8_t tempREG = TEMP_REG;
    // Create variable to hold the ADC register variable (the ADC_REG value defined in the MS5637.hpp)
    uint8_t adcREG = ADC_REG;

    // Create a buffer to hold the coefficients, pressure, and temperature from the I2C
    uint16_t coefVal[6] = {};
    uint8_t outputBuf[3];

    // Create a variable to do the calculations in (nerd stuff, don't worry about it)
    uint64_t mathVar;

    // Do a multibyte i2c read as we need 2 bytes back (each coefficient register is 2 bytes)
    for (int i = 0; i < 6; i++) {
        io::I2C::I2CStatus status = i2c.readReg(this->i2cSlaveAddress, &coefREG[i], 1, outputBuf, 2);
        if (status != io::I2C::I2CStatus::OK) {
            return io::I2C::I2CStatus::ERROR;
        }
        // combine the two bytes of sensor data
        coefVal[i] = (static_cast<uint16_t>(outputBuf[0]) << 8) | outputBuf[1];
    }

    // Begin the conversion sequence for the pressure so the ADC has data to read
    io::I2C::I2CStatus pres_status = i2c.writeReg(this->i2cSlaveAddress, presREG, 0x00);
    if (pres_status != io::I2C::I2CStatus::OK) {
        return io::I2C::I2CStatus::ERROR;
    }

    // Wait 100 ms.
    core::time::wait(100);

    // Do a multibyte i2c read as we need 3 bytes back (each pressure register is 3 bytes)
    io::I2C::I2CStatus adc_status1 = i2c.readReg(this->i2cSlaveAddress, &adcREG, 1, outputBuf, 3);
    if (adc_status1 != io::I2C::I2CStatus::OK) {
        return io::I2C::I2CStatus::ERROR;
    }
    // combine the 24-bit raw pressure value as a 32-bit variable for calculations
    uint32_t pressureRAW = (static_cast<uint32_t>(outputBuf[0]) << 16) | (static_cast<uint32_t>(outputBuf[1]) << 8) | outputBuf[2];

    // Begin the conversion sequence for the temperature so the ADC has data to read
    io::I2C::I2CStatus temp_status = i2c.writeReg(this->i2cSlaveAddress, tempREG, 0x00);
    if (temp_status != io::I2C::I2CStatus::OK) {
        return io::I2C::I2CStatus::ERROR;
    }

    // Wait 100 ms.
    core::time::wait(100);

    // Do a multibyte i2c read as we need 3 bytes back (each temperature register is 3 bytes)
    io::I2C::I2CStatus adc_status2 = i2c.readReg(this->i2cSlaveAddress, &adcREG, 1, outputBuf, 3);
    if (adc_status2 != io::I2C::I2CStatus::OK) {
        return io::I2C::I2CStatus::ERROR;
    }
    // Combine the 24-bit raw temperature value as a 32-bit variable for calculations
    uint32_t temperatureRAW = (static_cast<uint32_t>(outputBuf[0]) << 16) | (static_cast<uint32_t>(outputBuf[1]) << 8) | outputBuf[2];

    // Calculate the adjusted pressure value
    uint64_t dT = static_cast<uint64_t>(temperatureRAW)-(static_cast<uint64_t>(coefVal[1])*std::pow(2,8));
    uint64_t offset = static_cast<uint64_t>(coefVal[1])*std::pow(2,16) + (static_cast<uint64_t>(coefVal[3])*dT/(std::pow(2,7)));
    uint64_t sensitivity = static_cast<uint64_t>(coefVal[0])*std::pow(2,15) + (static_cast<uint64_t>(coefVal[2])*dT/(std::pow(2,8)));
    mathVar = ((static_cast<uint64_t>(pressureRAW)*sensitivity/(std::pow(2,21)))-offset)/(std::pow(2,13));
    pressure = static_cast<uint32_t>(mathVar);

    // Return the i2c read status only after completing all functions
    return io::I2C::I2CStatus::OK; // temporary placeholder to be replaced
}

} // namespace TPMS
