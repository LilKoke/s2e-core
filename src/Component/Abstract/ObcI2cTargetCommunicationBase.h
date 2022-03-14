#pragma once
#include "../CDH/OBC.h"
#include "../../Interface/HilsInOut/HilsPortManager.h"
#include "ObcCommunicationBase.h"

class ObcI2cTargetCommunicationBase
{
public:
  ObcI2cTargetCommunicationBase(
    const unsigned int sils_port_id,
    const unsigned char i2c_address,
    OBC* obc
  );
  ObcI2cTargetCommunicationBase(
    const unsigned int hils_port_id,
    const unsigned char i2c_address,
    HilsPortManager* hils_port_manager
  );
  ObcI2cTargetCommunicationBase(
    const unsigned int sils_port_id,
    const unsigned int hils_port_id,
    const unsigned char i2c_address,
    OBC* obc,
    HilsPortManager* hils_port_manager
  );
  ~ObcI2cTargetCommunicationBase();

protected:
  void ReadRegister (const unsigned char reg_addr, unsigned char* data, const unsigned char len);
  void WriteRegister(const unsigned char reg_addr, const unsigned char* data, const unsigned char len);
  void ReadCommand  (unsigned char* data, const unsigned char len);
  int ReceiveCommand();
  int SendTelemetry (const unsigned char len);
  int GetStoredFrameCounter();

private:
  unsigned int sils_port_id_;
  unsigned int hils_port_id_;
  unsigned char i2c_address_;
  OBC_COM_UART_MODE sim_mode_ = OBC_COM_UART_MODE::MODE_ERROR;

  OBC* obc_;
  HilsPortManager* hils_port_manager_;
};