/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "intra_cu_handover_target_routine.h"
#include "srsran/cu_cp/cu_cp_types.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::rrc_nr;

intra_cu_handover_target_routine::intra_cu_handover_target_routine(
    const cu_cp_intra_cu_handover_target_request& request_,
    cu_cp_ue_context_release_handler&             ue_context_release_handler_,
    cu_cp_ue_context_manipulation_handler&        cu_cp_handler_,
    ue_manager&                                   ue_mng_,
    srslog::basic_logger&                         logger_) :
  request(request_),
  ue_context_release_handler(ue_context_release_handler_),
  cu_cp_handler(cu_cp_handler_),
  ue_mng(ue_mng_),
  logger(logger_)
{
}

void intra_cu_handover_target_routine::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  if (ue_mng.find_du_ue(request.target_ue_index) == nullptr) {
    logger.warning("Target UE={} got removed", request.target_ue_index);
    CORO_EARLY_RETURN();
  }
  target_ue = ue_mng.find_du_ue(request.target_ue_index);

  logger.debug("ue={}: \"{}\" initialized", request.target_ue_index, name());

  // Notify RRC UE to await ReconfigurationComplete.
  CORO_AWAIT_VALUE(reconf_result,
                   target_ue->get_rrc_ue()->handle_handover_reconfiguration_complete_expected(request.transaction_id,
                                                                                              request.timeout));

  if (!reconf_result) {
    logger.warning("ue={}: \"{}\" failed", request.target_ue_index, name());
    CORO_EARLY_RETURN();
  }

  {
    // Transfer old UE context (NGAP and E1AP) to new UE context and remove old UE context.
    cu_cp_handler.handle_handover_ue_context_push(request.source_ue_index, request.target_ue_index);
  }

  // Remove source UE context.

  if (ue_mng.find_du_ue(request.source_ue_index) == nullptr) {
    logger.warning("Source UE={} already got removed", request.source_ue_index);
  } else {
    source_ue = ue_mng.find_du_ue(request.source_ue_index);

    ue_context_release_command.ue_index             = source_ue->get_ue_index();
    ue_context_release_command.cause                = ngap_cause_radio_network_t::unspecified;
    ue_context_release_command.requires_rrc_release = false;
    CORO_AWAIT(ue_context_release_handler.handle_ue_context_release_command(ue_context_release_command));
    logger.debug("ue={}: \"{}\" removed source UE context", ue_context_release_command.ue_index, name());
  }

  logger.debug("ue={}: \"{}\" finalized", request.target_ue_index, name());

  CORO_RETURN();
}
