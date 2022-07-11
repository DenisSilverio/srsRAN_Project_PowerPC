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

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/adt/expected.h"
#include "srsgnb/asn1/f1ap.h"
#include "srsgnb/cu_cp/cu_cp_types.h"
#include "srsgnb/f1_interface/common/f1c_common.h"
#include "srsgnb/support/async/async_task.h"

namespace srsgnb {
namespace srs_cu_cp {

/// \remark See TS 38.473 Section 9.3.1.4: CU and DU F1AP DU valid values: (0..2^32-1)
enum f1ap_ue_id_t : uint64_t {
  INVALID_F1AP_UE_ID = 0x1ffffffff,
};

/// Convert integer to F1AP UE ID type.
constexpr inline f1ap_ue_id_t int_to_f1ap_ue_id(std::underlying_type_t<f1ap_ue_id_t> idx)
{
  return static_cast<f1ap_ue_id_t>(idx);
}

struct f1ap_ue_context {
  f1ap_ue_id_t du_ue_f1ap_id;
  ue_index_t   ue_index;
};

struct f1ap_initial_ul_rrc_msg {
  asn1::f1ap::init_ulrrc_msg_transfer_s msg;
};

struct f1ap_ul_rrc_msg {
  asn1::f1ap::ulrrc_msg_transfer_s msg;
};

struct f1ap_dl_rrc_msg {
  asn1::f1ap::dlrrc_msg_transfer_s msg;
};

class f1ap_rrc_message_transfer_procedure_handler
{
public:
  virtual ~f1ap_rrc_message_transfer_procedure_handler() = default;

  /// \brief Packs and transmits the DL RRC message transfer as per TS 38.473 section 8.4.2.
  /// \param[in] msg The DL RRC message transfer message to transmit.
  virtual void handle_dl_rrc_message_transfer(const f1ap_dl_rrc_msg& msg) = 0;
};

struct f1_setup_response_message {
  asn1::f1ap::f1_setup_resp_s response;
  asn1::f1ap::f1_setup_fail_s failure;
  bool                        success;
};

struct f1_setup_request_message {
  asn1::f1ap::f1_setup_request_s request;
};

struct f1_removal_request_message {
  asn1::f1ap::f1_removal_request_s request;
};

/// Handle F1AP interface management procedures as defined in TS 38.473 section 8.2.
class f1ap_connection_manager
{
public:
  virtual ~f1ap_connection_manager() = default;

  /// \brief Creates and transmits the F1 Setup outcome to the DU.
  /// \param[in] msg The f1_setup_response_message to transmit.
  /// \remark The CU transmits the F1SetupResponse/F1SetupFailure as per TS 38.473 section 8.2.3.
  virtual void handle_f1ap_setup_response(const f1_setup_response_message& msg) = 0;
};

struct f1ap_ue_context_setup_request_message {
  // TODO: replace asn1 by necessary containers
  asn1::f1ap::ue_context_setup_request_s msg;
};

struct f1ap_ue_context_setup_response_message {
  asn1::f1ap::ue_context_setup_resp_s msg;
  bool                                success;
};

struct f1ap_ue_context_release_command_message {
  asn1::f1ap::ue_context_release_complete_s msg;
};

struct f1ap_ue_context_release_complete_message {
  asn1::f1ap::ue_context_release_complete_s msg;
};

struct f1ap_ue_context_modification_request_message {
  asn1::f1ap::ue_context_mod_request_s msg;
};

struct f1ap_ue_context_modification_response_message {
  asn1::f1ap::ue_context_mod_resp_s response;
  asn1::f1ap::ue_context_mod_fail_s failure;
  bool                              success;
};

/// Handle F1AP UE context management procedures as defined in TS 38.473 section 8.3.
class f1ap_ue_context_manager
{
public:
  virtual ~f1ap_ue_context_manager() = default;

  /// Establish the UE context in F1.
  virtual async_task<f1ap_ue_context_setup_response_message>
  handle_ue_context_setup_request(const f1ap_ue_context_setup_request_message& request) = 0;

  /// \brief Initiates the UE Context Release procedure as per TS 38.473 section 8.3.3.
  /// \param[in] msg The UE Context Release message to transmit.
  /// \return Returns a UE Context Release Complete message.
  virtual async_task<f1ap_ue_context_release_complete_message>
  handle_ue_context_release(const f1ap_ue_context_release_command_message& msg) = 0;

  /// \brief Initiates the UE Context Modification procedure as per TS 38.473 section 8.3.4.
  /// \param[in] request The UE Context Modification message to transmit.
  /// \return Returns a f1ap_ue_context_modification_response_message struct struct with the success member set to
  /// 'true' in case of a successful outcome, 'false' otherwise.
  virtual async_task<f1ap_ue_context_modification_response_message>
  handle_ue_context_modification(const f1ap_ue_context_modification_request_message& request) = 0;
};

/// Methods used by F1AP to notify the DU processor about messages.
class f1c_du_processor_message_notifier
{
public:
  virtual ~f1c_du_processor_message_notifier() = default;

  /// \brief Notifies the DU processor about the reception of a F1 Setup Request message.
  /// \param[in] msg The received F1 Setup Request message.
  virtual void on_f1_setup_request_received(const f1_setup_request_message& msg) = 0;

  /// \brief Lookup the cell based on a given NR cell ID.
  /// \param[in] packed_nr_cell_id The packed NR cell ID received over F1AP.
  virtual du_cell_index_t find_cell(uint64_t packed_nr_cell_id) = 0;
};

/// Methods used by F1AP to notify about RRC messages.
class f1c_rrc_message_notifier
{
public:
  virtual ~f1c_rrc_message_notifier() = default;

  /// \brief Notifies about the reception of a initial UL RRC message transfer message.
  /// \param[in] pcell_index The DU cell index.
  /// \param[in] msg The received initial UL RRC message transfer message.
  /// \return Returns the index of the created UE.
  virtual ue_index_t on_initial_ul_rrc_message_transfer_received(const du_cell_index_t          pcell_index,
                                                                 const f1ap_initial_ul_rrc_msg& msg) = 0;

  /// \brief Notifies about the reception of a UL RRC message transfer message.
  /// \param[in] ue_index The UE index.
  /// \param[in] msg The received UL RRC message transfer message.
  virtual void on_ul_rrc_message_transfer_received(const ue_index_t ue_index, const f1ap_ul_rrc_msg& msg) = 0;
};

/// Methods used by F1AP to notify CU-CP manager about DU specific events.
class f1c_du_management_notifier
{
public:
  virtual ~f1c_du_management_notifier() = default;
  /// \brief Notifies the CU-CP manager about a successful F1 Removal procedure.
  /// The corresponding DU processor will be removed now.
  /// \param[in] du_index The index of the DU processor to delete.
  virtual void on_du_remove_request_received(const du_index_t du_index) = 0;
};

/// Combined entry point for F1C/U handling.
class f1_interface : public f1c_message_handler,
                     public f1c_event_handler,
                     public f1ap_rrc_message_transfer_procedure_handler,
                     public f1ap_connection_manager,
                     public f1ap_ue_context_manager
{
public:
  virtual ~f1_interface() = default;
};

} // namespace srs_cu_cp
} // namespace srsgnb
