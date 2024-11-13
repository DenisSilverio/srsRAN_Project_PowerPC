/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/ran/drx_config.h"

using namespace srsran;
using namespace drx_helper;

span<const std::chrono::milliseconds> srsran::drx_helper::valid_long_cycle_values()
{
  using msec = std::chrono::milliseconds;
  static constexpr std::array<msec, 20> values{msec{10},   msec{20},   msec{32},   msec{40},   msec{60},
                                               msec{64},   msec{70},   msec{80},   msec{128},  msec{160},
                                               msec{256},  msec{320},  msec{512},  msec{640},  msec{1024},
                                               msec{1280}, msec{2048}, msec{2560}, msec{5120}, msec{10240}};
  return values;
}

span<const std::chrono::milliseconds> srsran::drx_helper::valid_on_duration_timer_values()
{
  using msec = std::chrono::milliseconds;
  static constexpr std::array<msec, 24> values{msec{1},   msec{2},   msec{3},   msec{4},    msec{5},    msec{6},
                                               msec{8},   msec{10},  msec{20},  msec{30},   msec{40},   msec{50},
                                               msec{60},  msec{80},  msec{100}, msec{200},  msec{300},  msec{400},
                                               msec{500}, msec{600}, msec{800}, msec{1000}, msec{1200}, msec{1600}};
  return values;
}

span<const std::chrono::milliseconds> srsran::drx_helper::valid_inactivity_timer_values()
{
  using msec = std::chrono::milliseconds;
  static constexpr std::array<msec, 24> values{msec{0},   msec{1},   msec{2},    msec{3},    msec{4},   msec{5},
                                               msec{6},   msec{8},   msec{10},   msec{20},   msec{30},  msec{40},
                                               msec{50},  msec{60},  msec{80},   msec{100},  msec{200}, msec{300},
                                               msec{500}, msec{750}, msec{1280}, msec{1920}, msec{2560}};
  return values;
}
