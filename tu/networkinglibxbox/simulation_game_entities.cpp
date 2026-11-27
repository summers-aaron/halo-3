// networkinglibxbox/simulation_game_entities.obj: from the records 0x39f260 0x3d1ec8 0x3d1fb0 0x3d2050 0x3d20f0 0x3d2188 0x3d2398 0x3d2418 0x42d5a8
// @flags /TP /O2 /Oc
#include "simulation/simulation_distributed_world_types.h"
#include "memory/data_types.h"
#include "simulation/simulation_entity_types.h"
#include "simulation/simulation_globals.h"
#include "memory/tls_globals.h"
// @address 0x39f260 ?is_authority@c_simulation_world@@QBA_NXZ
// @address 0x3d1ec8 ?simulation_entity_create@@YAJW4e_simulation_entity_type@@JJ@Z
// @address 0x3d1fb0 ?simulation_entity_update@@YAXJJK@Z
// @address 0x3d2050 ?simulation_entity_latch_update@@YAXJJK@Z
// @address 0x3d20f0 simulation_entity_delete
// @address 0x3d2398 ?simulation_entity_get_gamestate_index@@YAJJ@Z
// @address 0x3d2418 simulation_entity_is_object_entity
// @address 0x42d5a8 ?prepare_state_data_for_gameworld@c_simulation_map_variant_group_entity_definition@@UAAXKJPAX@Z
#include <ppcintrinsics.h>
#include "tag_files/tag_types.h"
#include "tag_files/tag_groups.h"
#include "simulation/game_interface/simulation_game_entities.h"
#include "simulation/game_interface/simulation_game_map_variant.h"
#include "simulation/game_interface/simulation_game_entity_definitions.h"
#include "game/game_engine_sandbox.h"
#include "simulation/game_interface/entity_local_flags_types.h"
#include "cache/cache_files.h"
#include "networking/simulation/simulation_accessors.h"
#include "objects/object_definitions.h"
#include "models/model_types.h"
#include "physics/physics_model_types.h"
#include "networking/simulation/simulation_entity_database.h"
#include "networking/replication/replication_entity_manager.h"
#include "simulation/simulation_world_types.h"
#include "simulation/simulation_gamestate_entities.h"

inline c_simulation_entity_database *c_simulation_world::get_entity_database() { return &m_distributed_world->m_entity_database; }


static __forceinline bool simulation_game_entities_entity_is_local(c_simulation_entity_database *database, long entity_index)
{
    return (((c_replication_entity_manager *)database->entities)->m_entity_data[entity_index & 0x3ff].flags & 4) != 0;
}

static __forceinline void simulation_game_entities_set_simulation_entity_index(long gamestate_index, long simulation_entity_index)
{
    long absolute_index = gamestate_index & 0xffff;
    ((s_simulation_gamestate_entity_datum *)tls_simulation_gamestate_entity_data()->data)[absolute_index].entity_index = simulation_entity_index;
}

long simulation_entity_create(e_simulation_entity_type entity_type, long object_index, long gamestate_index)
{
    long entity_index = -1;
    c_simulation_world *world = simulation_get_world();
    if (world->is_distributed() && world->is_authority())
    {
        c_simulation_entity_database *database = world->get_entity_database();
        entity_index = database->entity_create(entity_type);
        if (entity_index != -1)
        {
            s_simulation_entity *entity = database->entity_get(entity_index);
            entity->gamestate_index = gamestate_index;
            simulation_game_entities_set_simulation_entity_index(entity->gamestate_index, entity->entity_index);
        }
    }
    return entity_index;
}

void simulation_entity_update(long entity_index, long unused, unsigned long update_mask)
{
    c_simulation_world *world = simulation_get_world();
    if (world->is_distributed() && world->is_authority())
    {
        c_simulation_entity_database *database = world->get_entity_database();
        if (simulation_game_entities_entity_is_local(database, entity_index))
            database->entity_update(entity_index, update_mask, false);
    }
}

void simulation_entity_latch_update(long entity_index, long unused, unsigned long update_mask)
{
    c_simulation_world *world = simulation_get_world();
    if (world->is_distributed() && world->is_authority())
    {
        c_simulation_entity_database *database = world->get_entity_database();
        if (simulation_game_entities_entity_is_local(database, entity_index))
            database->entity_update(entity_index, update_mask, true);
    }
}

extern "C" void simulation_entity_delete(long entity_index, long object_index, long gamestate_index)
{
    c_simulation_world *world = simulation_get_world();
    if (world->is_distributed())
    {
        c_simulation_entity_database *database = world->get_entity_database();
        s_simulation_entity *entity = database->entity_get(entity_index);
        if (simulation_game_entities_entity_is_local(database, entity_index))
        {
            entity->gamestate_index = -1;
            entity->exists_in_gameworld = false;
            ((c_replication_entity_manager *)database->entities)->delete_local_entity(entity_index);
        }
        else
        {
            entity->gamestate_index = -1;
            entity->exists_in_gameworld = false;
        }
    }
}

static __forceinline bool simulation_game_entities_has_multiplayer_properties(s_object_definition const *definition)
{
    return definition->multiplayer_object.count() > 0;
}

// @address 0x3d2188 ?simulation_entity_type_from_object_creation@@YA?AW4e_simulation_entity_type@@JJ_N@Z
enum e_simulation_entity_type simulation_entity_type_from_object_creation(long definition_index, long object_index, bool dynamic)
{
    s_object_definition *definition = TAG_GET('obje', s_object_definition, definition_index);
    long result = -1;
    switch (definition->object_type)
    {
    case _object_type_biped: result = _simulation_entity_type_unit; break;
    case _object_type_weapon: if (object_index == -1) result = _simulation_entity_type_weapon; break;
    case _object_type_vehicle: result = _simulation_entity_type_vehicle; break;
    case _object_type_equipment: result = _simulation_entity_type_item; break;
    case _object_type_crate:
        if (definition->model.index != -1)
        {
            bool physics_only = false;
            s_model_definition *model = TAG_GET('hlmt', s_model_definition, definition->model.index);
            if (model->physics_model_index != -1)
            {
                s_physics_model_definition *physics_model = TAG_GET('phmo', s_physics_model_definition, model->physics_model_index);
                physics_only = (physics_model->flags & 2) != 0;
            }
            if (!physics_only)
            {
                if (dynamic) result = _simulation_entity_type_generic_garbage;
                else result = _simulation_entity_type_generic;
            }
        }
        break;
    case _object_type_projectile: result = _simulation_entity_type_projectile; break;
    case _object_type_terminal: case _object_type_machine: case _object_type_control: result = _simulation_entity_type_device; break;
    case _object_type_scenery:
        if (game_engine_sandbox_is_active())
        {
            if (simulation_game_entities_has_multiplayer_properties(definition)) result = _simulation_entity_type_generic;
        }
        else if (definition->model.index != -1)
        {
            s_model_definition *model = TAG_GET('hlmt', s_model_definition, definition->model.index);
            if (model->damage_info.count > 0)
            {
                s_scenery_model_damage_info_definition *damage_info = (s_scenery_model_damage_info_definition *)model->damage_info.address;
                if (damage_info->damage_sections.count > 0 || damage_info->damage_constraints.count > 0) result = _simulation_entity_type_generic;
            }
        }
        break;
    }
    return (e_simulation_entity_type)result;
}

long simulation_entity_get_gamestate_index(long entity_index)
{
    long result = -1;
    if (entity_index != -1)
    {
        c_simulation_world *world = simulation_get_world();
        if (world->is_distributed())
        {
            c_simulation_entity_database *database = world->get_entity_database();
            s_simulation_entity *entity = database->entity_try_and_get(entity_index);
            if (entity)
                result = entity->gamestate_index;
        }
    }
    return result;
}

static __forceinline bool simulation_game_entities_entity_type_in_range(long entity_type)
{
    bool result = false;
    if ((unsigned long)(entity_type - 14) <= 7)
        result = true;
    return result;
}

extern "C" bool simulation_entity_is_object_entity(long entity_index)
{
    bool result = false;
    if (entity_index != -1)
    {
        c_simulation_world *world = simulation_get_world();
        if (world->is_distributed())
        {
            c_simulation_entity_database *database = world->get_entity_database();
            s_simulation_entity *entity = database->entity_try_and_get(entity_index);
            if (entity)
                result = simulation_game_entities_entity_type_in_range(entity->entity_type);
        }
    }
    return result;
}

void c_simulation_map_variant_group_entity_definition::prepare_state_data_for_gameworld(unsigned long update_mask, long size, void *state)
{
    s_simulation_game_entity_entry *entries = static_cast<s_simulation_game_entity_entry *>(state);
    for (long i = 0; i < 8; ++i)
    {
        if ((update_mask & (1 << (1 + i * 4))) != 0)
        {
            entries[i].gamestate_index2 = simulation_entity_get_gamestate_index(entries[i].entity_index2);
            entries[i].gamestate_index1 = simulation_entity_get_gamestate_index(entries[i].entity_index1);
        }
    }
}
