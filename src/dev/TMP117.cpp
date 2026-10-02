#include <dev/TMP117.hpp>

namespace rampup {

/*
 * The "slave(slave), i2c(i2c)" on line 9 are the c++ constructor initializer list.
 * This is effectively just saying to do i2c (the class variable) = i2c (the one passed in as a parameter) and the same
 * for slave
 */
TMP117::TMP117(io::I2C& i2c, uint8_t i2cSlaveAddress) : i2cSlaveAddress(i2cSlaveAddress), i2c(i2c) {}

io::I2C::I2CStatus TMP117::readTemp(uint16_t& temperature) {
    // Create variable to hold the register variable (the TEMP_REG value defined in the TMP117.hpp)
    uint8_t tempREG = TEMP_REG;

    // Create a buffer to hold the temperature from the I2C
    uint8_t outputBuf[2];

    // Do a multibyte i2c read as we need 2 bytes back (temperature register is 2 bytes)
    io::I2C::I2CStatus status = i2c.readReg(this->i2cSlaveAddress, &tempREG, 1, outputBuf, 2);
    if (status != io::I2C::I2CStatus::OK) {
        return io::I2C::I2CStatus::ERROR;
    } else {
        // Create a temporary variable (u32 or u64) to do conversion math in
        uint32_t mathVar;

        // combine the two bytes of sensor data
        uint16_t combined = outputBuf[0];
        combined <<= 8;
        combined |= outputBuf[1];

        // Convert; The raw output is 1 unit = 7.8125 m°C (= 0.0078125°C)
        mathVar = (static_cast<uint32_t>(combined) * 78125)/(10000);

        // Assign the value of the converted temp to the temperature variable
        temperature = static_cast<uint16_t>(mathVar);
        // Return the i2c read status
        return io::I2C::I2CStatus::OK; // temporary placeholder to be replaced
    }
}

} // namespace rampup
