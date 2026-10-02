#ifndef TPMS_BOARD_HPP
#define TPMS_BOARD_HPP

#include <co_core.h>
#include <cstdint>
#include <cstddef>
#include <tuple>
#include <type_traits>
#include <array>
#include <core/io/pin.hpp>
#include <core/utils/log.hpp>

namespace dev = core::dev;
namespace log = core::log;
namespace io  = core::io;

namespace TPMS {
/**
 * TPMS static dispatcher declaration
 */
template<typename... TPMSBoards>
class TPMS;

/**
 * CRTP specialization for inheritance by the main and auxiliary boards
 */
template <typename TPMSBoard>
class TPMS<TPMSBoard>
{
public:

  using BoardID = std::array<uint32_t, 3>;


  /**
   * Construct an individual TPMS board.
   *
   * @param boardID Unique ID assigned to this physical board.
   */
  explicit TPMS(const BoardID& boardID);


  /**
   * Execute the derived TPMS board's process order.
   *
   * The actual sequence is defined by the derived class's
   * processOrder() function.
   */
  void process();


  /**
   * Send data over the Bluetooth/UART interface.
   *
   * @param data Pointer to the data to transmit.
   * @param length Number of bytes to transmit.
   *
   * @return true if transmission succeeds.
   */
  bool send(const uint8_t* data, std::size_t length);


  /**
   * Receive data from the Bluetooth/UART interface.
   *
   * @param data Buffer into which received data is placed.
   * @param length Number of bytes to receive.
   *
   * @return true if reception succeeds.
   */
  bool receive(uint8_t* data, std::size_t length);


  /**
   * Get the configured ID of this TPMS board.
   */
  const BoardID& getBoardID() const;


  /**
   * Get the unique ID of the physical STM32.
   */
  static BoardID getCurrentBoardID();


protected:

  BoardID boardID_;
};

/**
 * Main board class for the Tire Pressure Monitor System (TPMS). Drives the main and auxiliary boards
 */
template <typename... TPMSBoards>
class TPMS
{
public:

  /**
   * Construct the TPMS dispatcher.
   *
   * Any number of TPMS board instances may be supplied.
   *
   * Example:
   *
   *     TPMS<TPMSMain, TPMSAux, TPMSAux>
   *         tpms(mainBoard, frontBoard, rearBoard);
   */
  explicit TPMS(TPMSBoards&... tpmsBoards);


  /**
   * Determine which TPMS board corresponds to the
   * current physical STM32 and execute its process().
   *
   * @return 0 if a matching TPMS board was found.
   * @return 1 if no matching board was found.
   */
  int process();


private:

  /**
   * References to all available TPMS board instances.
   */
  std::tuple<TPMSBoards&...> tpmsBoards_;


  /**
   * Get the unique ID of the physical STM32.
   */
  static std::array<uint32_t, 3>
  getCurrentBoardID();


  /**
   * Compare one TPMS board's ID against the current
   *        physical STM32 ID.
   */
  template <typename TPMSBoard>
  void processTPMSBoard(const std::array<uint32_t, 3>& currentID, TPMSBoard& tpmsBoard, bool& found);
};

}

#include <TPMSBoard.tpp>
#endif // TPMS_BOARD_HPP