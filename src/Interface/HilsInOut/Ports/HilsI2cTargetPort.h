/**
 * @file HilsI2cTargetPort.h
 * @brief Class to control I2C-USB converter for the target(device) side from COM port
 */

#pragma once
#include <map>

#include "HilsUartPort.h"

const int kDefaultCmdSize = 0xff;  //!< Default command size
const int kDefaultTxSize = 0xff;   //!< Default TX size

/**
 * @class HilsI2cTargetPort
 * @brief Class to control I2C-USB converter for the target(device) side from COM port
 * @details This class has a register to store data for S2E. It is different from the register inside the converter.
 * We have Write/Read command for the register for S2E and Send/Receive command for the register in the converter.
 */
class HilsI2cTargetPort : public HilsUartPort {
 public:
  /**
   * @fn HilsI2cTargetPort
   * @brief Constructor
   * @param [in] port_id: COM port ID
   */
  HilsI2cTargetPort(const unsigned int port_id);
  /**
   * @fn HilsI2cTargetPort
   * @brief Constructor
   * @param [in] port_id: COM port number connected to the I2C-USB converter
   * @param [in] max_register_number: Maximum register number
   */
  HilsI2cTargetPort(const unsigned int port_id, const unsigned char max_register_number);
  /**
   * @fn ~HilsI2cTargetPort
   * @brief Destructor
   */
  ~HilsI2cTargetPort();

  /**
   * @fn RegisterDevice
   * @brief Register the device as an I2C port.
   */
  void RegisterDevice();

  /**
   * @fn WriteRegister
   * @brief Set the register address to write a value in the next step
   * @param [in] reg_addr: Register address to write a value in the next step
   */
  int WriteRegister(const unsigned char reg_addr);
  /**
   * @fn WriteRegister
   * @brief Write a value in the register of this device
   * @param [in] reg_addr: Register address of this device
   * @param [in] value: 1 Byte value
   */
  int WriteRegister(const unsigned char reg_addr, const unsigned char value);

  /**
   * @fn ReadRegister
   * @brief Read the register value of the target device. The register address is used as the previous accessed address
   * @param [in] reg_addr: Register address of the target device
   * @return Read data
   */
  unsigned char ReadRegister(const unsigned char reg_addr);
  /**
   * @fn ReadCommand
   * @brief Read command requested from the COM port to the component
   * @param [out] rx_data: Data to the OBC
   * @param [in] length: Length of the rx_data
   * @return Length or zero when an error happened
   */
  int ReadCommand(unsigned char* rx_data, const unsigned int length);

  /**
   * @fn Receive
   * @brief Receive data from the I2C-USB converter and store the data to the register.
   * @return -1: an error happened, others: length of received data
   */
  int Receive();
  /**
   * @fn Send
   * @brief Send data in the register to the I2C-USB converter.
   * @param [in] len: Length of the data
   * @return -1: an error happened, others: length of sent data
   */
  int Send(const unsigned char len);
  /**
   * @fn GetStoredFrameCounter
   * @brief Receive stored frame counter
   */
  int GetStoredFrameCounter();

 private:
  unsigned char max_register_number_ = 0xff;  //!< Maximum register number
  unsigned char saved_reg_addr_ = 0x00;       //!< Saved register address
  unsigned int stored_frame_counter_ = 0;     //!< Send a few frames of telemetry to the converter in advance.

  /** @brief Device register: < register address, value>  **/
  std::map<unsigned char, unsigned char> device_registers_;

  /** @brief Buffer for the command from COM port : <cmd_buffer_length, value>  **/
  std::map<unsigned char, unsigned char> cmd_buffer_;
};
