#ifndef INTERFACE_INTERFACE_GLOBALS_H
#define INTERFACE_INTERFACE_GLOBALS_H

#include "interface/view_config_types.h"

struct fpw_first_person;

extern "C" {
extern __declspec(thread) fpw_first_person *g_first_person_weapon_data;
extern __declspec(thread) void *g_first_person_weapon_orientation_data;
}

#endif
