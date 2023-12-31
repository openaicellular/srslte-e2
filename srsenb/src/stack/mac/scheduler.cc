/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include <srsenb/hdr/stack/mac/scheduler_ue.h>
#include <string.h>

#include "srsenb/hdr/stack/mac/scheduler.h"
#include "srsenb/hdr/stack/mac/scheduler_carrier.h"
#include "srslte/common/logmap.h"
#include "srslte/srslte.h"

#define Console(fmt, ...) srslte::console(fmt, ##__VA_ARGS__)
#define Error(fmt, ...) srslte::logmap::get("MAC ")->error(fmt, ##__VA_ARGS__)

using srslte::tti_point;

namespace srsenb {

namespace sched_utils {

uint32_t tti_subtract(uint32_t tti1, uint32_t tti2)
{
  return TTI_SUB(tti1, tti2);
}

uint32_t max_tti(uint32_t tti1, uint32_t tti2)
{
  return ((tti1 - tti2) > 10240 / 2) ? SRSLTE_MIN(tti1, tti2) : SRSLTE_MAX(tti1, tti2);
}

} // namespace sched_utils

/*******************************************************
 *                 Sched Params
 *******************************************************/

bool sched_cell_params_t::set_cfg(uint32_t                             enb_cc_idx_,
                                  const sched_interface::cell_cfg_t&   cfg_,
                                  const sched_interface::sched_args_t& sched_args)
{
  enb_cc_idx = enb_cc_idx_;
  cfg        = cfg_;
  sched_cfg  = &sched_args;

  // Basic cell config checks
  if (cfg.si_window_ms == 0) {
    Error("SCHED: Invalid si-window length 0 ms\n");
    return false;
  }

  bool invalid_prach;
  if (cfg.cell.nof_prb == 6) {
    // PUCCH has to allow space for Msg3
    if (cfg.nrb_pucch > 1) {
      Console("Invalid PUCCH configuration: nrb_pucch=%d does not allow space for Msg3 transmission..\n",
              cfg.nrb_pucch);
      return false;
    }
    // PRACH has to fit within the PUSCH+PUCCH space
    invalid_prach = cfg.prach_freq_offset + 6 > cfg.cell.nof_prb;
  } else {
    // PRACH has to fit within the PUSCH space
    invalid_prach = (cfg.prach_freq_offset + 6) > (cfg.cell.nof_prb - cfg.nrb_pucch) or
                    ((int)cfg.prach_freq_offset < cfg.nrb_pucch);
  }
  if (invalid_prach) {
    Error("Invalid PRACH configuration: frequency offset=%d outside bandwidth limits\n", cfg.prach_freq_offset);
    Console("Invalid PRACH configuration: frequency offset=%d outside bandwidth limits\n", cfg.prach_freq_offset);
    return false;
  }

  // Set derived sched parameters

  // init regs
  regs.reset(new srslte_regs_t{});
  if (srslte_regs_init(regs.get(), cfg.cell) != SRSLTE_SUCCESS) {
    Error("Getting DCI locations\n");
    return false;
  }

  // Compute Common locations for DCI for each CFI
  for (uint32_t cfix = 0; cfix < pdcch_grid_t::MAX_CFI; cfix++) {
    sched_utils::generate_cce_location(regs.get(), &common_locations[cfix], cfix + 1);
  }
  if (common_locations[sched_cfg->max_nof_ctrl_symbols - 1].nof_loc[2] == 0) {
    Error("SCHED: Current cfi=%d is not valid for broadcast (check scheduler.max_nof_ctrl_symbols in conf file).\n",
          sched_cfg->max_nof_ctrl_symbols);
    Console("SCHED: Current cfi=%d is not valid for broadcast (check scheduler.max_nof_ctrl_symbols in conf file).\n",
            sched_cfg->max_nof_ctrl_symbols);
    return false;
  }

  // Compute UE locations for RA-RNTI
  for (uint32_t cfi = 0; cfi < 3; cfi++) {
    for (uint32_t sf_idx = 0; sf_idx < 10; sf_idx++) {
      sched_utils::generate_cce_location(regs.get(), &rar_locations[cfi][sf_idx], cfi + 1, sf_idx);
    }
  }

  // precompute nof cces in PDCCH for each CFI
  for (uint32_t cfix = 0; cfix < nof_cce_table.size(); ++cfix) {
    int ret = srslte_regs_pdcch_ncce(regs.get(), cfix + 1);
    if (ret < 0) {
      Error("SCHED: Failed to calculate the number of CCEs in the PDCCH\n");
      return false;
    }
    nof_cce_table[cfix] = (uint32_t)ret;
  }

  P        = srslte_ra_type0_P(cfg.cell.nof_prb);
  nof_rbgs = srslte::ceil_div(cfg.cell.nof_prb, P);

  return true;
}

/*******************************************************
 *
 * Initialization and sched configuration functions
 *
 *******************************************************/

sched::sched() : log_h(srslte::logmap::get("MAC")) {}

sched::~sched() {}

void sched::init(rrc_interface_mac* rrc_)
{
  rrc = rrc_;

  // Initialize first carrier scheduler
  carrier_schedulers.emplace_back(new carrier_sched{rrc, &ue_db, 0, &sched_results});

  reset();
}

int sched::reset()
{
  std::lock_guard<std::mutex> lock(sched_mutex);
  configured = false;
  for (std::unique_ptr<carrier_sched>& c : carrier_schedulers) {
    c->reset();
  }
  ue_db.clear();
  return 0;
}

void sched::set_sched_cfg(sched_interface::sched_args_t* sched_cfg_)
{
  std::lock_guard<std::mutex> lock(sched_mutex);
  if (sched_cfg_ != nullptr) {
    sched_cfg = *sched_cfg_;
  }
}

int sched::cell_cfg(const std::vector<sched_interface::cell_cfg_t>& cell_cfg)
{
  std::lock_guard<std::mutex> lock(sched_mutex);
  // Setup derived config params
  sched_cell_params.resize(cell_cfg.size());
  for (uint32_t cc_idx = 0; cc_idx < cell_cfg.size(); ++cc_idx) {
    if (not sched_cell_params[cc_idx].set_cfg(cc_idx, cell_cfg[cc_idx], sched_cfg)) {
      return SRSLTE_ERROR;
    }
  }

  // Create remaining cells, if not created yet
  uint32_t prev_size = carrier_schedulers.size();
  carrier_schedulers.resize(sched_cell_params.size());
  for (uint32_t i = prev_size; i < sched_cell_params.size(); ++i) {
    carrier_schedulers[i].reset(new carrier_sched{rrc, &ue_db, i, &sched_results});
  }

  // setup all carriers cfg params
  for (uint32_t i = 0; i < sched_cell_params.size(); ++i) {
#ifdef ENABLE_SLICER
    carrier_schedulers[i]->carrier_cfg(sched_cell_params[i], slicer_workshare);
#else
    carrier_schedulers[i]->carrier_cfg(sched_cell_params[i]);
#endif
  }

  configured = true;

  return 0;
}

/*******************************************************
 *
 * FAPI-like main sched interface. Wrappers to UE object
 *
 *******************************************************/

int sched::ue_cfg(uint16_t rnti, const sched_interface::ue_cfg_t& ue_cfg)
{
  std::lock_guard<std::mutex> lock(sched_mutex);
  // Add or config user
  auto it = ue_db.find(rnti);
  if (it == ue_db.end()) {
    // create new user
    ue_db[rnti].init(rnti, sched_cell_params);
    it = ue_db.find(rnti);
  }
  it->second.set_cfg(ue_cfg);

  return SRSLTE_SUCCESS;
}

int sched::ue_rem(uint16_t rnti)
{
  std::lock_guard<std::mutex> lock(sched_mutex);
  if (ue_db.count(rnti) > 0) {
    ue_db.erase(rnti);
  } else {
    Error("User rnti=0x%x not found\n", rnti);
    return SRSLTE_ERROR;
  }
  return SRSLTE_SUCCESS;
}

bool sched::ue_exists(uint16_t rnti)
{
  return ue_db_access(rnti, [](sched_ue& ue) {}) >= 0;
}

void sched::phy_config_enabled(uint16_t rnti, bool enabled)
{
  // TODO: Check if correct use of last_tti
  ue_db_access(
      rnti, [this, enabled](sched_ue& ue) { ue.phy_config_enabled(last_tti.to_uint(), enabled); }, __PRETTY_FUNCTION__);
}

int sched::bearer_ue_cfg(uint16_t rnti, uint32_t lc_id, sched_interface::ue_bearer_cfg_t* cfg_)
{
  return ue_db_access(rnti, [lc_id, cfg_](sched_ue& ue) { ue.set_bearer_cfg(lc_id, cfg_); });
}

int sched::bearer_ue_rem(uint16_t rnti, uint32_t lc_id)
{
  return ue_db_access(rnti, [lc_id](sched_ue& ue) { ue.rem_bearer(lc_id); });
}

uint32_t sched::get_dl_buffer(uint16_t rnti)
{
  // TODO: Check if correct use of last_tti
  uint32_t ret = SRSLTE_ERROR;
  ue_db_access(rnti, [&ret](sched_ue& ue) { ret = ue.get_pending_dl_new_data(); }, __PRETTY_FUNCTION__);
  return ret;
}

uint32_t sched::get_ul_buffer(uint16_t rnti)
{
  // TODO: Check if correct use of last_tti
  uint32_t ret = SRSLTE_ERROR;
  ue_db_access(rnti,
               [this, &ret](sched_ue& ue) { ret = ue.get_pending_ul_new_data(last_tti.to_uint(), -1); },
               __PRETTY_FUNCTION__);
  return ret;
}

uint64_t sched::get_dl_rbg_total(uint16_t rnti)
{
  uint64_t ret = SRSLTE_ERROR;
  ue_db_access(rnti, [&ret](sched_ue& ue) { ret = ue.get_dl_rbg_total(); }, __PRETTY_FUNCTION__);
  return ret;
}

uint64_t sched::get_ul_rb_total(uint16_t rnti)
{
  uint64_t ret = SRSLTE_ERROR;
  ue_db_access(rnti, [&ret](sched_ue& ue) { ret = ue.get_ul_rb_total(); }, __PRETTY_FUNCTION__);
  return ret;
}

int sched::dl_rlc_buffer_state(uint16_t rnti, uint32_t lc_id, uint32_t tx_queue, uint32_t retx_queue)
{
  return ue_db_access(rnti, [&](sched_ue& ue) { ue.dl_buffer_state(lc_id, tx_queue, retx_queue); });
}

int sched::dl_mac_buffer_state(uint16_t rnti, uint32_t ce_code, uint32_t nof_cmds)
{
  return ue_db_access(rnti, [ce_code, nof_cmds](sched_ue& ue) { ue.mac_buffer_state(ce_code, nof_cmds); });
}

int sched::dl_ack_info(uint32_t tti, uint16_t rnti, uint32_t enb_cc_idx, uint32_t tb_idx, bool ack)
{
  int ret = -1;
  ue_db_access(rnti, [&](sched_ue& ue) { ret = ue.set_ack_info(tti, enb_cc_idx, tb_idx, ack); }, __PRETTY_FUNCTION__);
  return ret;
}

int sched::ul_crc_info(uint32_t tti_rx, uint16_t rnti, uint32_t enb_cc_idx, bool crc)
{
  return ue_db_access(
      rnti, [tti_rx, enb_cc_idx, crc](sched_ue& ue) { ue.set_ul_crc(srslte::tti_point{tti_rx}, enb_cc_idx, crc); });
}

int sched::dl_ri_info(uint32_t tti, uint16_t rnti, uint32_t enb_cc_idx, uint32_t ri_value)
{
  return ue_db_access(rnti, [tti, enb_cc_idx, ri_value](sched_ue& ue) { ue.set_dl_ri(tti, enb_cc_idx, ri_value); });
}

int sched::dl_pmi_info(uint32_t tti, uint16_t rnti, uint32_t enb_cc_idx, uint32_t pmi_value)
{
  return ue_db_access(rnti, [tti, enb_cc_idx, pmi_value](sched_ue& ue) { ue.set_dl_pmi(tti, enb_cc_idx, pmi_value); });
}

int sched::dl_cqi_info(uint32_t tti, uint16_t rnti, uint32_t enb_cc_idx, uint32_t cqi_value)
{
  return ue_db_access(rnti, [tti, enb_cc_idx, cqi_value](sched_ue& ue) { ue.set_dl_cqi(tti, enb_cc_idx, cqi_value); });
}

int sched::dl_rach_info(uint32_t enb_cc_idx, dl_sched_rar_info_t rar_info)
{
  std::lock_guard<std::mutex> lock(sched_mutex);
  return carrier_schedulers[enb_cc_idx]->dl_rach_info(rar_info);
}

int sched::ul_cqi_info(uint32_t tti, uint16_t rnti, uint32_t enb_cc_idx, uint32_t cqi, uint32_t ul_ch_code)
{
  return ue_db_access(rnti, [&](sched_ue& ue) { ue.set_ul_cqi(tti, enb_cc_idx, cqi, ul_ch_code); });
}

int sched::ul_bsr(uint16_t rnti, uint32_t lcg_id, uint32_t bsr)
{
  return ue_db_access(rnti, [lcg_id, bsr](sched_ue& ue) { ue.ul_buffer_state(lcg_id, bsr); });
}

int sched::ul_buffer_add(uint16_t rnti, uint32_t lcid, uint32_t bytes)
{
  return ue_db_access(rnti, [lcid, bytes](sched_ue& ue) { ue.ul_buffer_add(lcid, bytes); });
}

int sched::ul_phr(uint16_t rnti, int phr)
{
  return ue_db_access(rnti, [phr](sched_ue& ue) { ue.ul_phr(phr); }, __PRETTY_FUNCTION__);
}

int sched::ul_sr_info(uint32_t tti, uint16_t rnti)
{
  return ue_db_access(rnti, [](sched_ue& ue) { ue.set_sr(); }, __PRETTY_FUNCTION__);
}

void sched::set_dl_tti_mask(uint8_t* tti_mask, uint32_t nof_sfs)
{
  std::lock_guard<std::mutex> lock(sched_mutex);
  carrier_schedulers[0]->set_dl_tti_mask(tti_mask, nof_sfs);
}

void sched::tpc_inc(uint16_t rnti)
{
  ue_db_access(rnti, [](sched_ue& ue) { ue.tpc_inc(); }, __PRETTY_FUNCTION__);
}

void sched::tpc_dec(uint16_t rnti)
{
  ue_db_access(rnti, [](sched_ue& ue) { ue.tpc_dec(); }, __PRETTY_FUNCTION__);
}

std::array<int, SRSLTE_MAX_CARRIERS> sched::get_enb_ue_cc_map(uint16_t rnti)
{
  std::array<int, SRSLTE_MAX_CARRIERS> ret{};
  ret.fill(-1); // -1 for inactive carriers
  ue_db_access(rnti,
               [this, &ret](sched_ue& ue) {
                 for (size_t enb_cc_idx = 0; enb_cc_idx < carrier_schedulers.size(); ++enb_cc_idx) {
                   auto p = ue.get_active_cell_index(enb_cc_idx);
                   if (p.second < SRSLTE_MAX_CARRIERS) {
                     ret[enb_cc_idx] = p.second;
                   }
                 }
               },
               __PRETTY_FUNCTION__);
  return ret;
}

#ifdef ENABLE_SLICER
void sched::set_ue_slice_status(uint16_t rnti, uint8_t status)
{
  ue_db_access(rnti, [status](sched_ue& ue) { ue.set_slice_status(status); });
}

void sched::set_slicer_workshare(bool workshare)
{
  slicer_workshare = workshare;
}
#endif

/*******************************************************
 *
 * Main sched functions
 *
 *******************************************************/

// Downlink Scheduler API
int sched::dl_sched(uint32_t tti_tx_dl, uint32_t enb_cc_idx, sched_interface::dl_sched_res_t& sched_result)
{
  if (!configured) {
    return 0;
  }

  std::lock_guard<std::mutex> lock(sched_mutex);
  if (enb_cc_idx >= carrier_schedulers.size()) {
    return 0;
  }

  tti_point tti_rx = tti_point{tti_tx_dl} - FDD_HARQ_DELAY_UL_MS;
  new_tti(tti_rx);

  // copy result
  sched_result = sched_results.get_sf(tti_rx)->get_cc(enb_cc_idx)->dl_sched_result;

  return 0;
}

// Uplink Scheduler API
int sched::ul_sched(uint32_t tti, uint32_t enb_cc_idx, srsenb::sched_interface::ul_sched_res_t& sched_result)
{
  if (!configured) {
    return 0;
  }

  std::lock_guard<std::mutex> lock(sched_mutex);
  if (enb_cc_idx >= carrier_schedulers.size()) {
    return 0;
  }

  // Compute scheduling Result for tti_rx
  tti_point tti_rx = tti_point{tti} - FDD_HARQ_DELAY_UL_MS - FDD_HARQ_DELAY_DL_MS;
  new_tti(tti_rx);

  // copy result
  sched_result = sched_results.get_sf(tti_rx)->get_cc(enb_cc_idx)->ul_sched_result;

  return SRSLTE_SUCCESS;
}

/// Generate scheduling decision for tti_rx, if it wasn't already generated
/// NOTE: The scheduling decision is made for all CCs in a single call/lock, otherwise the UE can have different
///       configurations (e.g. different set of activated SCells) in different CC decisions
void sched::new_tti(tti_point tti_rx)
{
  last_tti = std::max(last_tti, tti_rx);

  // Generate sched results for all CCs, if not yet generated
  for (size_t cc_idx = 0; cc_idx < carrier_schedulers.size(); ++cc_idx) {
    if (not is_generated(tti_rx, cc_idx)) {
      // Setup tti-specific vars of the UE
      for (auto& user : ue_db) {
        user.second.new_tti(tti_rx);
      }

      // Generate carrier scheduling result
      carrier_schedulers[cc_idx]->generate_tti_result(tti_rx);
    }
  }
}

/// Check if TTI result is generated
bool sched::is_generated(srslte::tti_point tti_rx, uint32_t enb_cc_idx) const
{
  const sf_sched_result* sf_result = sched_results.get_sf(tti_rx);
  return sf_result != nullptr and sf_result->get_cc(enb_cc_idx) != nullptr and
         sf_result->get_cc(enb_cc_idx)->is_generated(tti_rx);
}

// Common way to access ue_db elements in a read locking way
template <typename Func>
int sched::ue_db_access(uint16_t rnti, Func f, const char* func_name)
{
  std::lock_guard<std::mutex> lock(sched_mutex);
  auto                        it = ue_db.find(rnti);
  if (it != ue_db.end()) {
    f(it->second);
  } else {
    if (func_name != nullptr) {
      Error("User rnti=0x%x not found. Failed to call %s.\n", rnti, func_name);
    } else {
      Error("User rnti=0x%x not found.\n", rnti);
    }
    return SRSLTE_ERROR;
  }
  return SRSLTE_SUCCESS;
}

/*******************************************************
 *
 * Helper functions and common data types
 *
 *******************************************************/

void sched_cell_params_t::regs_deleter::operator()(srslte_regs_t* p)
{
  if (p != nullptr) {
    srslte_regs_free(p);
    delete p;
  }
}

rbg_interval rbg_interval::prbs_to_rbgs(const prb_interval& prbs, uint32_t P)
{
  return rbg_interval{srslte::ceil_div(prbs.start(), P), srslte::ceil_div(prbs.stop(), P)};
}

prb_interval prb_interval::rbgs_to_prbs(const rbg_interval& rbgs, uint32_t P)
{
  return prb_interval{rbgs.start() * P, rbgs.stop() * P};
}

rbg_interval rbg_interval::rbgmask_to_rbgs(const rbgmask_t& mask)
{
  int rb_start = -1;
  for (uint32_t i = 0; i < mask.size(); i++) {
    if (rb_start == -1) {
      if (mask.test(i)) {
        rb_start = i;
      }
    } else {
      if (!mask.test(i)) {
        return rbg_interval(rb_start, i);
      }
    }
  }
  if (rb_start != -1) {
    return rbg_interval(rb_start, mask.size());
  } else {
    return rbg_interval();
  }
}

prb_interval prb_interval::riv_to_prbs(uint32_t riv, uint32_t nof_prbs, int nof_vrbs)
{
  if (nof_vrbs < 0) {
    nof_vrbs = nof_prbs;
  }
  uint32_t rb_start, l_crb;
  srslte_ra_type2_from_riv(riv, &l_crb, &rb_start, nof_prbs, (uint32_t)nof_vrbs);
  return {rb_start, rb_start + l_crb};
}

#ifdef ENABLE_ZYLINIUM
bool sched::set_blocked_rbgmask(rbgmask_t& mask)
{
  bool ret = true;
  for (int i = 0; i < (int)carrier_schedulers.size(); i++)
    ret |= carrier_schedulers[i]->set_blocked_rbgmask(mask);
  return ret;
}

bool sched::set_blocked_prbmask(prbmask_t& mask)
{
  bool ret = true;
  for (int i = 0; i < (int)carrier_schedulers.size(); i++)
    ret |= carrier_schedulers[i]->set_blocked_prbmask(mask);
  return ret;
}
#endif

namespace sched_utils {

void generate_cce_location(srslte_regs_t*   regs_,
                           sched_dci_cce_t* location,
                           uint32_t         cfi,
                           uint32_t         sf_idx,
                           uint16_t         rnti)
{
  *location = {};

  srslte_dci_location_t loc[64];
  uint32_t              nloc = 0;
  if (rnti == 0) {
    nloc = srslte_pdcch_common_locations_ncce(srslte_regs_pdcch_ncce(regs_, cfi), loc, 64);
  } else {
    nloc = srslte_pdcch_ue_locations_ncce(srslte_regs_pdcch_ncce(regs_, cfi), loc, 64, sf_idx, rnti);
  }

  // Save to different format
  for (uint32_t i = 0; i < nloc; i++) {
    uint32_t l                                   = loc[i].L;
    location->cce_start[l][location->nof_loc[l]] = loc[i].ncce;
    location->nof_loc[l]++;
  }
}

} // namespace sched_utils

} // namespace srsenb

#ifdef ENABLE_ZYLINIUM
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <bitset>
//For debugging
#include <signal.h>

namespace srsenb {

namespace sched_utils {

const char* hex_char_to_bin(char c)
{
  // TODO handle default / error
  switch(toupper(c))
  {
  case '0': return "0000";
  case '1': return "0001";
  case '2': return "0010";
  case '3': return "0011";
  case '4': return "0100";
  case '5': return "0101";
  case '6': return "0110";
  case '7': return "0111";
  case '8': return "1000";
  case '9': return "1001";
  case 'A': return "1010";
  case 'B': return "1011";
  case 'C': return "1100";
  case 'D': return "1101";
  case 'E': return "1110";
  case 'F': return "1111";
  }
  return "XXXX";
}

// Hex string should have least significant bit be the start of the mask
std::string hex_str_to_bin_str(const std::string& hex, int output_length, srslte::log_ref log_h)
{
  if (hex[0] != '0' || hex[1] != 'x') {
    log_h->error("Illegal mask string %s not given in hex format", hex.c_str());
    return std::string();
  }

  if (((signed)hex.length() - 2) * 4 > output_length + 3) {
    log_h->error("Illegal output length of %d requested from string %s of length %d original length %d",
		 output_length, hex.c_str(), ((signed) hex.length()-2)*4, (signed) hex.length());
    return std::string();
  }

  std::string bin_reversed;
  std::string bin;

  // Convert hex chars to binary
  for (unsigned int i = 2; i < hex.length(); ++i)
    bin_reversed += hex_char_to_bin(hex[i]);

  // Reverse the binary string back
  for (int i = (signed)bin_reversed.length() - 1; i >= 0; i--)
    bin += bin_reversed[i];

  // If the leading hex char has leading zeroes in the binary representation
  // that should be cut off because of the output length, cut them off
  bin = bin.substr(0, output_length);

  // Zero pad the end if need be
  for (int i = bin.length(); i < output_length; i++)
    bin += "0";

  return bin;
}

bool hex_str_to_rbgmask(const std::string& s, srsenb::rbgmask_t& mask, srslte::log_ref log_h)
{
  // rbgmask_string should be a hex string with the least significant binary bit as the first rbg
  std::string rbgmask_string_binary = hex_str_to_bin_str(s, 25, log_h);
  log_h->debug("rbgmask binary %s\n",rbgmask_string_binary.c_str());

  for (int32_t i = 0; i < 25; i++) {
    if (rbgmask_string_binary[i] == '1')
      mask.set(i);
    else if (rbgmask_string_binary[i] == '0')
      mask.reset(i);
    else {
      log_h->error("Illegal character %c at index %d in binary blocked_rbgmask string %s, hex version %s\n",
		   rbgmask_string_binary[i],  i, rbgmask_string_binary.c_str(), s.c_str());
      return false;
    }
  }
  return true;
}

bool hex_str_to_prbmask(const std::string& s, srsenb::prbmask_t& mask, srslte::log_ref log_h)
{
  std::string prbmask_string_binary = hex_str_to_bin_str(s, 100, log_h);
  log_h->debug("prbmask binary %s\n",prbmask_string_binary.c_str());

  // Need to go backwards because the mask is in reverse order, for some reason
  for (int i = 0; i < 100; i++) {
    if (prbmask_string_binary[i] == '1')
      mask.set(i);
    else if (prbmask_string_binary[i] == '0')
      mask.reset(i);
    else {
      log_h->error("Illegal character %c at index %d in binary blocked_prbmask string %s, hex version %s\n",
		   prbmask_string_binary[i],  i, prbmask_string_binary.c_str(), s.c_str());
      return false;
    }
  }
  return true;
}

}

}
#endif
