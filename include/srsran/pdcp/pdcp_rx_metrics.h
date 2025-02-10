/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/support/engineering_notation.h"
#include "srsran/support/format/fmt_to_c_str.h"
#include "srsran/support/timers.h"
#include "fmt/format.h"
#include "fmt/ranges.h"
#include <array>

/*
 * This file will hold the interfaces and structures for the
 * PDCP RX entity metrics collection. This also includes formatting
 * helpers for printing the metrics.
 */
namespace srsran {

/// This struct will hold relevant metrics for the PDCP RX
struct pdcp_rx_metrics_container {
  uint32_t num_pdus;
  uint32_t num_pdu_bytes;
  uint32_t num_dropped_pdus;
  uint32_t num_sdus;
  uint32_t num_sdu_bytes;
  uint32_t num_integrity_verified_pdus;
  uint32_t num_integrity_failed_pdus;
  uint32_t num_t_reordering_timeouts;
  uint32_t reordering_delay_us;
  uint32_t reordering_counter;
  uint32_t sum_sdu_latency_ns; ///< total SDU latency (in ns)
  unsigned counter;

  // Histogram of SDU latencies
  static constexpr unsigned                   sdu_latency_hist_bins = 8;
  static constexpr unsigned                   nof_usec_per_bin      = 1;
  std::array<uint32_t, sdu_latency_hist_bins> sdu_latency_hist;
  uint32_t                                    max_sdu_latency_ns;
};

inline std::string format_pdcp_tx_metrics(timer_duration metrics_period, const pdcp_rx_metrics_container& m)
{
  fmt::memory_buffer buffer;
  fmt::format_to(std::back_inserter(buffer),
                 "num_sdus={} sdu_rate={}bps num_dropped_pdus={} num_pdus={} pdu_rate={}bps "
                 "num_integrity_verified_pdus={} num_integrity_failed_pdus={} num_t_reordering_timeouts={} "
                 "reordering_delay={}us reordering_counter={} sum_sdu_latency={}ns sdu_latency_hist=[",
                 scaled_fmt_integer(m.num_sdus, false),
                 float_to_eng_string(static_cast<float>(m.num_sdu_bytes) * 8 * 1000 / metrics_period.count(), 1, false),
                 scaled_fmt_integer(m.num_dropped_pdus, false),
                 scaled_fmt_integer(m.num_pdus, false),
                 float_to_eng_string(static_cast<float>(m.num_pdu_bytes) * 8 * 1000 / metrics_period.count(), 1, false),
                 scaled_fmt_integer(m.num_integrity_verified_pdus, false),
                 scaled_fmt_integer(m.num_integrity_failed_pdus, false),
                 scaled_fmt_integer(m.num_t_reordering_timeouts, false),
                 m.reordering_delay_us,
                 scaled_fmt_integer(m.reordering_counter, false),
                 m.sum_sdu_latency_ns);
  bool first_bin = true;
  for (auto freq : m.sdu_latency_hist) {
    fmt::format_to(std::back_inserter(buffer), "{}{}", first_bin ? "" : " ", float_to_eng_string(freq, 1, false));
    first_bin = false;
  }
  fmt::format_to(std::back_inserter(buffer), "] max_sdu_latency={}us", m.max_sdu_latency_ns * 1e-3);
  return to_c_str(buffer);
}
} // namespace srsran

namespace fmt {
// PDCP RX metrics formatter
template <>
struct formatter<srsran::pdcp_rx_metrics_container> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::pdcp_rx_metrics_container m, FormatContext& ctx) const
  {
    return format_to(ctx.out(),
                     "num_sdus={} num_sdu_bytes={} num_dropped_pdus={} num_pdus={} num_pdu_bytes={} "
                     "num_integrity_verified_pdus={} num_integrity_failed_pdus={} num_t_reordering_timeouts={} "
                     "reordering_delay={}us reordering_counter={} sum_sdu_latency={}ns sdu_latency_hist=[{}] "
                     "max_sdu_latency={}ns",
                     m.num_sdus,
                     m.num_sdu_bytes,
                     m.num_dropped_pdus,
                     m.num_pdus,
                     m.num_pdu_bytes,
                     m.num_integrity_verified_pdus,
                     m.num_integrity_failed_pdus,
                     m.num_t_reordering_timeouts,
                     m.reordering_delay_us,
                     m.reordering_counter,
                     m.sum_sdu_latency_ns,
                     fmt::join(m.sdu_latency_hist, " "),
                     m.max_sdu_latency_ns);
  }
};
} // namespace fmt
