/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_CU_CP_F1C_ASN1_HELPERS_H
#define SRSGNB_CU_CP_F1C_ASN1_HELPERS_H

#include "../ran/bcd_helpers.h"
#include "du_context.h"
#include "srsgnb/asn1/f1ap.h"
namespace srsgnb {

namespace srs_cu_cp {

/// \brief Converts ASN.1 CGI typo into internal struct.
/// \param[in] asn1_cgi The ASN.1 encoded NR-CGI.
/// \return The CGI converted to flat internal struct.
nr_cell_global_identity cgi_from_asn1(const asn1::f1ap::nrcgi_s& asn1_cgi)
{
  nr_cell_global_identity cgi          = {};
  uint32_t                encoded_plmn = asn1_cgi.plmn_id.to_number();
  ngap_plmn_to_mccmnc(encoded_plmn, &cgi.mcc, &cgi.mnc);
  cgi.nci.packed = asn1_cgi.nrcell_id.to_number();
  return cgi;
}

/// \brief Fills ASN.1 F1SetupResponse struct.
/// \param[out] response The F1SetupResponse ASN.1 struct to fill.
/// \param[in] params DU setup parameters to add to the F1SetupRequest.
void fill_asn1_f1_setup_response(asn1::f1ap::f1_setup_resp_s&                         response,
                                 const std::string&                                   name,
                                 const uint8_t                                        rrc_version,
                                 const slot_array<du_cell_context, MAX_NOF_DU_CELLS>& du_cell_db)
{
  // fill CU common info
  response->gnb_cu_name_present = true;
  response->gnb_cu_name->from_string(name);
  response->gnb_cu_rrc_version.value.latest_rrc_version.from_number(rrc_version);

  // activate all DU cells
  response->cells_to_be_activ_list_present = true;
  for (const auto& du_cell : du_cell_db) {
    asn1::protocol_ie_single_container_s<asn1::f1ap::cells_to_be_activ_list_item_ies_o> resp_cell;
    resp_cell->cells_to_be_activ_list_item().nrpci_present = true;
    resp_cell->cells_to_be_activ_list_item().nrpci         = du_cell.pci;
    // TODO: write converter to asn1::f1ap::nrcgi_s
    // resp_cell->cells_to_be_activ_list_item().nrcgi = to_nrcgi_s(du_cell.cgi);
    response->cells_to_be_activ_list.value.push_back(resp_cell);
  }
}

} // namespace srs_cu_cp

} // namespace srsgnb

#endif // SRSGNB_CU_CP_F1C_ASN1_HELPERS_H