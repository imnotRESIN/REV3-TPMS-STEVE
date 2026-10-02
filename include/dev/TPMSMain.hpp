#ifndef TPMS_MAIN_BOARD_HPP
#define TPMS_MAIN_BOARD_HPP

#include <co_core.h>
#include <cstdint>
#include "TPMSBoard.hpp"
#include <core/io/CANDevice.hpp>
#include <core/io/CANOpenMacros.hpp>
#include <core/io/pin.hpp>
#include <core/utils/log.hpp>

// Clang was removed because it adds unnecessary tabs in front of the following macros.
// clang-format off
//TODO: REMOVE ONCE SDOs ARE IN EVT-CORE!!!!
//Temporary fix for CAN SDO server to process requests!!!!
#define SDO_CONFIGURATION_1200                          \
    {                                                   \
        /* Communication Object SDO Server */           \
        .Key  = CO_KEY(0x1200, 0x00, CO_OBJ_D___R_),    \
        .Type = CO_TUNSIGNED32,                         \
        .Data = (CO_DATA) 0x02,                         \
    },                                                  \
    {                                                   \
        /* SDO Server Request COBID */                  \
        .Key  = CO_KEY(0x1200, 0x01, CO_OBJ_DN__R_),    \
        .Type = CO_TUNSIGNED32,                         \
        .Data = (CO_DATA) CO_COBID_SDO_REQUEST(),       \
    },                                                  \
    { /* SDO Server Response COBID */                   \
        .Key  = CO_KEY(0x1200, 0x02, CO_OBJ_DN__R_),    \
        .Type = CO_TUNSIGNED32,                         \
        .Data = (CO_DATA) CO_COBID_SDO_RESPONSE(),      \
    }
//TODO: REMOVE ONCE RPDO FIX IS IN EVT-CORE!!!!
//Temporary fix for RPDOs being mapped to the same data as TPDOs!!!!
#define RECEIVE_PDO_MAPPING_ENTRY_16XX(RPDO_NUMBER, SUB_INDEX, DATA_SIZE)             \
    {                                                                                 \
        .Key  = CO_KEY(0x1600 + RPDO_NUMBER, SUB_INDEX, CO_OBJ_D___R_),               \
        .Type = CO_TUNSIGNED32,                                                       \
        .Data = (CO_DATA) CO_LINK(0x2200 + RPDO_NUMBER, 0x00 + SUB_INDEX, DATA_SIZE), \
    }
// clang-format on

namespace dev = core::dev;
namespace log = core::log;
namespace io  = core::io;

namespace TPMS {
/**
 * Main board class for the Tire Pressure Monitor System (TPMS). Holds the object dictionary and handles updating the
 * pressure sensor values.
 */
class TPMSMain
    : public TPMS<TPMSMain>, public CANDevice {
public:
    static constexpr io::Pin UART2_CTS = io::Pin::PA_0;
    static constexpr io::Pin UART2_RTS = io::Pin::PA_1;
    static constexpr io::Pin UART2_TX = io::Pin::PA_2;
    static constexpr io::Pin UART2_RX = io::Pin::PA_3;

    static constexpr io::Pin CAN_RX = io::Pin::PA_11;
    static constexpr io::Pin CAN_TX = io::Pin::PA_12;

    /**
     * Construct a TPMS instance
     *
     * @param sensorPressures An array of sensor pressure values updated by each pressure sensor instance
     */
    using Base = TPMS;
    using BoardID = BoardID;

    TPMSMain(const BoardID& boardID, uint32_t parameter1, uint32_t parameter2);

    /**
     * Update pressure values
     */
    void processOrder();

    /**
     * Get the contained sample data
     *
     * @return The value of the sample data
     */
    uint32_t getSampleData();

    /**
     * Get a pointer to the start of the object dictionary
     *
     * @return Pointer to the start of the object dictionary
     */
    CO_OBJ_T* getObjectDictionary() override;

    /**
     * Get the number of elements in the object dictionary.
     *
     * @return The number of elements in the object dictionary
     */
    uint8_t getNumElements() override;

    /**
     * Get the device's node ID
     *
     * @return The node ID of the can device.
     */
    uint8_t getNodeID() override;

    /**
     * Update pressure values
     */
    void process();

    /**
     * Set current NMT mode
     *
     * @param newMode New NMT mode
     */
    void setMode(CO_MODE newMode);

private:

    uint32_t parameter1_;
    uint32_t parameter2_;

    void sendRequest();
    void receiveResponse();
    void processResponse();

    /**
     * The node ID used to identify the device on the CAN network.
     */
    static constexpr uint8_t NODE_ID = 1;

    /**
     * This sample data will be exposed over CAN through the object
     * dictionary. The address of the variable will be included in the
     * object dictionary and can be updated via SDO via a CANopen client.
     * This device will then broadcast the value via a triggered PDO.
     */
    uint32_t sampleData;

    /**
     * Have to know the size of the object dictionary for initialization
     * process.
     */
    static constexpr uint8_t OBJECT_DICTIONARY_SIZE = 21;

    /**
     * The object dictionary itself. Will be populated by this object during
     * construction.
     *
     * The plus one is for the special "end of dictionary" marker.
     */
    CO_OBJ_T objectDictionary[OBJECT_DICTIONARY_SIZE + 1] = {
        MANDATORY_IDENTIFICATION_ENTRIES_1000_1014,
        IDENTITY_OBJECT_1018,
        SDO_CONFIGURATION_1200,
        TRANSMIT_PDO_SETTINGS_OBJECT_18XX(0, TRANSMIT_PDO_TRIGGER_TIMER, TRANSMIT_PDO_INHIBIT_TIME_DISABLE, 200),

        TRANSMIT_PDO_MAPPING_START_KEY_1AXX(0, 1),
        TRANSMIT_PDO_MAPPING_ENTRY_1AXX(0, 0x01, PDO_MAPPING_UNSIGNED8),

        DATA_LINK_START_KEY_21XX(0, 1),
        DATA_LINK_21XX(0, 1, CO_TUNSIGNED32, &sampleData),

        // End of dictionary marker
        CO_OBJ_DICT_ENDMARK,
    };
    /** Current NMT Mode */
    CO_MODE mode = CO_PREOP;
    };
}

#endif // TPMS_MAIN_BOARD_HPP