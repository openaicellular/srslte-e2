
#include "srslte/interfaces/enb_metrics_interface.h"
#include "srsenb/hdr/stack/rrc/rrc_metrics.h"
#include "srsenb/hdr/stack/upper/common_enb.h"
#include "srsenb/hdr/ric/e2ap.h"
#include "srsenb/hdr/ric/e2sm.h"
#include "srsenb/hdr/ric/agent.h"
#include "srsenb/hdr/ric/agent_asn1.h"
#include "srsenb/hdr/ric/e2sm_kpm.h"
#include "srsenb/hdr/ric/e2ap_encode.h"
#include "srsenb/hdr/ric/e2ap_decode.h"
#include "srsenb/hdr/ric/e2ap_generate.h"

#include "E2AP_Cause.h"
#include "E2AP_RICactionType.h"
#include "E2AP_RICindicationType.h"
#include "E2SM_KPM_E2SM-KPM-RANfunction-Description.h"
#include "E2SM_KPM_RIC-ReportStyle-List.h"
#include "E2SM_KPM_RIC-EventTriggerStyle-List.h"
#include "E2SM_KPM_RT-Period-IE.h"
#include "E2SM_KPM_E2SM-KPM-EventTriggerDefinition.h"
#include "E2SM_KPM_Trigger-ConditionIE-Item.h"
#include "E2SM_KPM_E2SM-KPM-IndicationHeader.h"
#include "E2SM_KPM_E2SM-KPM-IndicationMessage.h"
#include "E2SM_KPM_PM-Containers-List.h"
#include "E2SM_KPM_PF-Container.h"
#include "E2SM_KPM_CellResourceReportListItem.h"
#include "E2SM_KPM_ServedPlmnPerCellListItem.h"
#include "E2SM_KPM_EPC-DU-PM-Container.h"
#include "E2SM_KPM_PF-ContainerListItem.h"
#include "E2SM_KPM_PlmnID-List.h"
#include "E2SM_KPM_EPC-CUUP-PM-Format.h"
#include "E2SM_KPM_PerQCIReportListItemFormat.h"
#include "E2SM_KPM_PerUEReportListItemFormat.h"
#include "E2SM_KPM_PerQCIReportListItem.h"
#include "E2SM_KPM_PerUEReportListItem.h"
#ifdef ENABLE_SLICER
#include "E2SM_KPM_PerSliceReportListItemFormat.h"
#include "E2SM_KPM_PerSliceReportListItem.h"
#endif

namespace ric {
namespace kpm {

#ifdef ENABLE_SLICER
void slice_metrics::reset()
{
  slices.clear();
}

void slice_metrics::update(metrics *m,std::map<std::string,std::vector<uint16_t>> slice_map)
{
  std::map<uint16_t,std::string> rev_map;
  std::map<std::string,entity_metrics_t> slice_counters;
  for (auto it = slice_map.begin(); it != slice_map.end(); ++it) {
    slices[it->first] = entity_metrics_t{};
    slice_counters[it->first] = entity_metrics_t{};
    for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2)
      rev_map[*it2] = std::string(it->first);
  }
  slices.clear();
  for (auto it = m->ues.begin(); it != m->ues.end(); ++it) {
    if (rev_map.count(it->first) < 1)
      continue;
    slices[rev_map[it->first]].dl_bytes += it->second.dl_bytes;
    slices[rev_map[it->first]].ul_bytes += it->second.ul_bytes;
    slices[rev_map[it->first]].dl_prbs += it->second.dl_prbs;
    slices[rev_map[it->first]].ul_prbs += it->second.ul_prbs;

    slices[rev_map[it->first]].tx_pkts += it->second.tx_pkts;
    slices[rev_map[it->first]].tx_errors += it->second.tx_errors;
    slices[rev_map[it->first]].rx_pkts += it->second.rx_pkts;
    slices[rev_map[it->first]].rx_errors += it->second.rx_errors;

    // Many of these averages lack meaning, but we'll compute them anyway.
    slices[rev_map[it->first]].tx_brate += it->second.tx_brate;
    slices[rev_map[it->first]].rx_brate += it->second.rx_brate;
    if (!isnan(it->second.dl_cqi)) {
      slices[rev_map[it->first]].dl_cqi += it->second.dl_cqi;
      slice_counters[rev_map[it->first]].dl_cqi += 1;
    }
    if (!isnan(it->second.dl_ri)) {
      slices[rev_map[it->first]].dl_ri += it->second.dl_ri;
      slice_counters[rev_map[it->first]].dl_ri += 1;
    }
    if (!isnan(it->second.dl_pmi)) {
      slices[rev_map[it->first]].dl_pmi += it->second.dl_pmi;
      slice_counters[rev_map[it->first]].dl_pmi += 1;
    }
    if (!isnan(it->second.ul_phr)) {
      slices[rev_map[it->first]].ul_phr += it->second.ul_phr;
      slice_counters[rev_map[it->first]].ul_phr += 1;
    }
    if (!isnan(it->second.ul_mcs)) {
      slices[rev_map[it->first]].ul_mcs += it->second.ul_mcs;
      slice_counters[rev_map[it->first]].ul_mcs += 1;
    }
    slices[rev_map[it->first]].ul_samples += it->second.ul_samples;
    if (!isnan(it->second.dl_mcs)) {
      slices[rev_map[it->first]].dl_mcs += it->second.dl_mcs;
      slice_counters[rev_map[it->first]].dl_mcs += 1;
    }
    slices[rev_map[it->first]].dl_samples += it->second.dl_samples;
  }

  // Now compute averages for non-counter values.
  for (auto it = slice_counters.begin(); it != slice_counters.end(); ++it) {
    if (slice_counters[it->first].tx_brate)
      slices[it->first].tx_brate /= slice_counters[it->first].tx_brate;
    if (slice_counters[it->first].rx_brate)
      slices[it->first].rx_brate /= slice_counters[it->first].rx_brate;
    if (slice_counters[it->first].dl_cqi)
      slices[it->first].dl_cqi /= slice_counters[it->first].dl_cqi;
    if (slice_counters[it->first].dl_ri)
      slices[it->first].dl_ri /= slice_counters[it->first].dl_ri;
    if (slice_counters[it->first].dl_pmi)
      slices[it->first].dl_pmi /= slice_counters[it->first].dl_pmi;
    if (slice_counters[it->first].ul_phr)
      slices[it->first].ul_phr /= slice_counters[it->first].ul_phr;
    if (slice_counters[it->first].ul_sinr)
      slices[it->first].ul_sinr /= slice_counters[it->first].ul_sinr;
    if (slice_counters[it->first].ul_mcs)
      slices[it->first].ul_mcs /= slice_counters[it->first].ul_mcs;
    if (slice_counters[it->first].dl_mcs)
      slices[it->first].dl_mcs /= slice_counters[it->first].dl_mcs;
  }
}
#endif

#define DELTA(_outvar,_old,_new,_max)					\
    do {								\
	if (_new < _old)						\
	    _outvar = (_max - _old) + _new;				\
	else								\
	    _outvar = _new - _old;					\
    } while (0);

void metrics::update(srsenb::enb_metrics_t *em)
{
  /* Reset per-period metadata. */
  have_prbs = false;
  active_ue_count = 0;

  /* Reset per-period diffs. */
  memset(dl_bytes_by_qci,0,sizeof(dl_bytes_by_qci));
  memset(ul_bytes_by_qci,0,sizeof(ul_bytes_by_qci));
  ues.clear();

  /* Count per-period things. */
  for (uint16_t i = 0; i < em->stack.rrc.n_ues && i < ENB_METRICS_MAX_USERS; ++i) {
    if (em->stack.rrc.ues[i].state == srsenb::RRC_STATE_REGISTERED)
      ++active_ue_count;
  }

  /* 
   * Update total counters, and generate diffs.
   *
   * NB: we age out RNTIs as soon as they stop appearing in PDCP; that means
   * RRC has called pdcp->rem_user (which corresponds to an RRC disconnect).
   */

  /* A quick accumulator for use in rapid deletion of disconnected UEs. */
  std::map<uint16_t,bool> ues_present;

  /* Handle PDCP counters. */
  for (uint16_t i = 0; i < em->stack.pdcp.n_ues && i < ENB_METRICS_MAX_USERS; ++i) {
    uint16_t rnti = em->stack.pdcp.ues[i].rnti;
    if (rnti == 0)
      continue;
    ues_present[rnti] = true;

    if (total_ues.count(rnti) < 1) {
      entity_metrics_t nm = {
	em->stack.pdcp.ues[i].dl_bytes,
	em->stack.pdcp.ues[i].ul_bytes,
	0,0,0,0
      };
      memcpy(nm.dl_bytes_by_qci,em->stack.pdcp.ues[i].dl_bytes_by_qci,
	     sizeof(nm.dl_bytes_by_qci));
      memcpy(nm.ul_bytes_by_qci,em->stack.pdcp.ues[i].ul_bytes_by_qci,
	     sizeof(nm.ul_bytes_by_qci));
      total_ues[rnti] = ues[rnti] = nm;

      for (int j = 0; j < MAX_NOF_QCI; ++j) {
	dl_bytes_by_qci[j] += em->stack.pdcp.ues[i].dl_bytes_by_qci[j];
	ul_bytes_by_qci[j] += em->stack.pdcp.ues[i].ul_bytes_by_qci[j];
      }
    }
    else {
      DELTA(ues[rnti].dl_bytes,total_ues[rnti].dl_bytes,
	    em->stack.pdcp.ues[i].dl_bytes,UINT64_MAX);
      total_ues[rnti].dl_bytes = em->stack.pdcp.ues[i].dl_bytes;
      DELTA(ues[rnti].ul_bytes,total_ues[rnti].ul_bytes,
	    em->stack.pdcp.ues[i].ul_bytes,UINT64_MAX);
      total_ues[rnti].ul_bytes = em->stack.pdcp.ues[i].ul_bytes;
      for (int j = 0; j < MAX_NOF_QCI; ++j) {
	DELTA(ues[rnti].dl_bytes_by_qci[j],total_ues[rnti].dl_bytes_by_qci[j],
	      em->stack.pdcp.ues[i].dl_bytes_by_qci[j],UINT64_MAX);
	total_ues[rnti].dl_bytes_by_qci[j] = \
	  em->stack.pdcp.ues[i].dl_bytes_by_qci[j];
	dl_bytes_by_qci[j] += ues[rnti].dl_bytes_by_qci[j];
	DELTA(ues[rnti].ul_bytes_by_qci[j],total_ues[rnti].ul_bytes_by_qci[j],
	      em->stack.pdcp.ues[i].ul_bytes_by_qci[j],UINT64_MAX);
	total_ues[rnti].ul_bytes_by_qci[j] = \
	  em->stack.pdcp.ues[i].ul_bytes_by_qci[j];
	ul_bytes_by_qci[j] += ues[rnti].ul_bytes_by_qci[j];
      }
    }
  }

  /*
   * Handle MAC counters.
   *
   * NB: these zero on every read, so this is broken for > 1 subscription.
   * ... except the RB counters are read every time from sched_ue objects,
   * and those counters are total.
   */
  for (uint16_t i = 0; i < ENB_METRICS_MAX_USERS; ++i) {
    uint16_t rnti = em->stack.mac[i].rnti;
    if (rnti == 0)
      continue;
    ues_present[rnti] = true;
    have_prbs = true;

    if (total_ues.count(rnti) < 1) {
      entity_metrics_t nm = {
	0,0,em->stack.mac[i].dl_rb,em->stack.mac[i].ul_rb,0,0
      };
      total_ues[rnti] = ues[rnti] = nm;
    }
    else {
      DELTA(ues[rnti].dl_prbs,total_ues[rnti].dl_prbs,em->stack.mac[i].dl_rb,UINT64_MAX);
      total_ues[rnti].dl_prbs = em->stack.mac[i].dl_rb;
      DELTA(ues[rnti].ul_prbs,total_ues[rnti].ul_prbs,em->stack.mac[i].ul_rb,UINT64_MAX);
      total_ues[rnti].ul_prbs = em->stack.mac[i].ul_rb;
    }

    ues[rnti].tx_pkts = em->stack.mac[i].tx_pkts;
    ues[rnti].tx_errors = em->stack.mac[i].tx_errors;
    ues[rnti].tx_brate = em->stack.mac[i].tx_brate;
    ues[rnti].rx_pkts = em->stack.mac[i].rx_pkts;
    ues[rnti].rx_errors = em->stack.mac[i].rx_errors;
    ues[rnti].rx_brate = em->stack.mac[i].rx_brate;
    if (!isnan(em->stack.mac[i].dl_cqi))
      ues[rnti].dl_cqi = em->stack.mac[i].dl_cqi;
    if (!isnan(em->stack.mac[i].dl_ri))
      ues[rnti].dl_ri = em->stack.mac[i].dl_ri;
    if (!isnan(em->stack.mac[i].dl_pmi))
      ues[rnti].dl_pmi = em->stack.mac[i].dl_pmi;
    if (!isnan(em->stack.mac[i].phr))
      ues[rnti].ul_phr = em->stack.mac[i].phr;
    // Add some phy metrics as well.
    if (!isnan(em->phy[i].ul.sinr))
      ues[rnti].ul_sinr = em->phy[i].ul.sinr;
    if (!isnan(em->phy[i].ul.mcs))
      ues[rnti].ul_mcs = em->phy[i].ul.mcs;
    ues[rnti].ul_samples = em->phy[i].ul.n_samples;
    if (!isnan(em->phy[i].dl.mcs))
      ues[rnti].dl_mcs = em->phy[i].dl.mcs;
    ues[rnti].dl_samples = em->phy[i].dl.n_samples;
  }

  /* Remove stale RNTIs. */
  for (auto it = ues.begin(); it != ues.end(); ++it)
    if (ues_present.count(it->first) < 1)
      ues.erase(it);
  for (auto it = total_ues.begin(); it != total_ues.end(); ++it)
    if (ues_present.count(it->first) < 1)
      total_ues.erase(it);
}

void metrics::reset()
{
  active_ue_count = 0;
  memset(dl_bytes_by_qci,0,sizeof(dl_bytes_by_qci));
  memset(ul_bytes_by_qci,0,sizeof(ul_bytes_by_qci));
  ues.clear();
  total_ues.clear();
}

int e2sm_kpm_period_ie_to_ms(E2SM_KPM_RT_Period_IE_t ie)
{
  switch(ie) {
  case E2SM_KPM_RT_Period_IE_ms10:
    return 10;
  case E2SM_KPM_RT_Period_IE_ms20:
    return 20;
  case E2SM_KPM_RT_Period_IE_ms32:
    return 32;
  case E2SM_KPM_RT_Period_IE_ms40:
    return 40;
  case E2SM_KPM_RT_Period_IE_ms60:
    return 60;
  case E2SM_KPM_RT_Period_IE_ms64:
    return 64;
  case E2SM_KPM_RT_Period_IE_ms70:
    return 70;
  case E2SM_KPM_RT_Period_IE_ms80:
    return 80;
  case E2SM_KPM_RT_Period_IE_ms128:
    return 128;
  case E2SM_KPM_RT_Period_IE_ms160:
    return 160;
  case E2SM_KPM_RT_Period_IE_ms256:
    return 256;
  case E2SM_KPM_RT_Period_IE_ms320:
    return 320;
  case E2SM_KPM_RT_Period_IE_ms512:
    return 512;
  case E2SM_KPM_RT_Period_IE_ms640:
    return 640;
  case E2SM_KPM_RT_Period_IE_ms1024:
    return 1024;
  case E2SM_KPM_RT_Period_IE_ms1280:
    return 1280;
  case E2SM_KPM_RT_Period_IE_ms2048:
    return 2048;
  case E2SM_KPM_RT_Period_IE_ms2560:
    return 2560;
  case E2SM_KPM_RT_Period_IE_ms5120:
    return 5120;
  case E2SM_KPM_RT_Period_IE_ms10240:
    return 10240;
  default:
    break;
  }

  return -1;
}

kpm_model::kpm_model(ric::agent *agent_) :
  service_model(agent_,"ORAN-E2SM-KPM","1.3.6.1.4.1.1.1.2.2"),
  serial_number(1), lock(PTHREAD_MUTEX_INITIALIZER)
{
  for (int i = 0; i < NUM_PERIODS; ++i) {
    periods[i].timer_id = -1;
    periods[i].ms = ric::kpm::e2sm_kpm_period_ie_to_ms((E2SM_KPM_RT_Period_IE_t)i);
  }
}

int kpm_model::init()
{
  ric::ran_function_t *func;
  E2SM_KPM_E2SM_KPM_RANfunction_Description_t *func_def;
  E2SM_KPM_RIC_ReportStyle_List_t *ric_report_style_item;
  E2SM_KPM_RIC_EventTriggerStyle_List_t *ric_event_trigger_style_item;

  E2SM_INFO(agent,"kpm: building function list\n");

  /* Create and encode our function list. */
  func = (ric::ran_function_t *)calloc(1,sizeof(*func));
  func->function_id = get_next_ran_function_id();
  func->model = this;
  func->revision = 0;
  func->name = "ORAN-E2SM-KPM";
  func->description = "KPM monitor";

  func_def = (E2SM_KPM_E2SM_KPM_RANfunction_Description_t *) \
    calloc(1,sizeof(*func_def));

  func_def->ranFunction_Name.ranFunction_ShortName.buf = \
    (uint8_t *)strdup(func->name.c_str());
  func_def->ranFunction_Name.ranFunction_ShortName.size = \
    strlen(func->name.c_str());
  func_def->ranFunction_Name.ranFunction_E2SM_OID.buf = \
    (uint8_t *)strdup(func->model->oid.c_str());
  func_def->ranFunction_Name.ranFunction_E2SM_OID.size = \
    strlen(func->model->oid.c_str());
  func_def->ranFunction_Name.ranFunction_Description.buf = \
    (uint8_t *)strdup(func->description.c_str());
  func_def->ranFunction_Name.ranFunction_Description.size = \
    strlen(func->description.c_str());
  func_def->e2SM_KPM_RANfunction_Item.ric_EventTriggerStyle_List = \
      (struct E2SM_KPM_E2SM_KPM_RANfunction_Description::E2SM_KPM_E2SM_KPM_RANfunction_Description__e2SM_KPM_RANfunction_Item::E2SM_KPM_E2SM_KPM_RANfunction_Description__e2SM_KPM_RANfunction_Item__ric_EventTriggerStyle_List *)calloc(1,sizeof(*func_def->e2SM_KPM_RANfunction_Item.ric_EventTriggerStyle_List));
  ric_event_trigger_style_item = (E2SM_KPM_RIC_EventTriggerStyle_List_t *)calloc(1,sizeof(*ric_event_trigger_style_item));
  ric_event_trigger_style_item->ric_EventTriggerStyle_Type = 1;
  ric_event_trigger_style_item->ric_EventTriggerStyle_Name.buf = (uint8_t *)strdup("Trigger1");
  ric_event_trigger_style_item->ric_EventTriggerStyle_Name.size = strlen("Trigger1");
  ric_event_trigger_style_item->ric_EventTriggerFormat_Type = 1;
  ASN_SEQUENCE_ADD(
    &func_def->e2SM_KPM_RANfunction_Item.ric_EventTriggerStyle_List->list,
    ric_event_trigger_style_item);

  func_def->e2SM_KPM_RANfunction_Item.ric_ReportStyle_List = \
      (struct E2SM_KPM_E2SM_KPM_RANfunction_Description::E2SM_KPM_E2SM_KPM_RANfunction_Description__e2SM_KPM_RANfunction_Item::E2SM_KPM_E2SM_KPM_RANfunction_Description__e2SM_KPM_RANfunction_Item__ric_ReportStyle_List *)calloc(1,sizeof(*func_def->e2SM_KPM_RANfunction_Item.ric_ReportStyle_List));
  ric_report_style_item = (E2SM_KPM_RIC_ReportStyle_List_t *)calloc(1,sizeof(*ric_report_style_item));
  ric_report_style_item->ric_ReportStyle_Type = 6;
  ric_report_style_item->ric_ReportStyle_Name.buf = (uint8_t *)strdup("O-CU-UP Measurement Container for the EPC connected deployment");
  ric_report_style_item->ric_ReportStyle_Name.size = strlen("O-CU-UP Measurement Container for the EPC connected deployment");
  ric_report_style_item->ric_IndicationHeaderFormat_Type = 1;
  ric_report_style_item->ric_IndicationMessageFormat_Type = 1;
  ASN_SEQUENCE_ADD(&func_def->e2SM_KPM_RANfunction_Item.ric_ReportStyle_List->list,
  		   ric_report_style_item);
  
  func->enc_definition_len = ric::e2ap::encode(
    &asn_DEF_E2SM_KPM_E2SM_KPM_RANfunction_Description,0,
    func_def,&func->enc_definition);
  if (func->enc_definition_len < 0) {
    E2SM_ERROR(agent,
      "failed to encode %s function %s!\n",
      name.c_str(),func->name.c_str());
    ASN_STRUCT_FREE_CONTENTS_ONLY(
      asn_DEF_E2SM_KPM_E2SM_KPM_RANfunction_Description,func_def);
    free(func_def);
    free(func);

    return -1;
  }

  func->enabled = 1;
  func->definition = func_def;

  functions.push_back(func);

  queue.start();

  return 0;
}

void kpm_model::stop()
{
  pthread_mutex_lock(&lock);
  queue.stop();
  for (int i = 0; i < NUM_PERIODS; ++i) {
    if (periods[i].timer_id < 0)
      continue;
    periods[i].subscriptions.clear();
    periods[i].last_metrics.reset();
#ifdef ENABLE_SLICER
    periods[i].last_slice_metrics.reset();
#endif
  }
  for (auto it = subscriptions.begin(); it != subscriptions.end(); ++it) {
    ric::subscription_t *sub = *it;
    subscription_model_data *md = (subscription_model_data *)sub->model_data;
    delete md;
    sub->model_data = NULL;
  }
  subscriptions.clear();
  pthread_mutex_unlock(&lock);
}

int kpm_model::handle_subscription_add(ric::subscription_t *sub)
{
  E2SM_KPM_E2SM_KPM_EventTriggerDefinition_t etdef;
  E2SM_KPM_Trigger_ConditionIE_Item *ie,**ptr;
  int sub_period_ms;
  ric::action_t *action;
  bool any_actions_enabled = false;
  subscription_model_data *md;

  if (sub->event_trigger.size < 1 || sub->event_trigger.buf == NULL) {
    E2SM_ERROR(agent,"kpm: no report event trigger; no periods\n");
    return -1;
  }

  md = new subscription_model_data_t{};
  memset(&etdef,0,sizeof(etdef));
  if (ric::e2ap::decode(
	agent,&asn_DEF_E2SM_KPM_E2SM_KPM_EventTriggerDefinition,&etdef,
	sub->event_trigger.buf,sub->event_trigger.size)) {
    E2SM_ERROR(agent,"failed to decode e2sm kpm event trigger definition\n");
    goto errout;
  }
  if (etdef.present != E2SM_KPM_E2SM_KPM_EventTriggerDefinition_PR_eventDefinition_Format1) {
    E2SM_ERROR(agent,"kpm only supports eventDefinition_Format1\n");
    goto errout;
  }
  if (etdef.choice.eventDefinition_Format1.policyTest_List) {
    for (ptr = etdef.choice.eventDefinition_Format1.policyTest_List->list.array;
	 ptr < &etdef.choice.eventDefinition_Format1.policyTest_List->list.array[etdef.choice.eventDefinition_Format1.policyTest_List->list.count];
	 ptr++) {
      ie = (E2SM_KPM_Trigger_ConditionIE_Item *)*ptr;
      sub_period_ms = ric::kpm::e2sm_kpm_period_ie_to_ms(ie->report_Period_IE);
      if (sub_period_ms < 0) {
        E2SM_ERROR(agent,"kpm: invalid report period %d\n",sub_period_ms);
	goto errout;
      }
      md->periods.push_back((int)ie->report_Period_IE);
      //md->periods.push_back((int)E2SM_KPM_RT_Period_IE_ms1024);
    }
  }
  if (md->periods.empty()) {
    E2SM_ERROR(agent,"kpm: invalid report trigger; no periods\n");
    goto errout;
  }

  for (std::list<ric::action_t *>::iterator it = sub->actions.begin(); it != sub->actions.end(); ++it) {
    action = *it;
    if (action->type != E2AP_RICactionType_report) {
	E2SM_WARN(agent,"kpm: invalid actionType %ld; not enabling this action\n",
		  action->type);
      continue;
    }
    action->enabled = true;
    any_actions_enabled = true;
  }
  if (!any_actions_enabled) {
    E2SM_ERROR(agent,"kpm: no actions enabled; failing subscription\n");
    goto errout;
  }

  /* This is a valid subscription; add it. */
  pthread_mutex_lock(&lock);
  subscriptions.push_back(sub);
  for (std::list<int>::iterator it = md->periods.begin(); it != md->periods.end(); ++it) {
    int period = *it;

    periods[period].subscriptions.push_back(sub);
    if (periods[period].timer_id < 0) {
      struct timeval tv = { periods[period].ms / 1000,
			    (periods[period].ms % 1000) * 1000 };
      periods[period].timer_id = queue.insert_periodic(
	tv,timer_callback,this);
      periods[period].last_metrics.reset();
#ifdef ENABLE_SLICER
      periods[period].last_slice_metrics.reset();
#endif
    }
  }
  sub->model_data = md;
  pthread_mutex_unlock(&lock);

  return 0;

 errout:
  pthread_mutex_unlock(&lock);
  delete md;
  ASN_STRUCT_FREE_CONTENTS_ONLY(
    asn_DEF_E2SM_KPM_E2SM_KPM_EventTriggerDefinition,(&etdef));

  return -1;
}

int kpm_model::handle_subscription_del(
  ric::subscription_t *sub,int force,long *cause,long *cause_detail)
{
  subscription_model_data_t *md = (subscription_model_data_t *)sub->model_data;

  pthread_mutex_lock(&lock);

  for (std::list<int>::iterator it = md->periods.begin(); it != md->periods.end(); ++it) {
    int period = *it;

    periods[period].subscriptions.remove(sub);
    if (periods[period].subscriptions.size() == 0) {
      queue.cancel(periods[period].timer_id);
      periods[period].timer_id = -1;
    }
  }
  delete md;
  sub->model_data = NULL;
  subscriptions.remove(sub);

  pthread_mutex_unlock(&lock);

  return 0;
}

void kpm_model::handle_control(ric::control_t *control)
{
  E2SM_ERROR(agent,"kpm: no controls supported\n");
}

void *kpm_model::timer_callback(int timer_id,void *arg)
{
  kpm_model *model = (kpm_model *)arg;
  model->agent->push_task([model,timer_id]() { model->send_indications(timer_id); });
  return NULL;
}

void kpm_model::send_indications(int timer_id)
{
  uint8_t *buf = NULL;
  ssize_t buf_len = 0;
  std::list<ric::subscription_t *>::iterator it;
  ric::subscription_t *sub;
  std::list<ric::action_t *>::iterator it2;
  ric::action_t *action;
  E2SM_KPM_E2SM_KPM_IndicationHeader_t ih;
  E2SM_KPM_E2SM_KPM_IndicationMessage_t im;
  uint8_t *header_buf = NULL;
  ssize_t header_buf_len = 0;
  uint8_t *msg_buf = NULL;
  ssize_t msg_buf_len = 0;
  E2SM_KPM_PM_Containers_List_t *pmc_item;
  E2SM_KPM_PF_Container_t *pf_item;
  E2SM_KPM_CellResourceReportListItem_t *crr_item;
  E2SM_KPM_ServedPlmnPerCellListItem_t *served_item;
  E2SM_KPM_PF_ContainerListItem_t *epc_cu_up_item;
  E2SM_KPM_PerQCIReportListItemFormat_t *epc_cu_up_report_item;
  E2SM_KPM_PlmnID_List_t *epc_cu_up_plmnid_item;
  int period;
  srsenb::enb_metrics_t em;
  metrics *dm;

  /*
   * We would prefer not to be locked while generating asn1, but in this
   * case, we are referencing the per-period metrics during generation,
   * and those must be locked together with the period subscriptions.
   */
  pthread_mutex_lock(&lock);
  for (period = 0; period < NUM_PERIODS; ++period)
    if (timer_id == periods[period].timer_id)
      break;
  if (period == NUM_PERIODS) {
    E2SM_ERROR(agent,"kpm: bogus timer_id %d; ignoring!\n",timer_id);
    pthread_mutex_unlock(&lock);
    return;
  }
  if (periods[period].subscriptions.size() == 0) {
    pthread_mutex_unlock(&lock);
    return;
  }

  E2SM_INFO(agent,"kpm: sending indications for period %d (%d ms)\n",
	    period,periods[period].ms);

  /*
   * First, we grab all the RF data and process it.
   */
  memset(&em,0,sizeof(em));
  agent->enb_metrics_interface->get_metrics(&em);
  periods[period].last_metrics.update(&em);
  dm = &periods[period].last_metrics;
#ifdef ENABLE_SLICER
  periods[period].last_slice_metrics.update(
    dm,agent->enb_slicer_interface->get_slice_map());
  slice_metrics *sm = &periods[period].last_slice_metrics;
#endif

  /*
   * Second, we generate the e2sm-specific stuff.
   *
   * NB: we really need this to be action-specific, because actions can
   * request a particular report style, but since we currently only
   * generate one report style, don't worry for now.
   */
  memset(&ih,0,sizeof(ih));
  ih.present = E2SM_KPM_E2SM_KPM_IndicationHeader_PR_indicationHeader_Format1;
  ih.choice.indicationHeader_Format1.pLMN_Identity = \
    (E2SM_KPM_PLMN_Identity_t *)calloc(1,sizeof(E2SM_KPM_PLMN_Identity_t));
  ASN1_MAKE_PLMNID(
    agent->args.stack.s1ap.mcc,agent->args.stack.s1ap.mnc,
    ih.choice.indicationHeader_Format1.pLMN_Identity);

  E2SM_DEBUG(agent,"indication header:\n");
  E2SM_XER_PRINT(NULL,&asn_DEF_E2SM_KPM_E2SM_KPM_IndicationHeader,&ih);

  header_buf_len = ric::e2ap::encode(
    &asn_DEF_E2SM_KPM_E2SM_KPM_IndicationHeader,0,&ih,&header_buf);
  if (header_buf_len < 0) {
    E2SM_ERROR(agent,"failed to encode indication header; aborting send_indication\n");
    ASN_STRUCT_FREE_CONTENTS_ONLY(
      asn_DEF_E2SM_KPM_E2SM_KPM_IndicationHeader,&ih);
    goto out;
  }
  /*
  memset(&ih,0,sizeof(ih));
  if (ric::e2ap::decode(
	agent,&asn_DEF_E2SM_KPM_E2SM_KPM_IndicationHeader,&ih,
	header_buf,header_buf_len)) {
    E2SM_ERROR(agent,"failed to redecode e2sm ih\n");
    goto out;
  }
  E2SM_DEBUG(agent,"indication header (decoded):\n");
  E2SM_XER_PRINT(NULL,&asn_DEF_E2SM_KPM_E2SM_KPM_IndicationHeader,&ih);
  ASN_STRUCT_FREE_CONTENTS_ONLY(
    asn_DEF_E2SM_KPM_E2SM_KPM_IndicationHeader,&ih);
  */

  memset(&im,0,sizeof(im));
  im.ric_Style_Type = (long)4;
  im.indicationMessage.present = \
    E2SM_KPM_E2SM_KPM_IndicationMessage__indicationMessage_PR_indicationMessage_Format1;

  /*
   * O-DU performance metrics (style 2).
   */
  pmc_item = (E2SM_KPM_PM_Containers_List_t *)calloc(1,sizeof(*pmc_item));
  memset(pmc_item,0,sizeof(*pmc_item));
  pmc_item->performanceContainer = pf_item = (E2SM_KPM_PF_Container_t *)calloc(1,sizeof(*pf_item));
  pf_item->present = E2SM_KPM_PF_Container_PR_oDU;
  /* For each cell: */
  for (uint32_t cc = 0; cc < agent->phy_cfg.phy_cell_cfg.size(); cc++) {
    crr_item = (E2SM_KPM_CellResourceReportListItem_t *)calloc(1,sizeof(*crr_item));
    ASN1_MAKE_PLMNID(
      agent->args.stack.s1ap.mcc,agent->args.stack.s1ap.mnc,
      &crr_item->nRCGI.pLMN_Identity);
    ASN1_MAKE_NRCGI((long)agent->phy_cfg.phy_cell_cfg[cc].cell_id,
		    &crr_item->nRCGI.nRCellIdentity);

    crr_item->dl_TotalofAvailablePRBs = \
      (long *)calloc(1,sizeof(*crr_item->dl_TotalofAvailablePRBs));
    *crr_item->dl_TotalofAvailablePRBs = agent->args.stack.mac.nof_prb;
    crr_item->ul_TotalofAvailablePRBs = \
      (long *)calloc(1,sizeof(*crr_item->ul_TotalofAvailablePRBs));
    *crr_item->ul_TotalofAvailablePRBs = agent->args.stack.mac.nof_prb;

    served_item = (E2SM_KPM_ServedPlmnPerCellListItem *)calloc(1,sizeof(*served_item));
    ASN1_MAKE_PLMNID(
      agent->args.stack.s1ap.mcc,agent->args.stack.s1ap.mnc,
      &served_item->pLMN_Identity);

    /*
     * NB: only do this for cell 0, because we don't get metrics from srslte
     * indexed by cell.
     *
     * NB: for now, we just forge an entry for QCI 0, because we don't have
     * per-QCI RB metrics, but that list must have at least one element.
     */
    E2SM_KPM_EPC_DU_PM_Container_t *duc = \
      (E2SM_KPM_EPC_DU_PM_Container_t *)calloc(1,sizeof(*duc));

    E2SM_KPM_PerQCIReportListItem_t *pqi = \
      (E2SM_KPM_PerQCIReportListItem_t *)calloc(1,sizeof(*pqi));
    ASN_SEQUENCE_ADD(&duc->perQCIReportList.list,pqi);

    served_item->du_PM_EPC = duc;

    if (dm->have_prbs) {
      duc->perUEReportList = (E2SM_KPM_EPC_DU_PM_Container::E2SM_KPM_EPC_DU_PM_Container__perUEReportList *)calloc(1,sizeof(*duc->perUEReportList));
      for (auto it = dm->ues.begin(); it != dm->ues.end(); ++it) {
	E2SM_KPM_PerUEReportListItem_t *pui = \
	  (E2SM_KPM_PerUEReportListItem_t *)calloc(1,sizeof(*pui));
	pui->rnti = it->first;
	asn_uint642INTEGER(&pui->dl_PRBUsage,it->second.dl_prbs);
	asn_uint642INTEGER(&pui->ul_PRBUsage,it->second.ul_prbs);
	pui->tx_pkts = it->second.tx_pkts;
	pui->tx_errors = it->second.tx_errors;
	pui->tx_brate = it->second.tx_brate;
	pui->rx_pkts = it->second.rx_pkts;
	pui->rx_errors = it->second.rx_errors;
	pui->rx_brate = it->second.rx_brate;
	pui->dl_cqi = it->second.dl_cqi;
	pui->dl_ri = it->second.dl_ri;
	pui->dl_pmi = it->second.dl_pmi;
	pui->ul_phr = it->second.ul_phr;
	pui->ul_sinr = it->second.ul_sinr;
	pui->ul_mcs = it->second.ul_mcs;
	pui->ul_samples = it->second.ul_samples;
	pui->dl_mcs = it->second.dl_mcs;
	pui->dl_samples = it->second.dl_samples;
	ASN_SEQUENCE_ADD(&duc->perUEReportList->list,pui);
      }

#ifdef ENABLE_SLICER
      if (sm->slices.size() > 0) {
	  duc->perSliceReportList = \
	    (E2SM_KPM_EPC_DU_PM_Container::E2SM_KPM_EPC_DU_PM_Container__perSliceReportList *)calloc(1,sizeof(*duc->perSliceReportList));
	  for (auto it = sm->slices.begin(); it != sm->slices.end(); ++it) {
	    E2SM_KPM_PerSliceReportListItem_t *slice_item = \
	      (E2SM_KPM_PerSliceReportListItem_t *)calloc(1,sizeof(*slice_item));
	    slice_item->sliceName.buf = (uint8_t *)malloc(it->first.size());
	    slice_item->sliceName.size = it->first.size();
	    strncpy((char *)slice_item->sliceName.buf,it->first.c_str(),
		    slice_item->sliceName.size);
	    asn_uint642INTEGER(&slice_item->dl_PRBUsage,it->second.dl_prbs);
	    asn_uint642INTEGER(&slice_item->ul_PRBUsage,it->second.ul_prbs);
	    asn_uint642INTEGER(&slice_item->dl_PRBUsage,it->second.dl_prbs);
	    slice_item->tx_pkts = it->second.tx_pkts;
	    slice_item->tx_errors = it->second.tx_errors;
	    slice_item->tx_brate = it->second.tx_brate;
	    slice_item->rx_pkts = it->second.rx_pkts;
	    slice_item->rx_errors = it->second.rx_errors;
	    slice_item->rx_brate = it->second.rx_brate;
	    slice_item->dl_cqi = it->second.dl_cqi;
	    slice_item->dl_ri = it->second.dl_ri;
	    slice_item->dl_pmi = it->second.dl_pmi;
	    slice_item->ul_phr = it->second.ul_phr;
	    slice_item->ul_sinr = it->second.ul_sinr;
	    slice_item->ul_mcs = it->second.ul_mcs;
	    slice_item->ul_samples = it->second.ul_samples;
	    slice_item->dl_mcs = it->second.dl_mcs;
	    slice_item->dl_samples = it->second.dl_samples;
	    ASN_SEQUENCE_ADD(&duc->perSliceReportList->list,slice_item);
	  }
      }
#endif
    }

    ASN_SEQUENCE_ADD(&crr_item->servedPlmnPerCellList.list,served_item);
    ASN_SEQUENCE_ADD(&pf_item->choice.oDU.cellResourceReportList.list,crr_item);
  }
  ASN_SEQUENCE_ADD(
    &im.indicationMessage.choice.indicationMessage_Format1.pm_Containers.list,
    pmc_item);

  /*
   * O-CU-CP performance metrics (style 4).
   */
  pmc_item = (E2SM_KPM_PM_Containers_List_t *)calloc(1,sizeof(*pmc_item));
  memset(pmc_item,0,sizeof(*pmc_item));
  pmc_item->performanceContainer = pf_item = (E2SM_KPM_PF_Container_t *)calloc(1,sizeof(*pf_item));
  pf_item->present = E2SM_KPM_PF_Container_PR_oCU_CP;
  pf_item->choice.oCU_CP.cu_CP_Resource_Status.numberOfActive_UEs = (long *)calloc(1,sizeof(long));
  *pf_item->choice.oCU_CP.cu_CP_Resource_Status.numberOfActive_UEs = dm->active_ue_count;
  ASN_SEQUENCE_ADD(
    &im.indicationMessage.choice.indicationMessage_Format1.pm_Containers.list,
    pmc_item);

  /*
   * O-CU-UP performance metrics (style 6).
   */
  pmc_item = (E2SM_KPM_PM_Containers_List_t *)calloc(1,sizeof(*pmc_item));
  memset(pmc_item,0,sizeof(*pmc_item));
  pmc_item->performanceContainer = pf_item = (E2SM_KPM_PF_Container_t *)calloc(1,sizeof(*pf_item));
  pf_item->present = E2SM_KPM_PF_Container_PR_oCU_UP;

  epc_cu_up_item = (E2SM_KPM_PF_ContainerListItem_t *)calloc(1,sizeof(*epc_cu_up_item));
  epc_cu_up_item->interface_type = E2SM_KPM_NI_Type_f1_u;
  epc_cu_up_plmnid_item = (E2SM_KPM_PlmnID_List_t *)calloc(1,sizeof(*epc_cu_up_plmnid_item));
  ASN1_MAKE_PLMNID(
      agent->args.stack.s1ap.mcc,agent->args.stack.s1ap.mnc,
      &epc_cu_up_plmnid_item->pLMN_Identity);

  epc_cu_up_plmnid_item->cu_UP_PM_EPC = (E2SM_KPM_EPC_CUUP_PM_Format *)calloc(1,sizeof(*epc_cu_up_plmnid_item->cu_UP_PM_EPC));
  for (int i = 0; i < MAX_NOF_QCI; ++i) {
    // We must supply at least one item, so even if we have no stats, send
    // 0 bytes for the 0-th and 7th bearers.
    if (!(i == 0 || i == 7)
	&& dm->dl_bytes_by_qci[i] == 0 && dm->ul_bytes_by_qci[i] == 0)
      continue;

    epc_cu_up_report_item = (E2SM_KPM_PerQCIReportListItemFormat_t *)calloc(1,sizeof(*epc_cu_up_report_item));
    epc_cu_up_report_item->qci = i;
    epc_cu_up_report_item->pDCPBytesDL = (INTEGER_t *)calloc(1,sizeof(*epc_cu_up_report_item->pDCPBytesDL));
    epc_cu_up_report_item->pDCPBytesUL = (INTEGER_t *)calloc(1,sizeof(*epc_cu_up_report_item->pDCPBytesUL));
    asn_uint642INTEGER(epc_cu_up_report_item->pDCPBytesDL,dm->dl_bytes_by_qci[i]);
    asn_uint642INTEGER(epc_cu_up_report_item->pDCPBytesUL,dm->ul_bytes_by_qci[i]);
    ASN_SEQUENCE_ADD(&epc_cu_up_plmnid_item->cu_UP_PM_EPC->perQCIReportList.list,
		     epc_cu_up_report_item);
  }

  if (dm->ues.size() > 0) {
    epc_cu_up_plmnid_item->cu_UP_PM_EPC->perUEReportList = \
      (E2SM_KPM_EPC_CUUP_PM_Format::E2SM_KPM_EPC_CUUP_PM_Format__perUEReportList *)calloc(1,sizeof(*epc_cu_up_plmnid_item->cu_UP_PM_EPC->perUEReportList));
    for (auto it = dm->ues.begin(); it != dm->ues.end(); ++it) {
      E2SM_KPM_PerUEReportListItemFormat_t *ue_item = \
	(E2SM_KPM_PerUEReportListItemFormat_t *)calloc(1,sizeof(*ue_item));
      ue_item->rnti = it->first;
      asn_uint642INTEGER(&ue_item->bytesDL,it->second.dl_bytes);
      asn_uint642INTEGER(&ue_item->bytesUL,it->second.ul_bytes);
      ASN_SEQUENCE_ADD(&epc_cu_up_plmnid_item->cu_UP_PM_EPC->perUEReportList->list,
		       ue_item);
    }
  }

#ifdef ENABLE_SLICER
  if (sm->slices.size() > 0) {
    epc_cu_up_plmnid_item->cu_UP_PM_EPC->perSliceReportList = \
      (E2SM_KPM_EPC_CUUP_PM_Format::E2SM_KPM_EPC_CUUP_PM_Format__perSliceReportList *)calloc(1,sizeof(*epc_cu_up_plmnid_item->cu_UP_PM_EPC->perSliceReportList));
    for (auto it = sm->slices.begin(); it != sm->slices.end(); ++it) {
      E2SM_KPM_PerSliceReportListItemFormat_t *slice_item = \
	(E2SM_KPM_PerSliceReportListItemFormat_t *)calloc(1,sizeof(*slice_item));
      slice_item->sliceName.buf = (uint8_t *)malloc(it->first.size());
      slice_item->sliceName.size = it->first.size();
      strncpy((char *)slice_item->sliceName.buf,it->first.c_str(),
	      slice_item->sliceName.size);
      asn_uint642INTEGER(&slice_item->bytesDL,it->second.dl_bytes);
      asn_uint642INTEGER(&slice_item->bytesUL,it->second.ul_bytes);
      ASN_SEQUENCE_ADD(&epc_cu_up_plmnid_item->cu_UP_PM_EPC->perSliceReportList->list,
		       slice_item);
    }
  }
#endif

  ASN_SEQUENCE_ADD(&epc_cu_up_item->o_CU_UP_PM_Container.plmnList.list,
		   epc_cu_up_plmnid_item);
  ASN_SEQUENCE_ADD(&pf_item->choice.oCU_UP.pf_ContainerList.list,
		   epc_cu_up_item);
  ASN_SEQUENCE_ADD(
    &im.indicationMessage.choice.indicationMessage_Format1.pm_Containers.list,
    pmc_item);

  E2SM_DEBUG(agent,"indication message:\n");
  E2SM_XER_PRINT(NULL,&asn_DEF_E2SM_KPM_E2SM_KPM_IndicationMessage,&im);

  msg_buf_len = ric::e2ap::encode(
    &asn_DEF_E2SM_KPM_E2SM_KPM_IndicationMessage,0,&im,&msg_buf);
  if (msg_buf_len < 0) {
    E2SM_ERROR(agent,"failed to encode indication msg; aborting send_indication\n");
    ASN_STRUCT_FREE_CONTENTS_ONLY(
      asn_DEF_E2SM_KPM_E2SM_KPM_IndicationMessage,&im);
    goto out;
  }

  /*
   * Finally, for each subscription and its actions, generate and send
   * an indication.  We could only do this more efficiently if we did
   * all the PDU generation here, because the subscription and action
   * IDs are built into the RICindication message, so we have to
   * generate a new one for each.  This means we are less efficient due
   * to all the memcpys to create the IEs; and the memcpys into temp
   * buffers for the SM-specific octet strings.  Ah well.
   */
  for (it = subscriptions.begin(); it != subscriptions.end(); ++it) {
    sub = *it;
    for (it2 = sub->actions.begin(); it2 != sub->actions.end(); ++it2) {
      action = *it2;

      if (ric::e2ap::generate_indication(
	    agent,sub->request_id,sub->instance_id,sub->function_id,
	    action->id,serial_number++,(int)E2AP_RICindicationType_report,
	    header_buf,header_buf_len,msg_buf,msg_buf_len,NULL,0,&buf,&buf_len)) {
	E2SM_ERROR(
	  agent,"kpm: failed to generate indication (reqid=%ld,instid=%ld,funcid=%ld,actid=%ld)\n",
	  sub->request_id,sub->instance_id,sub->function_id,action->id);
      }
      else {
	E2SM_DEBUG(
	  agent,"kpm: sending indication (reqid=%ld,instid=%ld,funcid=%ld,actid=%ld)\n",
	  sub->request_id,sub->instance_id,sub->function_id,action->id);
	agent->send_sctp_data(buf,buf_len);
      }
      free(buf);
      buf = NULL;
      buf_len = 0;
    }
  }

 out:
  pthread_mutex_unlock(&lock);
  return;
}

}
}
