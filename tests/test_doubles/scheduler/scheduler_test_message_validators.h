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

/// \file
/// \brief Validators of scheduler result messages.
///
/// The validators in this file only perform checks that do not depend on any configuration or contextual information
/// of the scheduler or DU-high.

#pragma once

namespace srsran {
namespace test_helper {

/// \brief Determine if the PDSCH grant for a given UE has valid content.
bool is_valid_dl_msg_alloc(const dl_msg_alloc& grant);

/// \brief Determine if the PUSCH grant for a given UE has valid content.
bool is_valid_ul_sched_info(const ul_sched_info& grant);

/// \brief Determine if the UE PDSCH grants for a given slot are valid.
bool is_valid_dl_msg_alloc_list(span<const dl_msg_alloc> grants);

} // namespace test_helper
} // namespace srsran
