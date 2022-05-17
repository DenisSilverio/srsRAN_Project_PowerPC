/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief PUSCH decoder interface declaration.

#ifndef SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PUSCH_DECODER_H
#define SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PUSCH_DECODER_H

#include "srsgnb/adt/span.h"
#include "srsgnb/phy/upper/channel_coding/ldpc/ldpc_decoder.h"
#include "srsgnb/phy/upper/codeblock_metadata.h"
#include "srsgnb/phy/upper/rx_softbuffer.h"

#include <memory>

namespace srsgnb {

/// \brief PUSCH decoder interface.
///
/// User interface of the PHY Uplink Shared Channel (PUSCH) decoder.
class pusch_decoder
{
public:
  /// PUSCH decoding statistics.
  struct statistics {
    /// Denotes whether the received transport block passed the CRC.
    bool tb_crc_ok = false;
    /// Number of LDPC iterations???
    unsigned nof_ldpc_iters = 0;
  };

  /// Alias for LDPC decoding algorithm configuration.
  using algorithm_details = ldpc_decoder::configuration::algorithm_details;

  /// Default destructor.
  virtual ~pusch_decoder() = default;

  /// \brief Decodes a PUSCH codeword.
  ///
  /// Recovers a UL-SCH transport block from a PUSCH codeword. Reverting the encoding operations described in TS 38.212
  /// Sections 6.2.1-6.2.6, the codeword is first split into rate-matched codeblocks. Then, each codeblock is restored
  /// to its base rate, combined with previous retransmissions, and decoded. Finally, if all blocks pass the CRC check,
  /// the data bits from all codeblocks are concatenated to form the UL-SCH transport block. If applicable, a last
  /// transport-block CRC is computed and verified.
  ///
  /// \param[out]    transport_block The decoded transport block, with packed (8 bits per entry) representation.
  /// \param[out]    info            Decoding statistics.
  /// \param[in,out] soft_codeword   A soft-buffer for combining log-likelihood ratios from different retransmissions.
  /// \param[in]     llrs            The received codeword, as a sequence of log-likelihood ratios.
  /// \param[in]     new_data        Flags new data transmissions (as opposed to retransmissions of previously failed
  /// transport blocks). \param[in]     blk_cfg         Transport block configuration. \param[in]     alg_cfg LDPC
  /// decoding algorithm configuration.
  virtual void decode(span<uint8_t>            transport_block,
                      statistics&              info,
                      rx_softbuffer*           soft_codeword,
                      span<const int8_t>       llrs,
                      bool                     new_data,
                      const segment_config&    blk_cfg,
                      const algorithm_details& alg_cfg) = 0;
};

std::unique_ptr<pusch_decoder> create_pusch_decoder();

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PUSCH_DECODER_H
