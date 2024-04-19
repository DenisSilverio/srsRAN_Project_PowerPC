/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "procedures/rrc_ue_event_manager.h"
#include "rrc_ue_context.h"
#include "rrc_ue_logger.h"
#include "srsran/asn1/rrc_nr/ul_dcch_msg.h"
#include "srsran/rrc/rrc_ue.h"

namespace srsran {
namespace srs_cu_cp {

/// Main UE representation in RRC
class rrc_ue_impl final : public rrc_ue_interface
{
public:
  rrc_ue_impl(up_resource_manager&              up_resource_mng_,
              rrc_ue_du_processor_notifier&     du_proc_notif_,
              rrc_pdu_f1ap_notifier&            f1ap_pdu_notifier_,
              rrc_ue_nas_notifier&              nas_notif_,
              rrc_ue_control_notifier&          ngap_ctrl_notif_,
              rrc_ue_context_update_notifier&   cu_cp_notif_,
              rrc_ue_measurement_notifier&      measurement_notifier_,
              const ue_index_t                  ue_index_,
              const rnti_t                      c_rnti_,
              const rrc_cell_context            cell_,
              const rrc_ue_cfg_t&               cfg_,
              const byte_buffer                 du_to_cu_container,
              rrc_ue_task_scheduler&            task_sched,
              optional<rrc_ue_transfer_context> rrc_context);
  ~rrc_ue_impl() = default;

  // rrc_ul_ccch_pdu_handler
  void handle_ul_ccch_pdu(byte_buffer pdu) override;
  // rrc_ul_dcch_pdu_handler
  void handle_ul_dcch_pdu(const srb_id_t srb_id, byte_buffer pdcp_pdu) override;

  // rrc_ue_interface
  rrc_ul_ccch_pdu_handler&              get_ul_ccch_pdu_handler() override { return *this; }
  rrc_ul_dcch_pdu_handler&              get_ul_dcch_pdu_handler() override { return *this; }
  rrc_dl_nas_message_handler&           get_rrc_dl_nas_message_handler() override { return *this; }
  rrc_ue_srb_handler&                   get_rrc_ue_srb_handler() override { return *this; }
  rrc_ue_control_message_handler&       get_rrc_ue_control_message_handler() override { return *this; }
  rrc_ue_init_security_context_handler& get_rrc_ue_init_security_context_handler() override { return *this; }
  security::security_context&           get_rrc_ue_security_context() override { return context.sec_context; }
  rrc_ue_context_handler&               get_rrc_ue_context_handler() override { return *this; }
  rrc_ue_handover_preparation_handler&  get_rrc_ue_handover_preparation_handler() override { return *this; }

  // rrc_ue_srb_handler
  void                                  create_srb(const srb_creation_message& msg) override;
  static_vector<srb_id_t, MAX_NOF_SRBS> get_srbs() override;

  // rrc_dl_nas_message_handler
  void handle_dl_nas_transport_message(byte_buffer nas_pdu) override;

  // rrc_ue_control_message_handler
  async_task<bool> handle_rrc_reconfiguration_request(const rrc_reconfiguration_procedure_request& msg) override;
  rrc_ue_handover_reconfiguration_context
  get_rrc_ue_handover_reconfiguration_context(const rrc_reconfiguration_procedure_request& request) override;
  async_task<bool> handle_handover_reconfiguration_complete_expected(uint8_t transaction_id) override;
  async_task<bool> handle_rrc_ue_capability_transfer_request(const rrc_ue_capability_transfer_request& msg) override;
  rrc_ue_release_context  get_rrc_ue_release_context(bool requires_rrc_msg) override;
  rrc_ue_transfer_context get_transfer_context() override;
  optional<rrc_meas_cfg>  generate_meas_config(optional<rrc_meas_cfg> current_meas_config) override;
  bool                    handle_new_security_context(const security::security_context& sec_context) override;
  byte_buffer             get_rrc_handover_command(const rrc_reconfiguration_procedure_request& request,
                                                   unsigned                                     transaction_id) override;

  // rrc_ue_handover_preparation_handler
  byte_buffer get_packed_handover_preparation_message() override;
  byte_buffer handle_rrc_handover_command(byte_buffer cmd) override;

  // rrc_ue_context_handler
  rrc_ue_reestablishment_context_response get_context() override;

private:
  // message handlers
  void handle_pdu(const srb_id_t srb_id, byte_buffer rrc_pdu);
  void handle_rrc_setup_request(const asn1::rrc_nr::rrc_setup_request_s& msg);
  void handle_rrc_reest_request(const asn1::rrc_nr::rrc_reest_request_s& msg);
  void handle_ul_info_transfer(const asn1::rrc_nr::ul_info_transfer_ies_s& ul_info_transfer);
  void handle_rrc_transaction_complete(const asn1::rrc_nr::ul_dcch_msg_s& msg, uint8_t transaction_id_);
  void handle_measurement_report(const asn1::rrc_nr::meas_report_s& msg);

  // message senders
  /// Packs a DL-CCCH message and logs the message
  void send_dl_ccch(const asn1::rrc_nr::dl_ccch_msg_s& dl_ccch_msg);
  void send_dl_dcch(srb_id_t srb_id, const asn1::rrc_nr::dl_dcch_msg_s& dl_dcch_msg);

  // rrc_ue_setup_proc_notifier
  void on_new_dl_ccch(const asn1::rrc_nr::dl_ccch_msg_s& dl_ccch_msg) override;
  void on_ue_release_required(const ngap_cause_t& cause) override;

  // rrc_ue_security_mode_command_proc_notifier
  void on_new_dl_dcch(srb_id_t srb_id, const asn1::rrc_nr::dl_dcch_msg_s& dl_ccch_msg) override;
  void on_new_as_security_context() override;
  void on_security_context_sucessful() override;
  bool get_security_enabled() override { return context.security_enabled; }

  // initializes the security context and triggers the SMC procedure
  async_task<bool> handle_init_security_context(const security::security_context& sec_ctx) override;

  rrc_ue_context_t                context;
  up_resource_manager&            up_resource_mng;       // UP resource manager
  rrc_ue_du_processor_notifier&   du_processor_notifier; // notifier to the DU processor
  rrc_pdu_f1ap_notifier&          f1ap_pdu_notifier;     // PDU notifier to the F1AP
  rrc_ue_nas_notifier&            nas_notifier;          // PDU notifier to the NGAP
  rrc_ue_control_notifier&        ngap_ctrl_notifier;    // Control message notifier to the NGAP
  rrc_ue_context_update_notifier& cu_cp_notifier;        // notifier to the CU-CP
  rrc_ue_measurement_notifier&    measurement_notifier;  // cell measurement notifier
  byte_buffer                     du_to_cu_container;    // initial RRC message from DU to CU
  rrc_ue_task_scheduler&          task_sched;
  rrc_ue_logger                   logger;

  // RRC procedures handling
  std::unique_ptr<rrc_ue_event_manager> event_mng;

  /// RRC timer constants
  const unsigned rrc_reject_max_wait_time_s = 16;
};

} // namespace srs_cu_cp
} // namespace srsran
