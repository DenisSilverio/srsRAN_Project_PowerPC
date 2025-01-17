/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/fapi_adaptor/phy/messages/prs.h"
#include "srsran/fapi_adaptor/precoding_matrix_repository.h"
#include "srsran/phy/upper/signal_processors/prs/prs_generator_configuration.h"

using namespace srsran;
using namespace fapi_adaptor;

void srsran::fapi_adaptor::convert_prs_fapi_to_phy(prs_generator_configuration&       generator_config,
                                                   const fapi::dl_prs_pdu&            fapi_pdu,
                                                   uint16_t                           sfn,
                                                   uint16_t                           slot,
                                                   const precoding_matrix_repository& pm_repo)
{
  generator_config.slot            = slot_point(fapi_pdu.scs, sfn, slot);
  generator_config.cp              = fapi_pdu.cp;
  generator_config.n_id_prs        = fapi_pdu.nid_prs;
  generator_config.comb_size       = fapi_pdu.comb_size;
  generator_config.comb_offset     = fapi_pdu.comb_offset;
  generator_config.duration        = fapi_pdu.num_symbols;
  generator_config.start_symbol    = fapi_pdu.first_symbol;
  generator_config.prb_start       = fapi_pdu.start_rb;
  generator_config.freq_alloc      = interval<uint16_t>::start_and_len(fapi_pdu.start_rb, fapi_pdu.num_rbs);
  generator_config.power_offset_dB = fapi_pdu.prs_power_offset.has_value() ? fapi_pdu.prs_power_offset.value() : 0.f;
  generator_config.precoding       = precoding_configuration::make_wideband(
      pm_repo.get_precoding_matrix(fapi_pdu.precoding_and_beamforming.prgs.front().pm_index));
}
