// networkinglibxbox/network_bandwidth.obj: from the records 0x903a8 0x3dcb18 0x3dcba8 0x3dcc78 0x3dcdc0 0x3dcfd0
// @flags /TP /O2 /Oc
#include "cseries/cseries.h"
// @address 0x903a8 ?network_bandwidth_update@@YAXXZ
// @address 0x3dcb18 ?network_bandwidth_get_estimated_maximum_machine_capacity_of_bandwidth@@YAJJ@Z
// @address 0x3dcc78 ?network_bandwidth_compute_average@@YAJJPBJ@Z
// @address 0x3dcfd0 ?network_bandwidth_update_estimate@@YAXXZ
#include <string.h>
#include "memory/tls_globals.h"
#include "cseries/cseries_types.h"
#include "game/game_globals_storage.h"

#include "game/game.h"
#include "game/game_simulation_predicates.h"
#include "game/game_state_accessors.h"
#include "cseries/crt_xdk.h"
#include "simulation/simulation.h"
#include "networking/logic/network_bandwidth.h"
#include "networking/network_configuration.h"
#include "networking/network_configuration_types.h"
#include "networking/transport/transport_qos_types.h"
#include "networking/network_time_types.h"
// reset_bandwidth is c_network_observer's member (0x3d8f98): the observer is its this.
#include "networking/session/network_session_parameter_observer.h"
#include "networking/logic/network_bandwidth_globals.h"
static s_bandwidth_globals network_bandwidth_globals;

void network_bandwidth_dispose()
{
  network_bandwidth_globals.initialized = false;
}

// 0x903a8 helpers and callees
// @address 0x8c447c g_network_time_globals

void network_bandwidth_update(void)
{
  bool tracking = false;
  bool restricted = false;
  if (network_bandwidth_globals.online_network_environment && simulation_starting_up() &&
      game_simulation::game_is_distributed() && game_simulation::game_is_server() && !game_is_cooperative())
    tracking = true;
  if (!game_in_progress()) {
    restricted = network_bandwidth_globals.restricted;
  } else if (tls_game_globals()->options.game_mode != 3) {
    restricted = true;
  }
  c_network_observer *observer = network_bandwidth_globals.observer;
  if (restricted && !network_bandwidth_globals.restricted) {
    network_bandwidth_globals.restricted = true;
    observer->m_stream_total_bandwidth_average = -1;
  } else if (!restricted && network_bandwidth_globals.restricted) {
    network_bandwidth_globals.restricted = false;
    observer->m_stream_total_bandwidth_average = -1;
  }
  if (tracking && !network_bandwidth_globals.tracking) {
    if (network_bandwidth_globals.initialized) {
      observer->reset_bandwidth();
      network_bandwidth_globals.tracking = true;
      long now;
      if (g_network_time_globals.enabled)
        now = g_network_time_globals.time;
      else
        now = GetTickCount();
      network_bandwidth_globals.last_time = now;
    }
  } else if (!tracking && network_bandwidth_globals.tracking) {
    network_bandwidth_tracking_end();
  }
}

long network_bandwidth_get_estimated_maximum_machine_capacity_of_bandwidth(long bandwidth)
{
  long level;
  for (level = 16; level > 0; level--) {
    if (bandwidth >= g_network_configuration.machine_capacity_bandwidth_thresholds[level])
      break;
  }
  return MIN(MAX(level, 1), 16);
}

// 0x3dcba8
void network_bandwidth_notify_live_service_qos_measurement(const s_transport_qos_result *transport_qos_result)
{
  s_bandwidth_history *history = network_bandwidth_globals.history;
  if (history) {
    if (history->qos_count >= 8) {
      history->qos_count = 7;
      memmove(history->qos, history->qos + 1, sizeof(history->qos) - sizeof(history->qos[0]));
      history = network_bandwidth_globals.history;
    }
    history->qos[history->qos_count] = transport_qos_result->bandwidth_upstream_bps;
    history->qos_count++;
    ::network_bandwidth_update_estimate();
    if (network_bandwidth_globals.estimated &&
        transport_qos_result->bandwidth_upstream_bps >
            network_bandwidth_globals.bandwidth + network_bandwidth_globals.settings->dispute_margin)
    {
      network_bandwidth_globals.history->disputes++;
      ::network_bandwidth_update_estimate();
    }
  }
}

long network_bandwidth_compute_average(long sample_count, const long *samples)
{
    long sorted[128];
    long sorted_count;
    long trim_count = (long)((float)sample_count * network_bandwidth_globals.settings->trim_fraction);
    if (trim_count > 0) {
        sorted_count = 0;
        for (long sample_index = 0; sample_index < sample_count; sample_index++) {
            for (long insertion_index = sorted_count; insertion_index >= 0; insertion_index--) {
                if (insertion_index > 0 && sorted[insertion_index - 1] > samples[sample_index]) {
                    sorted[insertion_index] = sorted[insertion_index - 1];
                } else {
                    sorted[insertion_index] = samples[sample_index];
                    break;
                }
            }
            sorted_count++;
        }
        sorted_count = sorted_count - trim_count * 2;
        memmove(sorted, sorted + trim_count, sorted_count * 4);
    } else {
        memcpy(sorted, samples, sample_count * 4);
        sorted_count = sample_count;
    }
    long sum = 0;
    for (long sorted_index = 0; sorted_index < sorted_count; sorted_index++)
        sum += sorted[sorted_index];
    return sum / sorted_count;
}

static __forceinline long network_bandwidth_system_time() {
  if (g_network_time_globals.enabled)
    return g_network_time_globals.time;
  return GetTickCount();
}

static __forceinline bool network_bandwidth_sample_get(c_network_observer *observer, float *ratio, long *first, long *second) {
  bool result = false;
  if (observer->m_stream_peak_bandwidth_bps != -1 && observer->m_stream_total_bandwidth_average != -1 &&
      observer->m_bandwidth_probe_counts[1] + observer->m_bandwidth_probe_counts[0] > 0) {
    *first = observer->m_stream_peak_bandwidth_bps;
    *second = observer->m_stream_total_bandwidth_average;
    result = true;
    *ratio = (float)observer->m_bandwidth_probe_counts[1] / (float)(observer->m_bandwidth_probe_counts[1] + observer->m_bandwidth_probe_counts[0]);
  }
  return result;
}

// @address 0x3dcdc0 ?network_bandwidth_tracking_end@@YAXXZ
void network_bandwidth_tracking_end(void)
{
  long first;
  long second;
  float ratio;
  if (network_bandwidth_globals.tracking) {
    if (network_bandwidth_sample_get(network_bandwidth_globals.observer, &ratio, &first, &second) && network_bandwidth_globals.history) {
      long start = network_bandwidth_globals.last_time;
      bool due = network_bandwidth_system_time() - start >= network_bandwidth_globals.settings->interval;
      bool low = ratio < network_bandwidth_globals.settings->ratio;
      if (due && !low && (first < network_bandwidth_globals.bandwidth || second < network_bandwidth_globals.bandwidth))
        due = false;
      if (due) {
        s_bandwidth_history *history = network_bandwidth_globals.history;
        if (history->measurement_count >= 8) {
          history->measurement_count = 7;
          memmove(history->good, history->good + 1, sizeof(history->good) - sizeof(history->good[0]));
          memmove(network_bandwidth_globals.history->unsafe, network_bandwidth_globals.history->unsafe + 1,
              network_bandwidth_globals.history->measurement_count * 4);
          history = network_bandwidth_globals.history;
        }
        history->good[history->measurement_count] = first;
        history->unsafe[history->measurement_count] = second;
        history->measurement_count++;
        memset(&history->disputes, 0, sizeof(history->disputes));
        ::network_bandwidth_update_estimate();
      }
    }
    ((c_network_observer *)network_bandwidth_globals.observer)->reset_bandwidth();
    network_bandwidth_globals.tracking = false;
  }
}

static __forceinline void network_bandwidth_set_estimated_bandwidth(c_network_observer *observer, long bandwidth, bool estimated)
{
  if (!observer->m_bandwidth_locked) {
    if (bandwidth == -1) {
      observer->m_bandwidth_bps = 0x100000;
      observer->m_stream_low_bandwidth = true;
    } else {
      observer->m_bandwidth_bps = MIN(MAX(bandwidth, observer->m_configuration->stream_minimum_estimated_bandwidth_bps), 0x100000);
      observer->m_stream_low_bandwidth = estimated;
    }
    observer->reset_bandwidth();
    observer->m_bandwidth_locked = false;
  }
}

void network_bandwidth_update_estimate(void)
{
  long capacity = 16;
  long bandwidth = -1;
  bool estimated = false;
  if (network_bandwidth_globals.history && network_bandwidth_globals.online_network_environment) {
    long good = 0, unsafe = 0, qos = 0;
    if (network_bandwidth_globals.history->measurement_count >= network_bandwidth_globals.settings->minimum_measurements) {
      good = network_bandwidth_compute_average(network_bandwidth_globals.history->measurement_count, network_bandwidth_globals.history->good);
      unsafe = network_bandwidth_compute_average(network_bandwidth_globals.history->measurement_count, network_bandwidth_globals.history->unsafe);
    }
    if (network_bandwidth_globals.history->qos_count > 0)
      qos = network_bandwidth_compute_average(network_bandwidth_globals.history->qos_count, network_bandwidth_globals.history->qos);
    if (good > 0) {
      bandwidth = (good <= unsafe - network_bandwidth_globals.settings->safety_margin ? good : unsafe - network_bandwidth_globals.settings->safety_margin);
      bandwidth = MAX(bandwidth, network_bandwidth_globals.settings->minimum_bandwidth);
      estimated = true;
      if (qos > bandwidth && network_bandwidth_globals.history->disputes > network_bandwidth_globals.settings->dispute_threshold) {
        long excess = network_bandwidth_globals.history->disputes - network_bandwidth_globals.settings->dispute_threshold;
        long disputed = bandwidth + excess * network_bandwidth_globals.settings->dispute_increment;
        disputed = (disputed <= qos ? disputed : qos);
        bandwidth = disputed;
      }
    } else if (qos > 0) {
      bandwidth = MAX(qos, network_bandwidth_globals.settings->minimum_bandwidth);
    }
  }
  if (bandwidth != -1)
    capacity = network_bandwidth_get_estimated_maximum_machine_capacity_of_bandwidth(bandwidth);
  network_bandwidth_globals.bandwidth = bandwidth;
  network_bandwidth_globals.estimated = estimated;
  network_bandwidth_globals.capacity = capacity;
  network_bandwidth_set_estimated_bandwidth(network_bandwidth_globals.observer, network_bandwidth_globals.bandwidth, network_bandwidth_globals.estimated);
}
