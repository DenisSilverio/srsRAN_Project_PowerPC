/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "CLI/CLI11.hpp"
#include <optional>

namespace srsran {

/// UDP specific configuration of an UDP gateway.
struct udp_appconfig {
  /// Maximum amount of messages RX in a single syscall.
  unsigned rx_max_msgs = 256;
  /// Pool accupancy threshold after which packets are dropped.
  float pool_threshold = 0.9;
  /// Differentiated Services Code Point value.
  std::optional<unsigned> dscp;
};

/// \brief Configures the given CLI11 application with the UDP application configuration schema.
///
/// \param[out] app CLI11 application to configure.
/// \param[out] config UDP configuration that stores the parameters.
void configure_cli11_with_udp_config_schema(CLI::App& app, udp_appconfig& config);

} // namespace srsran
