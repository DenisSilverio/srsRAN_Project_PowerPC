
#pragma once

#include "srsgnb/ran/slot_point.h"

namespace srsgnb {

/// CQI Value reported.
using cqi_report = uint8_t;

/// RSSI value reported.
using rssi_report = uint16_t;

/// CRC.Indication
struct crc_indication_message {
  slot_point sl_rx;
  // TODO: remaining fields.
};

/// Interface to handle feedback information from the PHY.
class mac_cell_control_information_handler
{
public:
  virtual ~mac_cell_control_information_handler() = default;

  virtual void handle_crc(const crc_indication_message& msg) = 0;
};

} // namespace srsgnb
