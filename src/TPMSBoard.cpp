#include <TPMSBoard.hpp>

namespace TPMS {

TPMS::TPMS(int32_t* sensorPressures) {

}

CO_OBJ_T* TPMS::getObjectDictionary() {
    return &objectDictionary[0];
}

uint8_t TPMS::getNumElements() {
    return OBJECT_DICTIONARY_SIZE;
}

uint8_t TPMS::getNodeID() {
    return TPMS::NODE_ID;
}

void TPMS::process() {
    static uint32_t lastUpdate = 0;

    #ifdef EVT_CORE_LOG_ENABLE
    if (core::time::millis() - lastUpdate > 100) {
        lastUpdate = core::time::millis();
        log::LOGGER.log(log::Logger::LogLevel::DEBUG, "[%d] Updating!", lastUpdate);
        for (int i = 0; i < 2; i++) {
            log::LOGGER.log(log::Logger::LogLevel::DEBUG, "Pressure #%d: %d", i, sensorPressures[i]);
        }
    }
    #endif

    switch (mode) {
        // Auxiliary Mode
    default:
        log::LOGGER.log(log::Logger::LogLevel::ERROR, "Network Management state is not valid.");
    }
}

void TPMS::setMode(CO_MODE newMode) {
    mode = newMode;
}

} // namespace TMS