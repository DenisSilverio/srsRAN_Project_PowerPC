/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */
#pragma once

#include "srsgnb/phy/prach_buffer.h"
#include "srsgnb/phy/prach_buffer_context.h"
#include "srsgnb/phy/resource_grid.h"
#include "srsgnb/phy/resource_grid_context.h"

namespace srsgnb {

/// Describes the lower physical layer input gateway.
class lower_phy_input_gateway
{
public:
  /// Default destructor.
  virtual ~lower_phy_input_gateway() = default;

  /// \brief Requests to the lower PHY to capture PRACH window.
  ///
  /// The lower PHY must capture a PHY window identified by \c context. The capture must start at slot \c context.slot
  /// and symbol \c context.start_symbol. The capture must finish once \c buffer.is_full() returns true.
  ///
  /// \param[in] context Provides the PRACH window context.
  /// \param[in] buffer  Provides the PRACH buffer used to write the PRACH window.
  virtual void request_prach_window(const prach_buffer_context& context, prach_buffer* buffer) = 0;

  /// \brief Requests to the lower PHY an uplink slot.
  ///
  /// The lower PHY must process the slot described by \c context and notify the demodulation per symbol basis of the
  /// requested slot.
  ///
  /// The notification contains the exact context and grid.
  ///
  /// \param[in] context Resource grid context.
  /// \param[in] buffer  Resource grid to store the processed slot.
  virtual void request_uplink_slot(const resource_grid_context& context, resource_grid& grid) = 0;

  /// \brief Sends resource grid through the gateway.
  /// \param[in] context Indicates the resource grid context.
  /// \param[in] grid Provides the resource grid to transmit.
  virtual void send(const resource_grid_context& context, const resource_grid_reader& grid) = 0;
};
} // namespace srsgnb
