#ifndef EFFECTS_EFFECT_EVENT_DATA_H
#define EFFECTS_EFFECT_EVENT_DATA_H

struct s_data_array;
extern "C" {
extern __declspec(thread) s_data_array *g_event_data;
}

#endif
