/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_cp_manager_impl.h"
#include "../ran/bcd_helpers.h"
#include "f1c_asn1_helpers.h"
#include "srsgnb/f1_interface/f1ap_cu.h"

using namespace srsgnb;
using namespace srs_cu_cp;

cu_cp_manager_impl::cu_cp_manager_impl(const cu_cp_manager_config_t& cfg_) :
  cfg(cfg_), du_mng(cfg), ue_mng(cfg), main_ctrl_loop(128)
{
  // nothing to start straigt away on the CU
  ctx = {}; // make it compile
}

void cu_cp_manager_impl::handle_f1_setup_request(const f1_setup_request_message& msg)
{
  // TODO: add handling
  cfg.logger.debug("Received F1 setup request");

  // Reject request without served cells
  if (not msg.request->gnb_du_served_cells_list_present) {
    cfg.logger.error("Not handling F1 setup without served cells");
    send_f1_setup_failure(asn1::f1ap::cause_c::types::options::radio_network);
    return;
  }

  // Create new DU context
  du_context du_ctxt{};
  du_ctxt.du_index = MIN_DU_INDEX; // TODO: get unique next index
  du_ctxt.id       = msg.request->gnb_du_id.value;
  if (msg.request->gnb_du_name_present) {
    du_ctxt.name = msg.request->gnb_du_name.value.to_string();
  }

  for (const auto& served_cell : msg.request->gnb_du_served_cells_list.value) {
    const auto&     cell_item = served_cell.value().gnb_du_served_cells_item();
    du_cell_context du_cell;
    du_cell.cell_index = MIN_DU_CELL_INDEX; // TODO: get unique next idx
    du_cell.pci = cell_item.served_cell_info.nrpci;
    du_cell.cgi        = cgi_from_asn1(cell_item.served_cell_info.nrcgi);

    if (not cell_item.gnb_du_sys_info_present) {
      cfg.logger.error("Not handling served cells without system information");
      send_f1_setup_failure(asn1::f1ap::cause_c::types::options::radio_network);
      return;
    }

    // Store and unpack system information
    du_cell.sys_info.packed_mib =
        byte_buffer(cell_item.gnb_du_sys_info.mib_msg.begin(), cell_item.gnb_du_sys_info.mib_msg.end());
    du_cell.sys_info.packed_sib1 =
        byte_buffer(cell_item.gnb_du_sys_info.sib1_msg.begin(), cell_item.gnb_du_sys_info.sib1_msg.end());

    // TODO: add unpacking

    // add cell to DU context
    du_cell_index_t cell_index = du_cell.cell_index;
    du_ctxt.cell_db.emplace(cell_index, std::move(du_cell));
  }

  send_f1_setup_response(du_ctxt);

  // add DU
  du_mng.add_du(du_ctxt);
}

void cu_cp_manager_impl::handle_initial_ul_rrc_message_transfer(const initial_ul_rrc_message_transfer_message& msg)
{
  nr_cell_global_identity cgi = cgi_from_asn1(msg.msg->nrcgi.value);

  // Reject request without served cells
  if (not msg.msg->duto_currc_container_present) {
    cfg.logger.error("Not handling Initial UL RRC message transfer without DU to CU container");
    /// Assume the DU can't serve the UE. Ignoring the message.
    return;
  }

  cfg.logger.info(
      "Received Initial UL RRC message transfer nr_cgi={}, crnti={}", cgi.nci.packed, msg.msg->c_rnti.value);
  cfg.logger.debug("mcc={}, mnc={}", cgi.mcc, cgi.mnc);

  // TODO: Look up DU and cell with NR-CGI

  if (msg.msg->sul_access_ind_present) {
    cfg.logger.debug("Ignoring SUL access indicator");
  }
}

void cu_cp_manager_impl::handle_ul_rrc_message_transfer(const ul_rrc_message_transfer_message& msg)
{
  // TODO: add handling and start procedure if needed
}

/// Sender for F1AP messages
void cu_cp_manager_impl::send_f1_setup_response(const du_context& du_ctxt)
{
  f1_setup_response_message response;
  response.success = true;
  fill_asn1_f1_setup_response(response.response, cfg.name, cfg.rrc_version, du_ctxt.cell_db);
  cfg.f1ap_conn_mng->handle_f1ap_setup_response(response);
}

void cu_cp_manager_impl::send_f1_setup_failure(asn1::f1ap::cause_c::types::options cause)
{
  f1_setup_response_message response;
  response.success = false;
  response.failure->cause->set(cause);
  cfg.f1ap_conn_mng->handle_f1ap_setup_response(response);
}

size_t cu_cp_manager_impl::get_nof_dus() const
{
  // TODO: this probably needs to be protected. It's hard to say when this is needed.
  return du_mng.get_nof_dus();
}

size_t cu_cp_manager_impl::get_nof_ues() const
{
  // TODO: This is temporary code.
  static std::mutex              mutex;
  static std::condition_variable cvar;
  size_t                         result = MAX_NOF_UES;
  cfg.cu_cp_mng_exec->execute([this, &result]() {
    std::unique_lock<std::mutex> lock(mutex);
    result = ue_mng.get_ues().size();
    cvar.notify_one();
  });
  {
    std::unique_lock<std::mutex> lock(mutex);
    while (result == MAX_NOF_DU_UES) {
      cvar.wait(lock);
    }
  }
  return result;
}
