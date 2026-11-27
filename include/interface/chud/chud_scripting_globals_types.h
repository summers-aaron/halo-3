#ifndef INTERFACE_CHUD_CHUD_SCRIPTING_GLOBALS_TYPES_H
#define INTERFACE_CHUD_CHUD_SCRIPTING_GLOBALS_TYPES_H

#include <stddef.h>
#include "items/equipment.h"

// Storage view shared with the owner in interfacelibxbox/chud.cpp.
struct chud_script_flags
{
    union
    {
        bool enabled;
        bool show;
    };
    // Script element indices 1..8 share these named boolean slots.
    union
    {
        bool items[8];
        struct
        {
            bool show_weapon_stats;
            bool show_crosshair;
            bool show_shield;
            bool show_grenades;
            bool show_messages;
            bool show_motion_sensor;
            bool show_spike_grenades;
            bool show_fire_grenades;
        };
    };
};

struct chud_scripting_globals_tls
{
    s_equipment_slot equipment_slots[8];
    long user_index;
    chud_script_flags scripting;
    bool show_compass;
    bool show_stamina;
    bool show_energy_meters;
    bool show_consumables;
};

typedef char chud_script_flags_size[(sizeof(chud_script_flags) == 9) ? 1 : -1];

typedef char chud_scripting_enabled_offset[(offsetof(chud_scripting_globals_tls, scripting) == 0x144) ? 1 : -1];

typedef char chud_scripting_weapon_stats_offset[(offsetof(chud_scripting_globals_tls, scripting)
    + offsetof(chud_script_flags, show_weapon_stats) == 0x145) ? 1 : -1];
typedef char chud_scripting_fire_grenades_offset[(offsetof(chud_scripting_globals_tls, scripting)
    + offsetof(chud_script_flags, show_fire_grenades) == 0x14c) ? 1 : -1];
typedef char chud_scripting_compass_offset[(offsetof(chud_scripting_globals_tls, show_compass) == 0x14d) ? 1 : -1];

typedef char chud_scripting_consumables_offset[(offsetof(chud_scripting_globals_tls, show_consumables) == 0x150) ? 1 : -1];

#endif // INTERFACE_CHUD_CHUD_SCRIPTING_GLOBALS_TYPES_H
