
#pragma once

#include "../cell/resource_grid.h"
#include "../ue_scheduling/ue_configuration.h"
#include "srsgnb/scheduler/scheduler_slot_handler.h"

namespace srsgnb {

/// PDCCH scheduling algorithm for a single cell and for both common and UE-dedicated CORESETs.
class pdcch_scheduler
{
public:
  virtual ~pdcch_scheduler() = default;

  /// Allocates RE space for common PDCCH, avoiding in the process collisions with other PDCCH allocations.
  /// \param rnti RNTI of allocation. Values: SI-RNTI, P-RNTI, RA-RNTIs.
  /// \param ss_id Search Space Id to use.
  /// \param aggr_lvl Aggregation Level of PDCCH allocation.
  /// \return Allocated PDCCH if successful.
  virtual pdcch_dl_information* alloc_pdcch_common(cell_slot_resource_allocator& slot_alloc,
                                                   rnti_t                        rnti,
                                                   search_space_id               ss_id,
                                                   aggregation_level             aggr_lvl) = 0;

  /// Allocates RE space for UE-dedicated DL PDCCH, avoiding in the process collisions with other PDCCH allocations.
  /// \param rnti RNTI of UE being allocated.
  /// \param user UE configuration for the provided cell.
  /// \param bwp_id BWP Id to use.
  /// \param ss_id Search Space Id to use.
  /// \param aggr_lvl Aggregation Level of PDCCH allocation.
  /// \param dci_fmt DCI format to use.
  /// \return Allocated PDCCH if successful.
  virtual pdcch_dl_information* alloc_dl_pdcch_ue(cell_slot_resource_allocator& slot_alloc,
                                                  rnti_t                        rnti,
                                                  const ue_cell_configuration&  user,
                                                  bwp_id_t                      bwpid,
                                                  search_space_id               ss_id,
                                                  aggregation_level             aggr_lvl,
                                                  dci_dl_format                 dci_fmt) = 0;

  /// Allocates RE space for UE-dedicated DL PDCCH, avoiding in the process collisions with other PDCCH allocations.
  /// \param rnti RNTI of UE being allocated.
  /// \param user UE configuration for the provided cell.
  /// \param bwp_id BWP Id to use.
  /// \param ss_id Search Space Id to use.
  /// \param aggr_lvl Aggregation Level of PDCCH allocation.
  /// \param dci_fmt DCI format to use.
  /// \return Allocated PDCCH if successful.
  virtual pdcch_ul_information* alloc_ul_pdcch_ue(cell_slot_resource_allocator& slot_alloc,
                                                  rnti_t                        rnti,
                                                  const ue_cell_configuration&  user,
                                                  bwp_id_t                      bwpid,
                                                  search_space_id               ss_id,
                                                  aggregation_level             aggr_lvl,
                                                  dci_ul_format                 dci_fmt) = 0;
};

} // namespace srsgnb
