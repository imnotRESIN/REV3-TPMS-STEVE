#ifndef TPMS_BOARD_TPP
#define TPMS_BOARD_TPP

#include <HALf3/stm32f3xx.h>
#include <HALf3/stm32f3xx_hal.h>

namespace TPMS {
/**
 * CRTP template implementation
 */
template <typename TPMSBoard>
TPMS<TPMSBoard>::TPMS(const BoardID& boardID) : boardID_(boardID) {}


template <typename TPMSBoard>
void TPMS<TPMSBoard>::process()
{
    static_cast<TPMSBoard*>(this)->processOrder();
}


template <typename TPMSBoard>
bool TPMS<TPMSBoard>::send(const uint8_t* data, std::size_t length)
{
    /*
     * Bluetooth/UART transmission implementation.
     *
     * The actual hardware communication can be implemented
     * here or delegated to the hardware communication layer.
     */

    return true;
}


template <typename TPMSBoard>
bool TPMS<TPMSBoard>::receive(uint8_t* data, std::size_t length)
{
    /*
     * Bluetooth/UART reception implementation.
     */

    return true;
}


template <typename TPMSBoard>
const typename TPMS<TPMSBoard>::BoardID&
TPMS<TPMSBoard>::getBoardID() const
{
    return boardID_;
}


template <typename TPMSBoard>
typename TPMS<TPMSBoard>::BoardID
TPMS<TPMSBoard>::getCurrentBoardID()
{
    return
    {
        HAL_GetUIDw0(),
        HAL_GetUIDw1(),
        HAL_GetUIDw2()
    };
}


/**
 * Static dispatcher implementation
 */

template <typename... TPMSBoards>
TPMS<TPMSBoards...>::TPMS(TPMSBoards&... tpmsBoards) : tpmsBoards_(tpmsBoards...) {}


template <typename... TPMSBoards>
int TPMS<TPMSBoards...>::process()
{
    const auto currentID =
        getCurrentBoardID();

    bool found = false;


    std::apply([&](auto&... tpmsBoard){(processTPMSBoard(currentID, tpmsBoard, found) || ...);}, tpmsBoards_);


    return found ? 0 : 1;
}


template <typename... TPMSBoards>
std::array<uint32_t, 3>
TPMS<TPMSBoards...>::getCurrentBoardID()
{
    return
    {
        HAL_GetUIDw0(),
        HAL_GetUIDw1(),
        HAL_GetUIDw2()
    };
}


template <typename... TPMSBoards>
template <typename TPMSBoard>
void TPMS<TPMSBoards...>::processTPMSBoard(const std::array<uint32_t, 3>& currentID, TPMSBoard& tpmsBoard, bool& found)
{
    if (found)
    {
        return;
    }


    if (currentID == tpmsBoard.getBoardID())
    {
        found = true;


        while (true)
        {
            tpmsBoard.process();
        }
    }
}

}

#endif //TPMS_BOARD_TPP