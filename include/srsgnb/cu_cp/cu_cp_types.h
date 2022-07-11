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

#include <cstdint>
#include <type_traits>

namespace srsgnb {
namespace srs_cu_cp {

/// Maximum number of UEs supported by CU-CP (implementation-defined).
enum ue_index_t : uint16_t {
  MIN_UE_INDEX     = 0,
  MAX_UE_INDEX     = 1023,
  MAX_NOF_UES      = 1024,
  INVALID_UE_INDEX = MAX_NOF_UES
};

/// Maximum number of DUs supported by CU-CP (implementation-defined).
enum du_index_t : uint16_t { MIN_DU_INDEX = 0, MAX_DU_INDEX = 1, MAX_NOF_DUS = 2, INVALID_DU_INDEX = MAX_NOF_DUS };

/// Maximum number of cells per DU supported by CU-CP (implementation-defined).
enum du_cell_index_t : uint16_t {
  MIN_DU_CELL_INDEX     = 0,
  MAX_DU_CELL_INDEX     = 15,
  MAX_NOF_DU_CELLS      = 16,
  INVALID_DU_CELL_INDEX = MAX_NOF_DU_CELLS
};

/// Convert integer to DU index type.
constexpr inline du_index_t int_to_du_index(std::underlying_type_t<du_index_t> idx)
{
  return static_cast<du_index_t>(idx);
}

/// Convert integer to CU UE index type.
constexpr inline ue_index_t int_to_ue_index(std::underlying_type_t<ue_index_t> idx)
{
  return static_cast<ue_index_t>(idx);
}

constexpr inline bool is_ue_index_valid(ue_index_t ue_idx)
{
  return ue_idx < MAX_NOF_UES;
}

/// Convert integer to CU cell index type.
inline du_cell_index_t int_to_du_cell_index(std::underlying_type_t<du_cell_index_t> idx)
{
  return static_cast<du_cell_index_t>(idx);
}

} // namespace srs_cu_cp
} // namespace srsgnb
