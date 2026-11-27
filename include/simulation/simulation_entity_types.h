#ifndef SIMULATION_SIMULATION_ENTITY_TYPES_H
#define SIMULATION_SIMULATION_ENTITY_TYPES_H

enum e_simulation_entity_type
{
    _simulation_entity_type_none = -1,
    _simulation_entity_type_slayer_engine_globals = 0,
    _simulation_entity_type_ctf_engine_globals = 1,
    _simulation_entity_type_assault_engine_globals = 2,
    _simulation_entity_type_oddball_engine_globals = 3,
    _simulation_entity_type_king_engine_globals = 4,
    _simulation_entity_type_territories_engine_globals = 5,
    _simulation_entity_type_juggernaut_engine_globals = 6,
    _simulation_entity_type_sandbox_engine_globals = 7,
    _simulation_entity_type_infection_engine_globals = 8,
    _simulation_entity_type_vip_engine_globals = 9,
    _simulation_entity_type_game_engine_player = 10,
    _simulation_entity_type_game_engine_statborg = 11,
    _simulation_entity_type_breakable_surface_group = 12,
    _simulation_entity_type_map_variant = 13,
    _simulation_entity_type_unit = 14,
    _simulation_entity_type_item = 15,
    _simulation_entity_type_generic = 16,
    _simulation_entity_type_generic_garbage = 17,
    _simulation_entity_type_vehicle = 18,
    _simulation_entity_type_projectile = 19,
    _simulation_entity_type_weapon = 20,
    _simulation_entity_type_device = 21,
    k_simulation_entity_type_count = 22
};

#endif // SIMULATION_SIMULATION_ENTITY_TYPES_H
