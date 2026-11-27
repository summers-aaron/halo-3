#ifndef NETWORKING_LOGIC_NETWORK_BANDWIDTH_H
#define NETWORKING_LOGIC_NETWORK_BANDWIDTH_H

// Functions defined in tu/networkinglibxbox/network_bandwidth.cpp.

struct s_transport_qos_result;

void network_bandwidth_dispose();
void network_bandwidth_update(void);
long network_bandwidth_get_estimated_maximum_machine_capacity_of_bandwidth(long bandwidth);
void network_bandwidth_notify_live_service_qos_measurement(const s_transport_qos_result *transport_qos_result);
long network_bandwidth_compute_average(long sample_count, const long *samples);
void network_bandwidth_update_estimate(void);
void network_bandwidth_tracking_end(void);

#endif // NETWORKING_LOGIC_NETWORK_BANDWIDTH_H
