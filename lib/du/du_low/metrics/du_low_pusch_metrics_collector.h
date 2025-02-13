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

#include "aggregators/channel_equalizer_metrics_aggregator.h"
#include "aggregators/channel_modulation_metrics_aggregator.h"
#include "aggregators/crc_calculator_metrics_aggregator.h"
#include "aggregators/port_channel_estimator_metrics_aggregator.h"
#include "aggregators/pusch_channel_estimator_metrics_aggregator.h"
#include "aggregators/pusch_demodulator_metrics_aggregator.h"
#include "aggregators/pusch_processor_metrics_aggregator.h"
#include "aggregators/scrambling_metrics_aggregator.h"
#include "aggregators/time_alignment_estimator_metrics_aggregator.h"
#include "aggregators/transform_precoder_metrics_aggregators.h"
#include "aggregators/ulsch_demultiplex_metrics_aggregator.h"
#include "srsran/du/du_low/o_du_low_metrics.h"

namespace srsran {
namespace srs_du {

/// PUSCH processing metrics collector.
class du_low_pusch_metrics_collector
{
public:
  /// Collects the metrics from the PUSCH aggregators and writes them into the given metrics argument.
  void collect_metrics(o_du_low_pusch_metrics& metrics);

  /// Returns CRC calculator metric aggregator's interface for metrics notification.
  crc_calculator_metric_notifier& get_pusch_crc_calculator_notifier() { return pusch_crc_calculator_aggregator; }

  /// Returns channel estimator metric aggregator's interface for metrics notification.
  pusch_channel_estimator_metric_notifier& get_pusch_channel_estimator_notifier()
  {
    return pusch_channel_estimator_aggregator;
  }

  /// Returns scrambling metric aggregator's interface for metrics notification.
  pseudo_random_sequence_generator_metric_notifier& get_pusch_scrambling_notifier()
  {
    return pusch_scrambling_aggregator;
  }

  /// Returns channel equalizer metric aggregator's interface for metrics notification.
  channel_equalizer_metric_notifier& get_pusch_channel_equalizer_notifier()
  {
    return pusch_channel_equalizer_aggregator;
  }

  /// Returns demodulation mapper metric aggregator's interface for metrics notification.
  common_channel_modulation_metric_notifier& get_pusch_demodulation_mapper_notifier()
  {
    return pusch_demodulation_mapper_aggregator;
  }

  /// Returns EVM calculator metric aggregator's interface for metrics notification.
  common_channel_modulation_metric_notifier& get_pusch_evm_calculator_notifier()
  {
    return pusch_evm_calculator_aggregator;
  }

  /// Returns demodulator metric aggregator's interface for metrics notification.
  pusch_demodulator_metric_notifier& get_pusch_demodulator_notifier() { return pusch_demodulator_aggregator; }

  /// Returns time alignment estimator metric aggregator's interface for metrics notification.
  time_alignment_estimator_metric_notifier& get_pusch_ta_estimator_notifier() { return pusch_ta_estimator_aggregator; }

  /// Returns time port channel estimator metric aggregator's interface for metrics notification.
  port_channel_estimator_metric_notifier& get_pusch_port_channel_estimator_notifier()
  {
    return pusch_port_ch_estimator_aggregator;
  }

  /// Returns time transform precoder metric aggregator's interface for metrics notification.
  transform_precoder_metric_notifier& get_pusch_transform_precoder_notifier()
  {
    return pusch_transform_precoder_aggregator;
  }

  /// Returns time PUSCH processor metrics aggregator's interface for metrics notification.
  pusch_processor_metric_notifier& get_pusch_processor_notifier() { return pusch_processor_aggregator; }

private:
  /// PUSCH CRC calculator metrics aggregator.
  crc_calculator_metrics_aggregator pusch_crc_calculator_aggregator;
  /// PUSCH scrambling metrics aggregator.
  scrambling_metrics_aggregator pusch_scrambling_aggregator;
  /// PUSCH channel estimator metrics aggregator.
  pusch_channel_estimator_metrics_aggregator pusch_channel_estimator_aggregator;
  /// PUSCH channel equalizer metrics aggregator.
  channel_equalizer_metrics_aggregator pusch_channel_equalizer_aggregator;
  /// PUSCH transform precoder metrics aggregator.
  transform_precoder_metrics_aggregator pusch_transform_precoder_aggregator;
  /// PUSCH demodulation mapper metrics aggregator.
  channel_modulation_metrics_aggregator pusch_demodulation_mapper_aggregator;
  /// PUSCH EVM calculator metrics aggregator.
  channel_modulation_metrics_aggregator pusch_evm_calculator_aggregator;
  /// PUSCH demodulator metrics aggregator.
  pusch_demodulator_metrics_aggregator pusch_demodulator_aggregator;
  /// Time alignment estimator metrics aggregator.
  time_alignment_estimator_metrics_aggregator pusch_ta_estimator_aggregator;
  /// Port channel estimator metrics aggregator.
  port_channel_estimator_metrics_aggregator pusch_port_ch_estimator_aggregator;
  /// UL-SCH demultiplexer metrics aggregator.
  ulsch_demultiplex_metrics_aggregator ulsch_demux_aggregator;
  /// PUSCH processor metrics aggregator.
  pusch_processor_metrics_aggregator pusch_processor_aggregator;
};

} // namespace srs_du
} // namespace srsran
