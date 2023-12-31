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

#include "srsenb/hdr/stack/enb_stack_lte.h"
#include "srsenb/hdr/enb.h"
#include "srslte/common/network_utils.h"
#include "srslte/srslte.h"
#include <srslte/interfaces/enb_metrics_interface.h>
#ifdef ENABLE_SLICER
#include <srslte/interfaces/enb_slicer_interface.h>
#endif

using namespace srslte;

namespace srsenb {

enb_stack_lte::enb_stack_lte(srslte::logger* logger_) :
  task_sched(512, 0, 128),
  logger(logger_),
  pdcp(&task_sched, "PDCP"),
  thread("STACK"),
  mac(&task_sched),
  s1ap(&task_sched),
  rrc(&task_sched)
{
  enb_task_queue  = task_sched.make_task_queue();
  mme_task_queue  = task_sched.make_task_queue();
  gtpu_task_queue = task_sched.make_task_queue();
  // sync_queue is added in init()

  pool = byte_buffer_pool::get_instance();
}

enb_stack_lte::~enb_stack_lte()
{
  stop();
}

std::string enb_stack_lte::get_type()
{
  return "lte";
}

int enb_stack_lte::init(const stack_args_t& args_, const rrc_cfg_t& rrc_cfg_, phy_interface_stack_lte* phy_)
{
  phy = phy_;
  if (init(args_, rrc_cfg_)) {
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

int enb_stack_lte::init(const stack_args_t& args_, const rrc_cfg_t& rrc_cfg_)
{
  args    = args_;
  rrc_cfg = rrc_cfg_;

  // setup logging for each layer
  srslte::logmap::register_log(std::unique_ptr<srslte::log>{new log_filter{"MAC ", logger, true}});
  mac_log->set_level(args.log.mac_level);
  mac_log->set_hex_limit(args.log.mac_hex_limit);

  // Init logs
  rlc_log->set_level(args.log.rlc_level);
  pdcp_log->set_level(args.log.pdcp_level);
  rrc_log->set_level(args.log.rrc_level);
  gtpu_log->set_level(args.log.gtpu_level);
  s1ap_log->set_level(args.log.s1ap_level);
  stack_log->set_level(args.log.stack_level);

  rlc_log->set_hex_limit(args.log.rlc_hex_limit);
  pdcp_log->set_hex_limit(args.log.pdcp_hex_limit);
  rrc_log->set_hex_limit(args.log.rrc_hex_limit);
  gtpu_log->set_hex_limit(args.log.gtpu_hex_limit);
  s1ap_log->set_hex_limit(args.log.s1ap_hex_limit);
  stack_log->set_hex_limit(args.log.stack_hex_limit);

  // Set up pcap and trace
  if (args.mac_pcap.enable) {
    mac_pcap.open(args.mac_pcap.filename.c_str());
    mac.start_pcap(&mac_pcap);
  }
  if (args.s1ap_pcap.enable) {
    s1ap_pcap.open(args.s1ap_pcap.filename.c_str());
    s1ap.start_pcap(&s1ap_pcap);
  }

  // Init Rx socket handler
  rx_sockets.reset(new srslte::rx_multisocket_handler("ENBSOCKETS", stack_log));

  // add sync queue
  sync_task_queue = task_sched.make_task_queue(args.sync_queue_size);

  // Init all layers
  mac.init(args.mac, rrc_cfg.cell_list, phy, &rlc, &rrc, mac_log);
  rlc.init(&pdcp, &rrc, &mac, task_sched.get_timer_handler(), rlc_log);
  pdcp.init(&rlc, &rrc, &gtpu);
  rrc.init(rrc_cfg, phy, &mac, &rlc, &pdcp, &s1ap, &gtpu);
  if (s1ap.init(args.s1ap, &rrc, this) != SRSLTE_SUCCESS) {
    stack_log->error("Couldn't initialize S1AP\n");
    return SRSLTE_ERROR;
  }
  if (gtpu.init(args.s1ap.gtp_bind_addr,
                args.s1ap.mme_addr,
                args.embms.m1u_multiaddr,
                args.embms.m1u_if_addr,
                &pdcp,
                this,
                args.embms.enable)) {
    stack_log->error("Couldn't initialize GTPU\n");
    return SRSLTE_ERROR;
  }

  started = true;
  start(STACK_MAIN_THREAD_PRIO);

  return SRSLTE_SUCCESS;
}

void enb_stack_lte::tti_clock()
{
  sync_task_queue.push([this]() { tti_clock_impl(); });
}

void enb_stack_lte::tti_clock_impl()
{
  task_sched.tic();
  rrc.tti_clock();
}

void enb_stack_lte::stop()
{
  if (started) {
    enb_task_queue.push([this]() { stop_impl(); });
    wait_thread_finish();
  }
}

void enb_stack_lte::stop_impl()
{
  rx_sockets->stop();

  s1ap.stop();
  gtpu.stop();
  mac.stop();
  rlc.stop();
  pdcp.stop();
  rrc.stop();

  if (args.mac_pcap.enable) {
    mac_pcap.close();
  }
  if (args.s1ap_pcap.enable) {
    s1ap_pcap.close();
  }

  task_sched.stop();

  started = false;
}

bool enb_stack_lte::get_metrics(stack_metrics_t* metrics)
{
  // use stack thread to query metrics
  auto ret = enb_task_queue.try_push([this]() {
    stack_metrics_t metrics{};
    mac.get_metrics(metrics.mac);
    rrc.get_metrics(metrics.rrc);
#ifdef ENABLE_RIC_AGENT_KPM
    pdcp.get_metrics(metrics.pdcp);
#endif
    s1ap.get_metrics(metrics.s1ap);
    pending_stack_metrics.push(metrics);
  });

  if (ret.first) {
    // wait for result
    *metrics = pending_stack_metrics.wait_pop();
    return true;
  }
  return false;
}

void enb_stack_lte::run_thread()
{
  while (started) {
    task_sched.run_next_task();
  }
}

void enb_stack_lte::handle_mme_rx_packet(srslte::unique_byte_buffer_t pdu,
                                         const sockaddr_in&           from,
                                         const sctp_sndrcvinfo&       sri,
                                         int                          flags)
{
  // Defer the handling of MME packet to eNB stack main thread
  auto task_handler = [this, from, sri, flags](srslte::unique_byte_buffer_t& t) {
    s1ap.handle_mme_rx_msg(std::move(t), from, sri, flags);
  };
  // Defer the handling of MME packet to main stack thread
  mme_task_queue.push(std::bind(task_handler, std::move(pdu)));
}

void enb_stack_lte::add_mme_socket(int fd)
{
  // Pass MME Rx packet handler functor to socket handler to run in socket thread
  auto mme_rx_handler =
      [this](srslte::unique_byte_buffer_t pdu, const sockaddr_in& from, const sctp_sndrcvinfo& sri, int flags) {
        handle_mme_rx_packet(std::move(pdu), from, sri, flags);
      };
  rx_sockets->add_socket_sctp_pdu_handler(fd, mme_rx_handler);
}

void enb_stack_lte::remove_mme_socket(int fd)
{
  rx_sockets->remove_socket(fd);
}

void enb_stack_lte::add_gtpu_s1u_socket_handler(int fd)
{
  auto gtpu_s1u_handler = [this](srslte::unique_byte_buffer_t pdu, const sockaddr_in& from) {
    auto task_handler = [this, from](srslte::unique_byte_buffer_t& t) {
      gtpu.handle_gtpu_s1u_rx_packet(std::move(t), from);
    };
    gtpu_task_queue.push(std::bind(task_handler, std::move(pdu)));
  };
  rx_sockets->add_socket_pdu_handler(fd, gtpu_s1u_handler);
}

void enb_stack_lte::add_gtpu_m1u_socket_handler(int fd)
{
  auto gtpu_m1u_handler = [this](srslte::unique_byte_buffer_t pdu, const sockaddr_in& from) {
    auto task_handler = [this, from](srslte::unique_byte_buffer_t& t) {
      gtpu.handle_gtpu_m1u_rx_packet(std::move(t), from);
    };
    gtpu_task_queue.push(std::bind(task_handler, std::move(pdu)));
  };
  rx_sockets->add_socket_pdu_handler(fd, gtpu_m1u_handler);
}

#ifdef ENABLE_SLICER
// eNodeB slicer interface
bool enb_stack_lte::slice_config(std::vector<slicer::slice_config_t> slice_configs)
{
  return mac.slicer.slice_config(slice_configs);
}

bool enb_stack_lte::slice_delete(std::vector<std::string> slice_names)
{
  return mac.slicer.slice_delete(slice_names);
}

std::vector<slicer::slice_status_t> enb_stack_lte::slice_status(std::vector<std::string> slice_names)
{
  return mac.slicer.slice_status(slice_names);
}

bool enb_stack_lte::slice_ue_bind(std::string slice_name, std::vector<uint64_t> imsi_list)
{
  return mac.slicer.slice_ue_bind(slice_name, imsi_list);
}

bool enb_stack_lte::slice_ue_unbind(std::string slice_name, std::vector<uint64_t> imsi_list)
{
  return mac.slicer.slice_ue_unbind(slice_name, imsi_list);
}

std::map<std::string,std::vector<uint16_t>> enb_stack_lte::get_slice_map()
{
  return mac.slicer.get_slice_map();
}
#endif

#ifdef ENABLE_ZYLINIUM
bool enb_stack_lte::set_blocked_rbgmask(rbgmask_t& blocked_rbgmask)
{
  return mac.set_blocked_rbgmask(blocked_rbgmask);
}

bool enb_stack_lte::set_blocked_prbmask(prbmask_t& blocked_prbmask)
{
  return mac.set_blocked_prbmask(blocked_prbmask);
}
#endif

} // namespace srsenb
