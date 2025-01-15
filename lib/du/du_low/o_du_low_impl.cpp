/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "o_du_low_impl.h"
#include "srsran/fapi_adaptor/phy/phy_fapi_sector_adaptor.h"
#include "srsran/phy/upper/upper_phy.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;
using namespace srs_du;

o_du_low_impl::o_du_low_impl(std::unique_ptr<du_low>                         du_lo_,
                             std::unique_ptr<fapi_adaptor::phy_fapi_adaptor> fapi_adaptor_,
                             unsigned                                        nof_cells) :
  du_lo(std::move(du_lo_)), fapi_adaptor(std::move(fapi_adaptor_))
{
  srsran_assert(du_lo, "Invalid upper PHY");
  srsran_assert(fapi_adaptor, "Invalid FAPI adaptor");

  for (unsigned i = 0; i != nof_cells; ++i) {
    upper_phy&                             upper          = du_lo->get_upper_phy(i);
    fapi_adaptor::phy_fapi_sector_adaptor& sector_adaptor = fapi_adaptor->get_sector_adaptor(i);

    upper.set_rx_results_notifier(sector_adaptor.get_rx_results_notifier());
    upper.set_timing_notifier(sector_adaptor.get_timing_notifier());
    upper.set_error_notifier(sector_adaptor.get_error_notifier());
  }
}

du_low& o_du_low_impl::get_du_low()
{
  return *du_lo;
}

fapi_adaptor::phy_fapi_adaptor& o_du_low_impl::get_phy_fapi_adaptor()
{
  return *fapi_adaptor;
}

void o_du_low_impl::start()
{
  // Nothing to do as the FAPI adaptor and DU low are stateless.
}

void o_du_low_impl::stop()
{
  du_lo->get_operation_controller().stop();
}
