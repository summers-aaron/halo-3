#ifndef INTERFACE_EVENT_MANAGER_GLOBALS_H
#define INTERFACE_EVENT_MANAGER_GLOBALS_H

union point2d;
enum e_gamepad_stick;
enum e_controller_index;
enum e_controller_component;

void event_manager_tab(e_gamepad_stick gamepad_stick, e_controller_index controller_index, const point2d *point,
    unsigned long timestamp, e_controller_component controller_component);

extern "C" {
extern unsigned long g_event_manager_last_tab_times[4];
extern unsigned long g_event_manager_repeat_start_times[4];
extern point2d g_event_manager_previous_stick[4];
extern bool g_event_manager_tab_active[4];
extern bool g_event_manager_tab_enabled;
}

#endif // INTERFACE_EVENT_MANAGER_GLOBALS_H
