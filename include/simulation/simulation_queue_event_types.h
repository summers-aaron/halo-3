#ifndef SIMULATION_SIMULATION_QUEUE_EVENT_TYPES_H
#define SIMULATION_SIMULATION_QUEUE_EVENT_TYPES_H

#include <stddef.h>

enum e_simulation_event_type
{
    _simulation_event_type_none = -1,
    _simulation_event_type_damage_aftermath = 0,
    _simulation_event_type_damage_section_response = 1,
    _simulation_event_type_breakable_surface_damage = 2,
    _simulation_event_type_projectile_attached = 3,
    _simulation_event_type_projectile_effect = 4,
    _simulation_event_type_projectile_impact_effect = 5,
    _simulation_event_type_projectile_object_impact_effect = 6,
    _simulation_event_type_game_engine = 7,
    _simulation_event_type_unit_board_vehicle = 8,
    _simulation_event_type_unit_pickup = 9,
    _simulation_event_type_weapon_effect = 10,
    _simulation_event_type_weapon_empty_click = 11,
    _simulation_event_type_unit_exit_vehicle = 12,
    _simulation_event_type_weapon_fire = 13,
    _simulation_event_type_weapon_reload = 14,
    _simulation_event_type_unit_grenade_initiate = 15,
    _simulation_event_type_unit_melee_initiate = 16,
    _simulation_event_type_weapon_pickup = 17,
    _simulation_event_type_weapon_put_away = 18,
    _simulation_event_type_weapon_drop = 19,
    _simulation_event_type_vehicle_flip = 20,
    _simulation_event_type_vehicle_trick = 21,
    _simulation_event_type_device_touch = 22,
    _simulation_event_type_unit_grenade_release = 23,
    _simulation_event_type_unit_melee_damage = 24,
    _simulation_event_type_unit_melee_clang = 25,
    _simulation_event_type_unit_enter_vehicle = 26,
    _simulation_event_type_game_engine_request_boot_player = 27,
    _simulation_event_type_player_respawn_request = 28,
    _simulation_event_type_unit_equipment_use = 29,
    _simulation_event_type_unit_equipment_pickup = 30,
    _simulation_event_type_projectile_supercombine_request = 31,
    _simulation_event_type_object_refresh = 32,
    _simulation_event_type_player_editor_request = 33,
    k_simulation_event_type_count = 34
};

struct s_simulation_queue_decoded_event_data
{
    e_simulation_event_type event_type;
    long reference_count;
    long references[2];
    unsigned char payload[1024];
    long payload_size;
};

typedef char simulation_queue_decoded_event_data_size[(sizeof(s_simulation_queue_decoded_event_data) == 0x414) ? 1 : -1];
typedef char simulation_queue_decoded_event_payload_offset[(offsetof(s_simulation_queue_decoded_event_data, payload) == 0x10) ? 1 : -1];
typedef char simulation_queue_decoded_event_payload_size_offset[(offsetof(s_simulation_queue_decoded_event_data, payload_size) == 0x410) ? 1 : -1];

#endif // SIMULATION_SIMULATION_QUEUE_EVENT_TYPES_H
