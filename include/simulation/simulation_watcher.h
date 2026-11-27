#ifndef SIMULATION_SIMULATION_WATCHER_H
#define SIMULATION_SIMULATION_WATCHER_H

class c_simulation_watcher;
enum e_network_session_boot_reason;

extern "C" void simulation_watcher_handle_machine_boot(c_simulation_watcher *watcher, unsigned long mask, e_network_session_boot_reason reason);

#endif // SIMULATION_SIMULATION_WATCHER_H
