#include <dev/TPMSMain.hpp>

namespace TPMS
{

TPMSMain::TPMSMain(const BoardID& boardID, uint32_t parameter1, uint32_t parameter2)
    : Base(boardID), CANDevice(), parameter1_(parameter1), parameter2_(parameter2) {}


void TPMSMain::processOrder()
{
    sendRequest();

    receiveResponse();

    processResponse();
}


void TPMSMain::sendRequest()
{
    uint8_t request[] = {0x01};

    // Use the inherited TPMS::send() here.
    Base::send(request,sizeof(request));
}


void TPMSMain::receiveResponse()
{
    uint8_t response[64]{};

    // Use the inherited TPMS::receive() here.
    Base::receive(response,sizeof(response));

    // Store/process response as appropriate.
}


void TPMSMain::processResponse()
{
    // Decode:
    //
    //   Auxiliary board ID
    //   MS5637 I2C address
    //   Pressure
    //   Temperature
    //
    // from the response received above.
}

}  // namespace TPMS