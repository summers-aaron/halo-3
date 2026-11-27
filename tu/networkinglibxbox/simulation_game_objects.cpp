// networkinglibxbox/simulation_game_objects.obj: from the records 0x40a9e0 0x40a9f0 0x40aac0 0x40abb8 0x40acb0 0x40ad10 0x40ad60 0x40adf8 0x40af28 0x40afb8 0x40b058 0x40b0d0 0x40b128 0x40b3a8 0x40b530 0x40b6c0 0x40bff0 0x40c818 0x40c8e0 0x40c9d0 0x40cb38 0x40d178 0x40d840 0x40dc30 0x40dd60 0x40dd70
// @compiler xdk6130
// @flags /TP /O2
#include "simulation/simulation_distributed_world_types.h"
#include "networking/simulation/simulation_entity_definition.h"
#include "simulation/simulation_game_position_update_types.h"
#include "simulation/simulation_node_space_encoding_types.h"
#include "simulation/simulation_globals.h"
#include "memory/tls_globals.h"
#include "objects/object_header.h"
#include "tag_files/tag_groups.h"
#include "networking/simulation/simulation_entity_database.h"
#include "networking/simulation/simulation_view_telemetry.h"
#include "memory/data.h"
#include "math/matrix_math_types.h"
#include "tag_files/tag_types.h"
#include "objects/object_types.h"
#include "objects/object_placement.h"
#include "cseries/cseries.h"
#include "game/map_variant.h"
#include "game/game_globals_storage.h"
#include "objects/object_datum.h"
#include "game/game.h"
#include "objects/objects.h"
#include "objects/object_hierarchy.h"
#include "simulation/game_interface/simulation_game_damage_operations.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "units/units.h"
#include "cache/cache_files.h"
#include "objects/damage.h"
#include "simulation/game_interface/simulation_game_internal.h"
#include "simulation/game_interface/simulation_game_objects.h"
#include "simulation/simulation_encoding.h"
#include "simulation/simulation_gamestate_entities.h"
#include "render/render_lens_flares.h"
#include "memory/bitstream.h"
#include "game/game_engine_sandbox.h"
#include "objects/multiplayer_object_types.h"
#include "simulation/simulation_world_types.h"
#include "simulation/game_interface/entity_local_flags_types.h"
#include "objects/object_type_definition.h"
#include "math/real_math_types.h"
#include "math/real_math_limit.h"
#include "math/real_math.h"
#include <math.h>
#include "simulation/game_interface/simulation_game_object_data_types.h"
#include "scenario/scenario.h"
#include "networking/simulation/simulation_accessors.h"
#include "networking/simulation/simulation_gamestate_encoding.h"
#include "networking/replication/replication_entity.h"
// @address 0x40a9e0 ?event_type_name@c_simulation_object_refresh_event_definition@@UAAPBDXZ
// @address 0x220d10 ?compare_quantized_reals@c_bitstream@@SA_NMMMMJ_N0@Z
// @address 0x220e68 ?compare_vectors@c_bitstream@@SA_NPBTreal_vector3d@@0MMJ@Z
// @address 0x221030 ?compare_axes@c_bitstream@@SA_NPBTreal_vector3d@@000@Z
// @address 0x3ade48 ?simulation_compare_quantized_positions@@YA_NPBTreal_point3d@@0JPBTreal_rectangle3d@@@Z
// @address 0x40d840 ?object_state_compare@c_simulation_object_entity_definition@@KA_NPAUs_simulation_object_state_data@@0@Z
// @address 0x327bb0 ?unit_update_driver_and_gunner@@YAXJ@Z
// @address 0x221398 ?write_bit_internal@c_bitstream@@AAAX_N@Z
// @address 0x221480 ?write_value_internal@c_bitstream@@QAAXKJ@Z
// @address 0x221648 ?read_bit_internal@c_bitstream@@AAA_NXZ
// @address 0x221768 ?read_value_internal@c_bitstream@@AAAKJ@Z
// @address 0x807a0 ?game_is_cooperative@@YA_NXZ
// @address 0x74b700 ?global_origin3d@@3PBTreal_point3d@@B
// @address 0x74b70c ?global_forward3d@@3PBTreal_vector3d@@B
// @address 0x74b714 ?global_up3d@@3PBTreal_vector3d@@B
// @address 0x7cff08 ?global_scenario@@3PAUscenario@@A
// @address 0x7cff0c ?global_tag_instances@@3PBUcache_file_tag_instance@@B
// @address 0x73e2dc ?object_type_definitions@@3PAPAUobject_type_definition@@A
// @address 0xac748 ?object_try_and_get_and_verify_type@@YAPAXJK@Z
// @address 0x3af138 ?simulation_gamestate_entity_get_object_index@@YAJJ@Z
// @address 0x40eb58 ?simulation_write_definition_index@@YAXPBDPAVc_bitstream@@J@Z
// @address 0x40ebd8 ?simulation_read_definition_index@@YAJPBDPAVc_bitstream@@@Z
// @address 0x87af0 ?tag_try_and_get_unsafe@@YAPAXKJ@Z
// @address 0x19d2e8 ?vectors3d_from_euler_angles3d@@YAXPATreal_vector3d@@0PBTreal_euler_angles3d@@@Z
// @address 0x40a9f0 ?simulation_object_get_replicated_object_from_entity@@YAJJ@Z
// @address 0x40acb0 ?entity_replication_required_for_view_activation@c_simulation_object_entity_definition@@UAA_NPBUs_simulation_entity@@@Z
// @address 0x40ad10 ?build_creation_data@c_simulation_object_entity_definition@@UAAXJJPAX@Z
// @address 0x40ad60 ?build_updated_state_data@c_simulation_object_entity_definition@@UAA_NPBUs_simulation_entity@@PAKJPAX@Z
// @address 0x40adf8 ?create_game_entity@c_simulation_object_entity_definition@@UAA_NJJPBXKJ0@Z
// @address 0x9a080 ?object_get_origin@@YAPATreal_point3d@@JPAT1@@Z
// @address 0x40dd70 ?write_description_to_string@c_simulation_object_refresh_event_definition@@UAAXPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@MJPAD@Z
// @address 0x1858b8 ?csnzprintf@@YAPADPADIPBDZZ
// @address 0x40af28 ?update_game_entity@c_simulation_object_entity_definition@@UAA_NJKJPBX@Z
// @address 0x40afb8 ?delete_game_entity@c_simulation_object_entity_definition@@UAA_NJ@Z
// @address 0x40b058 ?promote_game_entity_to_authority@c_simulation_object_entity_definition@@UAA_NJ@Z
// @address 0x40b0d0 ?gameworld_attachment_valid@c_simulation_object_entity_definition@@UAA_NJ@Z
// @address 0x40c9d0 ?object_calculate_update_requirements@c_simulation_object_entity_definition@@KAXPBUs_simulation_entity@@PBUs_entity_update_data@@PAJ@Z
// @address 0x40d178 ?object_update_decode@c_simulation_object_entity_definition@@KA_N_NPAKPAUs_simulation_object_state_data@@PAVc_bitstream@@0@Z
// @address 0xa31c8 ?dequantize_real@@YAMJMMJ_N@Z
// @address 0x12b7d0 ?valid_real_vector3d_axes2@@YA_NPBTreal_vector3d@@0@Z
// @address 0x205af8 ?valid_real_point3d@@YA_NPBTreal_point3d@@@Z
// @address 0x205af8 ?valid_real_vector3d@@YA_NPBTreal_vector3d@@@Z
// @address 0x220b48 ?read_vector@c_bitstream@@QAAXPBDPATreal_vector3d@@MMJ@Z
// @address 0x220c70 ?read_axes@c_bitstream@@QAAXPBDPATreal_vector3d@@1@Z
// @address 0xa2b80 ?quantize_real@@YAJMMMJ_N0@Z
// @address 0x220790 ?write_vector@c_bitstream@@QAAXPBDPBTreal_vector3d@@MMJ@Z
// @address 0x2208b0 ?write_axes@c_bitstream@@QAAXPBDPBTreal_vector3d@@1@Z
// @address 0x3adbd8 ?simulation_write_quantized_position@@YAXPAVc_bitstream@@PBTreal_point3d@@J_NPBTreal_rectangle3d@@@Z
// @address 0x3adf70 ?simulation_write_quantized_node_space_position@@YAXPAVc_bitstream@@PBTreal_point3d@@W4e_simulation_node_space_encoding_type@@J@Z
// @address 0x446e00 ?entity_is_active@c_simulation_view_telemetry_provider@@QBA_NJ@Z
// @address 0x446e88 ?make_room_for_update@c_entity_update_encode_helper@@QAA_NPAVc_bitstream@@JJJK@Z
// @address 0x446ef8 ?write_component_header@c_entity_update_encode_helper@@QAA_NJPBD@Z
// @address 0x446f80 ?finish_component@c_entity_update_encode_helper@@QAAXXZ
// @address 0x40cb38 ?object_update_encode@c_simulation_object_entity_definition@@KA_N_NKPAKPBUs_simulation_view_telemetry_data@@PBUs_simulation_object_state_data@@PAVc_bitstream@@J00@Z
// @address 0x3adff0 simulation_read_quantized_node_space_position

struct s_game_state;

static const float k_simulation_object_minimum_velocity = 0.03f;
static const float k_simulation_object_maximum_scale = 10.0f;
static const float k_simulation_object_maximum_translational_velocity = 350.0f;
static const float k_simulation_object_maximum_angular_velocity = 30.0f;
static const float k_simulation_object_minimum_body_vitality = -1.0f;
static const float k_simulation_object_maximum_body_vitality = 1.0f;
static const float k_simulation_object_maximum_shield_vitality = 4.0f;
static const float k_simulation_object_minimum_shield_vitality = 0.0f;
#include "math/real_math_constants.h"
#include "objects/object_node_matrices_inline.h"
#include "math/matrix_math_functions.h"

#include "objects/object_definitions.h"
#include "scenario/scenario_objects.h"
#include "models/model_damage_info.h"
#include "models/model_types.h"
#include "models/model_variant_types.h"

#include "simulation/game_interface/simulation_game_object_entity_definition.h"
#include "game/game_engine_scoring_globals.h"

namespace
{
    static __forceinline void simulation_gamestate_index_encode(c_bitstream *packet, long gamestate_index)
    {
        packet->write_integer("gamestgate-index-id", (unsigned short)(gamestate_index >> 16), 16);
        packet->write_integer("gamestate-index-absolute", (unsigned short)gamestate_index, 11);
    }
}

namespace
{

    static __forceinline c_simulation_entity_definition *simulation_get_entity_definition(long entity_type)
    {
        return ((c_simulation_entity_definition **)simulation_get_type_collection())[entity_type + 1];
    }

    static __forceinline s_simulation_entity *entity_database_try_and_get(c_simulation_entity_database *database, long entity_index)
    {
        long absolute_index = entity_index & 0x3ff;
        s_simulation_entity *entity = 0;
        if (entity_index != -1 && database->channels[absolute_index].entity_index == entity_index)
            entity = &database->channels[absolute_index];
        return entity;
    }
    static __forceinline long simulation_gamestate_entity_get_simulation_entity_index(long gamestate_index)
    {
        s_simulation_gamestate_entity_datum *entity = datum_get<s_simulation_gamestate_entity_datum>(tls_simulation_gamestate_entity_data(), gamestate_index);
        long entity_index = entity->entity_index;
        return entity_index;
    }
    static __forceinline bool simulation_object_entity_is_local(c_simulation_entity_database *database, long entity_index)
    {
        const s_replication_entity_data *entity = &((c_replication_entity_manager *)database->entities)->m_entity_data[entity_index & 0x3ff];
        return (entity->flags & 4) != 0;
    }
}

// 0x40a9e0
const char *c_simulation_object_refresh_event_definition::event_type_name()
{
    return "object_refresh";
}


long simulation_object_get_replicated_object_from_entity(long entity_index)
{
    long object_index = -1;
    if (entity_index != -1)
    {
        c_simulation_world *world = simulation_get_world();
        s_simulation_entity *entity = entity_database_try_and_get(&world->m_distributed_world->m_entity_database, entity_index);
        if (entity)
        {
            c_simulation_entity_definition *definition = simulation_get_entity_definition(entity->entity_type);
            if (entity->exists_in_gameworld && entity->gamestate_index != -1 && definition->entity_type_is_gameworld_object())
                object_index = simulation_gamestate_entity_get_object_index(entity->gamestate_index);
        }
    }
    return object_index;
}

// @address 0x40aac0 ?simulation_object_get_authoritative_entity_internal@@YAJJ_N@Z
long simulation_object_get_authoritative_entity_internal(long object_index, bool validate)
{
    long result = -1;
    if (!game_is_cooperative() && object_index != -1)
    {
        c_simulation_world *world = simulation_get_world();
        c_simulation_entity_database *database = &world->m_distributed_world->m_entity_database;
        s_object_datum *object = object_get(object_index);
        long gamestate_index = object->simulation_entity_index;
        long entity_index = -1;
        if (gamestate_index != -1)
        {
            entity_index = simulation_gamestate_entity_get_simulation_entity_index(gamestate_index);
        }
        if (entity_index != -1 && (!validate || entity_database_try_and_get(database, entity_index)))
        {
            if (simulation_object_entity_is_local(database, entity_index))
                result = entity_index;
        }
    }
    return result;
}

// @address 0x40abb8 ?simulation_object_get_predictive_entity_internal@@YAJJ_N@Z
long simulation_object_get_predictive_entity_internal(long object_index, bool validate)
{
    long result = -1;
    if (!game_is_cooperative() && object_index != -1)
    {
        c_simulation_world *world = simulation_get_world();
        c_simulation_entity_database *database = &world->m_distributed_world->m_entity_database;
        s_object_datum *object = object_get(object_index);
        long gamestate_index = object->simulation_entity_index;
        long entity_index = -1;
        if (gamestate_index != -1)
        {
            entity_index = simulation_gamestate_entity_get_simulation_entity_index(gamestate_index);
        }
        if (entity_index != -1 && (!validate || entity_database_try_and_get(database, entity_index)))
        {
            if (!simulation_object_entity_is_local(database, entity_index))
                result = entity_index;
        }
    }
    return result;
}

bool c_simulation_object_entity_definition::entity_replication_required_for_view_activation(const s_simulation_entity *entity)
{
    bool result = false;
    long object_index = simulation_gamestate_entity_get_object_index(entity->gamestate_index);
    if (object_index != -1)
        result = this->object_required_to_join_game(object_index);
    return result;
}

void c_simulation_object_entity_definition::build_creation_data(long entity_index, long creation_data_size, void *creation_data)
{
    long object_index = simulation_gamestate_entity_get_object_index(entity_index);
    if (object_index != -1)
        this->build_object_creation_data(object_index, creation_data_size, creation_data);
}

bool c_simulation_object_entity_definition::build_updated_state_data(const s_simulation_entity *entity,
    unsigned long *update_mask, long argument, void *data)
{
    bool result = false;
    long object_index = simulation_gamestate_entity_get_object_index(entity->gamestate_index);
    if (object_index != -1)
    {
        unsigned long mask = *update_mask;
        if (object_get(object_index)->identifier.m_type == 1)
            mask &= 0x1ffff;
        *update_mask = this->handle_object_update(object_index, mask, argument, data);
        result = true;
    }
    return result;
}

// 0x40adf8
bool c_simulation_object_entity_definition::create_game_entity(long gamestate_index, long creation_data_size,
    const void *creation_data, unsigned long update_mask, long state_data_size, const void *state_data)
{
    int created_object_index;
    int object_index;
    s_object_header *header;
    object_datum *object_data;
    unsigned long creation_data_local;
    unsigned char is_simulation_mode;
    unsigned long creation_data_size_ref[1];
    bool result;
    creation_data_size_ref[0] = update_mask;
    result = false;
    created_object_index = this->create_object(creation_data_size, creation_data, creation_data_size_ref, state_data_size, state_data);
    object_index = created_object_index;
    if (object_index != -1)
    {
        header = object_header_get(object_index);
        object_data = header->datum;
        if (!(header->flags & 16))
        {
            datum_get<s_simulation_gamestate_entity_datum>(
                tls_simulation_gamestate_entity_data(), gamestate_index)->object_index = object_index;
            object_attach_gamestate_entity(created_object_index, gamestate_index);
            creation_data_local = creation_data_size_ref[0];
            result = true;
            if (creation_data_local != 0)
                this->apply_object_update(object_index, creation_data_local, state_data_size, state_data);
            is_simulation_mode = 0;
            if (tls_game_globals()->options.game_simulation == 4)
                is_simulation_mode = 1;
            if (is_simulation_mode)
            {
                object_data->simulation_object_index = tls_game_time_globals()->time;
                object_get_origin(object_index, &object_data->simulation_update_position);
            }
        }
    }
    return result;
}

bool c_simulation_object_entity_definition::update_game_entity(long entity_index, unsigned long update_mask, long argument, const void *data)
{
    bool result = false;
    long object_index = simulation_gamestate_entity_get_object_index(entity_index);
    if (object_index != -1 && this->gameworld_attachment_valid(entity_index))
    {
        this->apply_object_update(object_index, update_mask, argument, data);
        result = true;
    }
    return result;
}

bool c_simulation_object_entity_definition::delete_game_entity(long entity_index)
{
    bool result = false;
    long object_index = simulation_gamestate_entity_get_object_index(entity_index);
    if (object_index != -1 && this->gameworld_attachment_valid(entity_index))
    {
        if (!this->handle_delete_object(object_index))
        {
            object_detach_gamestate_entity(object_index, entity_index);
            object_delete(object_index);
        }
        result = true;
    }
    return result;
}

bool c_simulation_object_entity_definition::promote_game_entity_to_authority(long entity_index)
{
    bool result = false;
    long object_index = simulation_gamestate_entity_get_object_index(entity_index);
    if (object_index != -1 && this->gameworld_attachment_valid(entity_index))
        result = this->promote_object_to_authority(object_index);
    return result;
}

bool c_simulation_object_entity_definition::gameworld_attachment_valid(long entity_index)
{
    bool result = false;
    long object_index = simulation_gamestate_entity_get_object_index(entity_index);
    if (object_index != -1)
    {
        s_object_datum *object = (s_object_datum *)object_try_and_get_and_verify_type(object_index, (unsigned long)-1);
        if (object != 0 && object->simulation_entity_index == entity_index)
            result = true;
    }
    return result;
}
static __forceinline float simulation_game_objects_scenario_object_get_scale(const s_scenario_object *object)
{
    return object->scale > 0.0f ? object->scale : 1.0f;
}

// @address 0x40b128 ?object_build_baseline_state_data@c_simulation_object_entity_definition@@KA_NPBUs_simulation_object_creation_data@@PAUs_simulation_object_state_data@@@Z
bool c_simulation_object_entity_definition::object_build_baseline_state_data(
    const s_simulation_object_creation_data *creation_data, s_simulation_object_state_data *state_data)
{
    bool success = false;
    if (tag_try_and_get_unsafe('obje', creation_data->definition_index))
    {
        const s_object_definition *definition =
            (const s_object_definition *)tag_get('obje', creation_data->definition_index);
        state_data->position = *global_origin3d;
        state_data->forward = *global_forward3d;
        state_data->up = *global_up3d;
        state_data->scale = 1.0f;
        if (creation_data->scenario_datum_index != -1)
        {
            long object_type = definition->object_type;
            const object_type_definition *type_definition = object_type_definitions[object_type];
            if (type_definition->scenario_datums_offset != -1 && type_definition->scenario_palette_offset != -1)
            {
                scenario *scenario_data = global_scenario;
                long datum_size = 0;
                long datums_offset = (short)object_type_definitions[object_type]->scenario_datums_offset;
                datum_size = object_type_definitions[object_type]->scenario_datum_size;
                const s_tag_block *datums = (s_tag_block *)((char *)scenario_data + datums_offset);
                if (creation_data->scenario_datum_index >= 0 && creation_data->scenario_datum_index < datums->count)
                {
                    const s_scenario_object *placement = (const s_scenario_object *)tag_block_get_range_with_size(datums,
                        creation_data->scenario_datum_index, 1, datum_size);
                    state_data->position = placement->position;
                    vectors3d_from_euler_angles3d(&state_data->forward, &state_data->up, &placement->rotation);
                    state_data->scale = simulation_game_objects_scenario_object_get_scale(placement);
                }
            }
        }
        state_data->region_count = 1;
        state_data->constraint_count = 0;
        state_data->body_stun_ticks_zero = true;
        state_data->shield_stun_ticks_zero = true;
        state_data->shield_stunned = false;
        state_data->shield_depleted = false;
        if (definition->model.index != -1)
        {
            const s_model_definition *model = (const s_model_definition *)tag_get(
                'hlmt', definition->model.index);
            if (model->damage_info.count > 0)
            {
                const s_model_damage_info *damage =
                    (const s_model_damage_info *)tag_block_get_range_with_size(&model->damage_info, 0, 1, sizeof(s_model_damage_info));
                if (damage->maximum_body_vitality > 0.0f)
                    state_data->body_vitality = 1.0f;
                if (damage->maximum_shield_vitality > 0.0f)
                    state_data->shield_vitality = 1.0f;
                state_data->constraint_count = (unsigned char)damage->constraints.count;
            }
            if (model->regions.count > 0)
                state_data->region_count = (unsigned char)model->regions.count;
        }
        state_data->destroyed_regions = -1;
        state_data->parent_entity_index = -1;
        state_data->parent_gamestate_index = -1;
        state_data->parent_node_index = -1;
        state_data->parent_attached = false;
        state_data->parent_up = *global_up3d;
        state_data->parent_forward = *global_forward3d;
        state_data->map_variant_index = creation_data->map_variant_index;
        success = true;
    }
    return success;
}

static void get_model_region_information(long object_index, long *region_count,
    char **region_permutations, char **visible_permutations, object_region_information **region_information);
static __forceinline real_vector3d *simulation_game_objects_transform_vector(const real_matrix3x3 *matrix,
    const real_vector3d *vector, real_vector3d *result)
{
    float i = vector->i;
    float j = vector->j;
    float k = vector->k;
    result->i = matrix->left.i * j + matrix->up.i * k + matrix->forward.i * i;
    result->j = matrix->left.j * j + matrix->up.j * k + matrix->forward.j * i;
    result->k = matrix->left.k * j + matrix->up.k * k + matrix->forward.k * i;
    return result;
}

// @address 0x40b6c0 ?object_handle_update@c_simulation_object_entity_definition@@KAKJKPAUs_simulation_object_state_data@@@Z
unsigned long c_simulation_object_entity_definition::object_handle_update(
    long object_index, unsigned long update_mask, s_simulation_object_state_data *state_data)
{
    s_object_datum *object = object_get(object_index);
    unsigned long changed = 0;
    real_point3d position;
    if (update_mask & 1)
    {
        if (state_data->dead != (bool)((object->damage_flags >> 2) & 1))
        {
            state_data->dead = (bool)((object->damage_flags >> 2) & 1);
            changed |= 1;
        }
    }

    float epsilon = 0.0001f;
    if (object->parent_object_index == -1)
    {
        if (update_mask & 2)
        {
            real_rectangle3d bounds;
            simulation_encoding_get_world_bounds(&bounds);
            position.x = PIN(object->position.x, bounds.x0, bounds.x1);
            position.y = PIN(object->position.y, bounds.y0, bounds.y1);
            position.z = PIN(object->position.z, bounds.z0, bounds.z1);
            if (fabsf(state_data->position.x - position.x) >= epsilon || fabsf(state_data->position.y - position.y) >= epsilon ||
                fabsf(state_data->position.z - position.z) >= epsilon)
            {
                state_data->position = position;
                changed |= 2;
            }
        }
        if ((update_mask & 4) &&
            (fabsf(state_data->forward.i - object->forward.i) >= epsilon || fabsf(state_data->forward.j - object->forward.j) >= epsilon ||
                fabsf(state_data->forward.k - object->forward.k) >= epsilon || fabsf(state_data->up.i - object->up.i) >= epsilon ||
                fabsf(state_data->up.j - object->up.j) >= epsilon || fabsf(state_data->up.k - object->up.k) >= epsilon))
        {
            state_data->forward = object->forward;
            state_data->up = object->up;
            changed |= 4;
        }
    }
    if (update_mask & 8)
    {
        float scale = PIN(object->scale, 0.0f, 10.0f);
        if (state_data->scale != scale)
        {
            state_data->scale = scale;
            changed |= 8;
        }
    }
    {
        real_vector3d velocity;
        if (update_mask & 16)
        {
            velocity = object->translational_velocity;
            limit3d(&velocity, 350.0f);
            if (fabsf(state_data->translational_velocity.i - velocity.i) >= epsilon ||
                fabsf(state_data->translational_velocity.j - velocity.j) >= epsilon ||
                fabsf(state_data->translational_velocity.k - velocity.k) >= epsilon)
            {
                state_data->translational_velocity = velocity;
                changed |= 16;
            }
        }
        if (update_mask & 32)
        {
            velocity = object->angular_velocity;
            limit3d(&velocity, 30.0f);
            if (fabsf(state_data->angular_velocity.i - velocity.i) >= epsilon ||
                fabsf(state_data->angular_velocity.j - velocity.j) >= epsilon ||
                fabsf(state_data->angular_velocity.k - velocity.k) >= epsilon)
            {
                state_data->angular_velocity = velocity;
                changed |= 32;
            }
        }
    }
    if (update_mask & 64)
    {
        float vitality = PIN(object->body_vitality, -1.0f, 1.0f);
        bool unstunned = object->body_stun_ticks == 0;
        if (state_data->body_vitality != vitality || state_data->body_stun_ticks_zero != unstunned)
        {
            state_data->body_vitality = vitality;
            state_data->body_stun_ticks_zero = unstunned;
            changed |= 64;
        }
    }
    if (update_mask & 128)
    {
        float vitality = PIN(object->shield_vitality, 0.0f, 4.0f);
        bool unstunned = object->shield_stun_ticks == 0;
        bool stunned = (bool)((object->damage_flags >> 10) & 1);
        bool depleted = (bool)((object->damage_flags >> 3) & 1);
        if (state_data->shield_vitality != vitality || state_data->shield_stun_ticks_zero != unstunned ||
            state_data->shield_stunned != stunned || state_data->shield_depleted != depleted)
        {
            state_data->shield_vitality = vitality;
            state_data->shield_stun_ticks_zero = unstunned;
            state_data->shield_stunned = stunned;
            state_data->shield_depleted = depleted;
            changed |= 128;
        }
    }
    if (update_mask & 256)
    {
        object_region_information *regions = 0;
        long region_count = 0;
        get_model_region_information(object_index, &region_count, 0, 0, &regions);
        if (state_data->region_count == region_count && region_count > 0)
        {
            unsigned char *source = (unsigned char *)regions + 1;
            unsigned char *destination = &state_data->region_states[0];
            unsigned long remaining = (unsigned long)region_count;
            do
            {
                unsigned char value = *source;
                if (*destination != (signed char)value)
                {
                    *destination = value;
                    changed |= 256;
                }
                remaining = remaining - 1;
                source = source + 8;
                destination = destination + 1;
            } while (remaining != 0);
        }
    }
    if (update_mask & 512)
    {
        const s_object_definition *definition =
            TAG_GET('obje', const s_object_definition, object->definition_index);
        long constraint_count = 0;
        if (definition->model.index != -1)
        {
            const s_model_definition *model = (const s_model_definition *)tag_get(
                'hlmt', definition->model.index);
            if (model->damage_info.count > 0)
                constraint_count = ((s_model_damage_info *)model->damage_info.address)->constraints.count;
        }
        if (state_data->constraint_count == constraint_count)
        {
            if (state_data->destroyed_constraints != object->destroyed_constraints ||
                state_data->loosened_constraints != object->loosened_constraints)
            {
                state_data->destroyed_constraints |= object->destroyed_constraints;
                state_data->loosened_constraints |= object->loosened_constraints;
                changed |= 512;
            }
        }
    }
    if ((update_mask & 1024) && state_data->destroyed_regions != object->destroyed_regions)
    {
        state_data->destroyed_regions = object->destroyed_regions;
        changed |= 1024;
    }
    if (update_mask & 2048)
    {
        long parent_object_index = object->parent_object_index;
        long parent_entity_index = -1;
        long parent_node_index = -1;
        bool attached = (bool)((object->flags >> 26) & 1);
        if (parent_object_index != -1)
        {
            s_object_datum *parent = object_get(parent_object_index);
            if (parent->simulation_entity_index == -1)
            {
                if (parent->parent_object_index != -1 && object_header_get(parent->parent_object_index)->object_type == 0)
                {
                    parent_node_index = parent->parent_node_index;
                    parent_object_index = parent->parent_object_index;
                }
                else
                    parent_node_index = object->parent_node_index;
            }
            else
                parent_node_index = object->parent_node_index;
            parent_entity_index = simulation_object_get_authoritative_entity_internal(parent_object_index, false);
        }
        if (state_data->parent_entity_index != parent_entity_index || state_data->parent_node_index != parent_node_index ||
            state_data->parent_attached != attached)
        {
            state_data->parent_entity_index = parent_entity_index;
            state_data->parent_node_index = parent_node_index;
            state_data->parent_attached = attached;
            real_point3d *parent_position = &state_data->parent_position;
            object_get_origin(object_index, parent_position);
            ::object_get_orientation(object_index, &state_data->parent_forward, &state_data->parent_up);
            if (parent_entity_index != -1 && parent_node_index != -1)
            {
                long parent_matrices_count;
                const real_matrix4x3 *parent_matrices = object_get_node_matrices(parent_object_index, &parent_matrices_count);
                const real_matrix4x3 *parent_matrix = &parent_matrices[parent_node_index];
                real_matrix4x3 inverse;
                matrix4x3_inverse(parent_matrix, &inverse);
                matrix4x3_transform_point(&inverse, parent_position, parent_position);
                real_matrix3x3 *rotation = &inverse.rotation;
                simulation_game_objects_transform_vector(rotation, &state_data->parent_up, &state_data->parent_up);
                simulation_game_objects_transform_vector(rotation, &state_data->parent_forward, &state_data->parent_forward);
            }
            changed |= 2048;
        }
    }
    if (update_mask & 4096)
    {
        void *object_data = object_try_and_get_and_verify_type(object_index, (unsigned long)-1);
        if (object_data != 0)
        {
            short multiplayer_offset = ((const object_datum *)object_data)->multiplayer_data_offset;
            if (multiplayer_offset != -1)
            {
                s_multiplayer_object_properties *multiplayer = (s_multiplayer_object_properties *)((char *)object_data + multiplayer_offset);
                if (multiplayer != 0 && state_data->multiplayer_flags != multiplayer->flags)
                {
                    state_data->multiplayer_flags = multiplayer->flags;
                    changed |= 4096;
                }
            }
        }
    }
    if (update_mask & 8192)
    {
        long map_variant_index = object->map_variant_index;
        if (map_variant_index != state_data->map_variant_index)
        {
            state_data->map_variant_index = map_variant_index;
            changed |= 8192;
            return changed;
        }
    }
    return changed;
}

static __forceinline void get_model_region_information(long object_index, long *region_count,
    char **region_permutations, char **visible_permutations, object_region_information **region_information)
{
    s_object_datum *object = object_get(object_index);
    *region_count = object->region_information_size;
    char *base = (char *)object + object->region_information_offset;
    bool region_damaged = flags_test(object->flags, 11);
    if (region_damaged)
        *region_count -= 16;
    *region_count /= 10;
    if (region_information)
        *region_information = (object_region_information *)base;
    if (region_permutations)
        *region_permutations = base + *region_count * 8;
    if (visible_permutations)
        *visible_permutations = base + *region_count * 9;
}
static __forceinline void simulation_game_objects_object_set_body_vitality(long object_index, float body_vitality, short stun_ticks)
{
    s_object_datum *object = object_get(object_index);
    object->body_vitality = body_vitality;
    object->body_stun_ticks = stun_ticks;
}
static __forceinline void simulation_game_objects_object_set_shield_stun_ticks(long object_index, long stun_ticks)
{
    s_object_datum *object = object_get(object_index);
    stun_ticks = PIN(stun_ticks, 0, 32766);
    object->shield_stun_ticks = (short)stun_ticks;
}
static __forceinline void simulation_game_objects_object_set_shield_vitality(
    long object_index, float shield_vitality, short stun_ticks, bool depleted, bool recharging)
{
    s_object_datum *object = object_get(object_index);
    object->shield_vitality = shield_vitality;
    simulation_game_objects_object_set_shield_stun_ticks(object_index, stun_ticks);
    if (recharging)
        object->damage_flags |= FLAG(11);
    else
        object->damage_flags &= ~FLAG(11);
    object_set_shield_depleted(object_index, depleted, damage_owner_unknown());
}
static __forceinline void simulation_game_objects_object_set_region_state(long object_index, long region_count, const unsigned char *region_states)
{
    long object_region_count;
    get_model_region_information(object_index, &object_region_count, 0, 0, 0);
    for (long region_index = 0; region_index < object_region_count; region_index++)
        object_set_model_state(object_index, region_index, region_states[region_index], false);
}
static __forceinline bool simulation_game_objects_multiplayer_flags_valid(
    c_flags<e_multiplayer_object_data_flags, short, k_multiplayer_object_data_flags_count> flags)
{
    bool valid = true;
    valid = valid && (flags.get() >> k_multiplayer_object_data_flags_count) == 0;
    return valid;
}
static __forceinline real_matrix4x3 *simulation_game_objects_object_get_node_matrices_with_count(long object_index, long *node_count)
{
    s_object_datum *object = object_get(object_index);
    *node_count = object->node_matrices_size / sizeof(real_matrix4x3);
    return (real_matrix4x3 *)((char *)object + object->node_matrices_offset);
}

// @address 0x40bff0 ?object_apply_update@c_simulation_object_entity_definition@@IAAXJKPBUs_simulation_object_state_data@@@Z
void c_simulation_object_entity_definition::object_apply_update(long object_index, unsigned long update_mask,
    const s_simulation_object_state_data *state_data)
{
    if (TEST_BIT(update_mask, 0) && state_data->dead)
        object_cause_fatal_damage_aftermath(object_index);

    if (TEST_BIT(update_mask, 1) || TEST_BIT(update_mask, 2) || TEST_BIT(update_mask, 4) || TEST_BIT(update_mask, 5))
    {
        if (object_get(object_index)->parent_object_index == -1)
        {
            const real_point3d *position = 0;
            const real_vector3d *forward = 0;
            const real_vector3d *up = 0;
            const real_vector3d *translational_velocity = 0;
            const real_vector3d *angular_velocity = 0;
            if (TEST_BIT(update_mask, 1))
                position = &state_data->position;
            if (TEST_BIT(update_mask, 2))
            {
                forward = &state_data->forward;
                up = &state_data->up;
            }
            if (TEST_BIT(update_mask, 4))
                translational_velocity = &state_data->translational_velocity;
            if (TEST_BIT(update_mask, 5))
                angular_velocity = &state_data->angular_velocity;
            e_simulation_game_position_update_context context = simulation_query_local_predictive_control(object_index, 0)
                ? _simulation_game_position_update_context_predicted
                : _simulation_game_position_update_context_authority;
            e_simulation_game_position_update_result result = simulation_evaluate_position_update(
                object_index, context, position, forward, up, translational_velocity, angular_velocity);
            simulation_apply_position_update(object_index, context, result, position, forward, up, translational_velocity, angular_velocity);
        }
    }

    if (TEST_BIT(update_mask, 3))
        object_set_scale(object_index, state_data->scale, 0.0f, true);

    if (TEST_BIT(update_mask, 6))
        simulation_game_objects_object_set_body_vitality(object_index, state_data->body_vitality, !state_data->body_stun_ticks_zero);

    if (TEST_BIT(update_mask, 7))
    {
        float shield_vitality = state_data->shield_vitality;
        s_object_datum *object = object_get(object_index);
        if (shield_vitality < object->shield_vitality && state_data->shield_stunned && flags_test(object->damage_flags, 10))
            shield_vitality = object->shield_vitality;
        simulation_game_objects_object_set_shield_vitality(
            object_index, shield_vitality, !state_data->shield_stun_ticks_zero, state_data->shield_depleted, state_data->shield_stunned);
    }

    if (TEST_BIT(update_mask, 8))
    {
        long region_count;
        get_model_region_information(object_index, &region_count, 0, 0, 0);
        if (region_count == state_data->region_count)
            simulation_game_objects_object_set_region_state(object_index, state_data->region_count, state_data->region_states);
    }

    if (TEST_BIT(update_mask, 9))
    {
        s_object_datum *object = object_get(object_index);
        const s_object_definition *definition =
            (const s_object_definition *)tag_get('obje', object->definition_index);
        long constraint_count = 0;
        if (definition->model.index != -1)
        {
            const s_model_definition *model = (const s_model_definition *)tag_get(
                'hlmt', definition->model.index);
            if (model->damage_info.count > 0)
                constraint_count = ((s_model_damage_info *)model->damage_info.address)->constraints.count;
        }
        if (state_data->constraint_count == constraint_count && (state_data->destroyed_constraints || state_data->loosened_constraints))
        {
            object_damage_constraints(object_index, state_data->destroyed_constraints, state_data->loosened_constraints);
            object_set_at_rest(object_index, false);
        }
    }

    if (TEST_BIT(update_mask, 10))
    {
        short destroyed_regions = state_data->destroyed_regions;
        if (destroyed_regions == -1 || VALID_INDEX(destroyed_regions, 8))
            object_get(object_index)->destroyed_regions = destroyed_regions;
    }

    if (TEST_BIT(update_mask, 11))
    {
        long parent_gamestate_index = state_data->parent_gamestate_index;
        long parent_object_index = -1;
        bool parent_valid = true;
        s_object_datum *object = object_get(object_index);
        if (parent_gamestate_index != -1)
        {
            parent_object_index = simulation_gamestate_entity_get_object_index(parent_gamestate_index);
            if (parent_object_index == -1)
                parent_valid = false;
        }
        if (parent_valid && parent_object_index != -1)
        {
            long parent_node_count;
            simulation_game_objects_object_get_node_matrices_with_count(parent_object_index, &parent_node_count);
            if (!VALID_INDEX(state_data->parent_node_index, parent_node_count))
                parent_valid = false;
        }
        if (parent_valid)
        {
            bool detach = false;
            bool attach = false;
            if (object->parent_object_index != parent_object_index)
            {
                if (object->parent_object_index != -1)
                    detach = true;
                if (parent_object_index != -1)
                    attach = true;
            }
            else if (object->parent_object_index != -1 && object->parent_node_index != state_data->parent_node_index)
            {
                detach = true;
                attach = true;
            }
            if (detach)
                object_detach(object_index);
            if (attach)
            {
                long parent_node_count;
                real_matrix4x3 *node_matrices = simulation_game_objects_object_get_node_matrices_with_count(parent_object_index, &parent_node_count);
                bool attach_to_parent = true;
                for (long ancestor_index = parent_object_index; ancestor_index != -1;
                    ancestor_index = object_get(ancestor_index)->parent_object_index)
                {
                    if (ancestor_index == object_index)
                    {
                        attach_to_parent = false;
                        break;
                    }
                }
                real_point3d position;
                real_vector3d forward;
                real_vector3d up;
                matrix4x3_transform_point(&node_matrices[state_data->parent_node_index], &state_data->parent_position, &position);
                matrix4x3_transform_normal(&node_matrices[state_data->parent_node_index], &state_data->parent_up, &up);
                matrix4x3_transform_normal(&node_matrices[state_data->parent_node_index],
                    &state_data->parent_forward, &forward);
                object_set_position_direct(object_index, &position, &forward, &up, 0, false);
                if (attach_to_parent)
                {
                    object_attach_to_node_with_type(parent_object_index, object_index, (short)state_data->parent_node_index);
                    if (state_data->parent_attached)
                        object->flags |= FLAG(26);
                    else
                        object->flags &= ~FLAG(26);
                }
            }
        }
    }

    if (TEST_BIT(update_mask, 12))
    {
        void *object_data = object_try_and_get_and_verify_type(object_index, (unsigned long)-1);
        if (object_data)
        {
            short multiplayer_offset = ((s_object_datum *)object_data)->multiplayer_data_offset;
            if (multiplayer_offset != -1)
            {
                s_multiplayer_object_properties *multiplayer = (s_multiplayer_object_properties *)((char *)object_data + multiplayer_offset);
                if (multiplayer)
                {
                    c_flags<e_multiplayer_object_data_flags, short,
                        k_multiplayer_object_data_flags_count> multiplayer_flags(state_data->multiplayer_flags);
                    if (simulation_game_objects_multiplayer_flags_valid(multiplayer_flags))
                    {
                        if (!flags_test(multiplayer_flags.get(), 11) &&
                            flags_test(multiplayer->flags, 11))
                            game_engine_sandbox_set_object_connected(object_index, 0);
                        multiplayer->flags = multiplayer_flags;
                    }
                }
            }
        }
    }

    if (TEST_BIT(update_mask, 13))
    {
        if (object_get_and_verify_type(object_index, (unsigned long)-1) != 0)
        {
            s_object_datum *object = (s_object_datum *)object_get_and_verify_type(object_index, (unsigned long)-1);
            short map_variant_index = state_data->map_variant_index;
            c_map_variant *map_variant = &tls_game_engine_globals()->map_info;
            if (map_variant_index != -1)
            {
                if (map_variant_index < map_variant->m_number_of_scenario_objects && object->name_index == -1)
                    map_variant_index = -1;
            }
            else if (object->map_variant_index != -1)
            {
                map_variant->remove_object(object->map_variant_index, _object_removal_origin_0);
            }
            object->map_variant_index = map_variant_index;
        }
    }
}

// @address 0x40c818 ?object_creation_encode@c_simulation_object_entity_definition@@KAXPBUs_simulation_object_creation_data@@PAVc_bitstream@@_N@Z
void c_simulation_object_entity_definition::object_creation_encode(const s_simulation_object_creation_data *creation_data,
    c_bitstream *packet, bool quantize)
{
    simulation_write_definition_index("object-definition-index", packet, creation_data->definition_index);
    packet->write_bool("object-scenario-datum-index-exists", creation_data->scenario_datum_index != -1);
    if (creation_data->scenario_datum_index != -1)
        packet->write_integer("object-scenario-datum-index", creation_data->scenario_datum_index, 13);
    packet->write_bool("object-map-variant-index-exists", creation_data->map_variant_index != -1);
    if (creation_data->map_variant_index != -1)
        packet->write_integer("object-map-variant-index", creation_data->map_variant_index, 10);
    packet->write_bool("allow-duplicate-scenario-objects", creation_data->allow_duplicate_scenario_objects);
    packet->write_integer("child-variant-index", creation_data->child_variant_index + 1, 5);
}

// @address 0x40c8e0 ?object_creation_decode@c_simulation_object_entity_definition@@KA_NPAUs_simulation_object_creation_data@@PAVc_bitstream@@_N@Z
bool c_simulation_object_entity_definition::object_creation_decode(s_simulation_object_creation_data *creation_data,
    c_bitstream *packet, bool quantize)
{
    creation_data->definition_index = simulation_read_definition_index("object-definition-index", packet);
    if (packet->read_bool("object-scenario-datum-index-exists"))
        creation_data->scenario_datum_index = packet->read_integer("object-scenario-datum-index", 13);
    else
        creation_data->scenario_datum_index = -1;
    if (packet->read_bool("object-map-variant-index-exists"))
        creation_data->map_variant_index = (short)packet->read_integer("object-map-variant-index", 10);
    else
        creation_data->map_variant_index = -1;
    creation_data->allow_duplicate_scenario_objects = packet->read_bool("allow-duplicate-scenario-objects");
    creation_data->child_variant_index = packet->read_integer("child-variant-index", 5) - 1;
    return !packet->overflowed() && creation_data->definition_index != -1;
}

void c_simulation_object_entity_definition::object_calculate_update_requirements(
    const s_simulation_entity *, const s_entity_update_data *update, long *required_bits)
{
    long minimum_bits = 0x7fffffff;
    unsigned long update_mask = update->update_mask;
    if ((update_mask & 2) != 0)
    {
        long bits = 62;
        minimum_bits = minimum_bits > bits ? bits : minimum_bits;
    }
    if ((update_mask & 4) != 0)
    {
        long bits = 41;
        minimum_bits = minimum_bits > bits ? bits : minimum_bits;
    }
    if ((update_mask & 8) != 0)
    {
        long bits = 22;
        minimum_bits = minimum_bits > bits ? bits : minimum_bits;
    }
    if ((update_mask & 16) != 0 || (update_mask & 32) != 0)
    {
        long bits = 43;
        minimum_bits = minimum_bits > bits ? bits : minimum_bits;
    }
    if ((update_mask & 64) != 0)
    {
        long bits = 23;
        minimum_bits = minimum_bits > bits ? bits : minimum_bits;
    }
    if ((update_mask & 128) != 0)
    {
        long bits = 24;
        minimum_bits = minimum_bits > bits ? bits : minimum_bits;
    }
    if ((update_mask & 1) != 0)
    {
        long bits = 15;
        minimum_bits = minimum_bits > bits ? bits : minimum_bits;
    }
    if ((update_mask & 256) != 0)
    {
        long bits = 18;
        bits += 48;
        minimum_bits = minimum_bits > bits ? bits : minimum_bits;
    }
    if ((update_mask & 512) != 0)
    {
        long bits = 19;
        bits += 32;
        minimum_bits = minimum_bits > bits ? bits : minimum_bits;
    }
    if ((update_mask & 1024) != 0)
    {
        long bits = 19;
        minimum_bits = minimum_bits > bits ? bits : minimum_bits;
    }
    if ((update_mask & 2048) != 0)
    {
        long bits = 112;
        minimum_bits = minimum_bits > bits ? bits : minimum_bits;
    }
    if ((update_mask & 4096) != 0)
    {
        long bits = 26;
        minimum_bits = minimum_bits > bits ? bits : minimum_bits;
    }
    if ((update_mask & 8192) != 0)
    {
        long bits = 30;
        minimum_bits = minimum_bits > bits ? bits : minimum_bits;
    }
    if (minimum_bits == 0x7fffffff)
        minimum_bits = 0;
    *required_bits = minimum_bits;
}

bool c_simulation_object_entity_definition::object_update_encode(bool initial_update, unsigned long update_mask,
    unsigned long *updated_mask, const s_simulation_view_telemetry_data *telemetry, const s_simulation_object_state_data *state_data,
    c_bitstream *packet, long maximum_bits, bool final_update, bool is_replicated)
{
    c_entity_update_encode_helper helper;
    bool result = false;
    if (helper.make_room_for_update(packet, maximum_bits, 0, 14, update_mask))
    {
        if (helper.write_component_header(0, "dead-exists"))
            packet->write_bool("dead", state_data->dead);
        helper.finish_component();
        if (helper.write_component_header(1, "position-exists"))
        {
            bool within_world = initial_update || final_update;
            simulation_write_quantized_position(packet, &state_data->position, 16, within_world, 0);
        }
        helper.finish_component();
        if (helper.write_component_header(2, "forward-and-up-exists"))
            packet->write_axes("forward-and-up", &state_data->forward, &state_data->up);
        helper.finish_component();
        if (helper.write_component_header(3, "scale-exists"))
        {
            bool identity = (float)fabs(state_data->scale - 1.0f) < k_real_epsilon;
            packet->write_bool("scale-is-identity", identity);
            if (!identity)
                packet->write_quantized_real("scale", state_data->scale, 0.0f,
                    k_simulation_object_maximum_scale, 7, false, false);
        }
        helper.finish_component();
        if (helper.write_component_header(4, "translational-velocity-exists"))
            packet->write_vector("translational-velocity", &state_data->translational_velocity,
                k_simulation_object_minimum_velocity, k_simulation_object_maximum_translational_velocity, 10);
        helper.finish_component();
        if (helper.write_component_header(5, "angular-velocity-exists"))
            packet->write_vector("angular-velocity", &state_data->angular_velocity, k_simulation_object_minimum_velocity,
                k_simulation_object_maximum_angular_velocity, 8);
        helper.finish_component();
        if (helper.write_component_header(6, "body-vitality-exists"))
        {
            packet->write_quantized_real("body-vitality", state_data->body_vitality,
                k_simulation_object_minimum_body_vitality, k_simulation_object_maximum_body_vitality, 8, true, true);
            packet->write_bool("body-stun-ticks-is-zero", state_data->body_stun_ticks_zero);
        }
        helper.finish_component();
        if (helper.write_component_header(7, "shield-vitality-exists"))
        {
            packet->write_quantized_real("shield-vitality", state_data->shield_vitality,
                k_simulation_object_minimum_shield_vitality, k_simulation_object_maximum_shield_vitality, 8, true, true);
            packet->write_bool("shield-stun-ticks-is-zero", state_data->shield_stun_ticks_zero);
            packet->write_bool("shield-charging", state_data->shield_stunned);
            packet->write_bool("shield-depleted", state_data->shield_depleted);
        }
        helper.finish_component();
        if (helper.write_component_header(8, "region-state-exists"))
        {
            packet->write_integer("region-count", state_data->region_count, 4);
            for (long region_index = 0; region_index < 16; region_index++)
                packet->write_integer("region-state", state_data->region_states[region_index], 3);
        }
        helper.finish_component();
        if (helper.write_component_header(9, "constraint-state-exists"))
        {
            packet->write_integer("constraint-count", state_data->constraint_count, 5);
            if (state_data->constraint_count > 0)
            {
                packet->write_integer("destroyed-constraints", state_data->destroyed_constraints, state_data->constraint_count);
                packet->write_integer("loosened-constraints", state_data->loosened_constraints, state_data->constraint_count);
            }
        }
        helper.finish_component();
        if (helper.write_component_header(10, "owner-team-index-exists"))
            packet->write_integer("owner-team-index", state_data->destroyed_regions + 1, 5);
        helper.finish_component();
        if (helper.write_component_header(11, "parent-state-exists"))
        {
            if (is_replicated)
            {
                bool active = true;
                if (state_data->parent_entity_index != -1 && telemetry)
                    active = telemetry->provider->entity_is_active(state_data->parent_entity_index);
                if (active)
                {
                    bool attached = state_data->parent_entity_index != -1 && state_data->parent_node_index != -1;
                    packet->write_bool("attached", attached);
                    if (attached)
                    {
                        replication_entity_index_encode(packet, state_data->parent_entity_index);
                        packet->write_integer("node", state_data->parent_node_index, 8);
                        packet->write_bool("initial-attachment", state_data->parent_attached);
                        simulation_write_quantized_node_space_position(packet, &state_data->parent_position,
                            _simulation_node_space_encoding_type_world, 16);
                        packet->write_axes("attachment-axes", &state_data->parent_forward, &state_data->parent_up);
                    }
                }
                else
                {
                    helper.skip_component();
                }
            }
            else
            {
                bool attached = state_data->parent_gamestate_index != -1 && state_data->parent_node_index != -1;
                packet->write_bool("attached", attached);
                if (attached)
                {
                    simulation_gamestate_index_encode(packet, state_data->parent_gamestate_index);
                    packet->write_integer("node", state_data->parent_node_index, 8);
                    packet->write_bool("initial-attachment", state_data->parent_attached);
                    simulation_write_quantized_node_space_position(packet, &state_data->parent_position,
                        _simulation_node_space_encoding_type_world, 16);
                    packet->write_axes("attachment-axes", &state_data->parent_forward, &state_data->parent_up);
                }
            }
        }
        helper.finish_component();
        if (helper.write_component_header(12, "multiplayer-properties-exists"))
            packet->write_integer("multiplayer-properties-data-flags", state_data->multiplayer_flags, 12);
        helper.finish_component();
        if (helper.write_component_header(13, "map-variant-index-exists"))
            packet->write_integer("map-variant-index", (unsigned short)state_data->map_variant_index, 16);
        helper.finish_component();
        helper.finish_update(updated_mask);
    result = true;
    }
    return result;
}

bool c_simulation_object_entity_definition::object_update_decode(
    bool initial_update, unsigned long *update_mask, s_simulation_object_state_data *state_data, c_bitstream *packet, bool is_replicated)
{
    bool valid = true;
    if (packet->read_bool("dead-exists"))
    {
        state_data->dead = packet->read_bool("dead");
        *update_mask |= 1;
    }
    if (packet->read_bool("position-exists"))
    {
        ::simulation_read_quantized_position(packet, &state_data->position, 16, 0);
        *update_mask |= 2;
        valid = valid && valid_real_point3d(&state_data->position);
    }
    if (packet->read_bool("forward-and-up-exists"))
    {
        packet->read_axes("forward-and-up", &state_data->forward, &state_data->up);
        *update_mask |= 4;
        valid = valid && valid_real_vector3d_axes2(&state_data->forward, &state_data->up);
    }
    if (packet->read_bool("scale-exists"))
    {
        if (packet->read_bool("scale-is-identity"))
            state_data->scale = 1.0f;
        else
            state_data->scale = packet->read_quantized_real("scale", 0.0f,
                k_simulation_object_maximum_scale, 7, false);
        *update_mask |= 8;
        valid = valid && valid_real(state_data->scale);
    }
    if (packet->read_bool("translational-velocity-exists"))
    {
        packet->read_vector("translational-velocity", &state_data->translational_velocity, k_simulation_object_minimum_velocity,
            k_simulation_object_maximum_translational_velocity, 10);
        *update_mask |= 16;
        valid = valid && valid_real_vector3d(&state_data->translational_velocity);
    }
    if (packet->read_bool("angular-velocity-exists"))
    {
        packet->read_vector("angular-velocity", &state_data->angular_velocity, k_simulation_object_minimum_velocity,
            k_simulation_object_maximum_angular_velocity, 8);
        *update_mask |= 32;
        valid = valid && valid_real_vector3d(&state_data->angular_velocity);
    }
    if (packet->read_bool("body-vitality-exists"))
    {
        state_data->body_vitality = packet->read_quantized_real("body-vitality", k_simulation_object_minimum_body_vitality,
            k_simulation_object_maximum_body_vitality, 8, true);
        state_data->body_stun_ticks_zero = packet->read_bool("body-stun-ticks-is-zero");
        *update_mask |= 64;
        valid = valid && valid_real(state_data->body_vitality);
    }
    if (packet->read_bool("shield-vitality-exists"))
    {
        state_data->shield_vitality = packet->read_quantized_real("shield-vitality", k_simulation_object_minimum_shield_vitality,
            k_simulation_object_maximum_shield_vitality, 8, true);
        state_data->shield_stun_ticks_zero = packet->read_bool("shield-stun-ticks-is-zero");
        state_data->shield_stunned = packet->read_bool("shield-charging");
        state_data->shield_depleted = packet->read_bool("shield-depleted");
        *update_mask |= 128;
        valid = valid && valid_real(state_data->shield_vitality);
    }
    if (packet->read_bool("region-state-exists"))
    {
        state_data->region_count = (unsigned char)packet->read_integer("region-count", 4);
        valid = valid && state_data->region_count <= 16;
        for (long region_index = 0; region_index < 16; region_index++)
        {
            state_data->region_states[region_index] = (unsigned char)packet->read_integer("region-state", 3);
            valid = valid && state_data->region_states[region_index] < 12;
        }
        *update_mask |= 256;
    }
    if (packet->read_bool("constraint-state-exists"))
    {
        state_data->constraint_count = (unsigned char)packet->read_integer("constraint-count", 5);
        valid = valid && state_data->constraint_count <= 16;
        if (state_data->constraint_count > 0)
        {
            state_data->destroyed_constraints = (unsigned short)packet->read_integer("destroyed-constraints", state_data->constraint_count);
            state_data->loosened_constraints = (unsigned short)packet->read_integer("loosened-constraints", state_data->constraint_count);
        }
        *update_mask |= 512;
    }
    if (packet->read_bool("owner-team-index-exists"))
    {
        state_data->destroyed_regions = (short)(packet->read_integer("owner-team-index", 5) - 1);
        *update_mask |= 1024;
    }
    if (packet->read_bool("parent-state-exists"))
    {
        if (packet->read_bool("attached"))
        {
            if (is_replicated)
                replication_entity_index_decode(packet, &state_data->parent_entity_index);
            else
                simulation_gamestate_index_decode(packet, &state_data->parent_gamestate_index);
            state_data->parent_node_index = packet->read_integer("node", 8);
            state_data->parent_attached = packet->read_bool("initial-attachment");
            simulation_read_quantized_node_space_position(packet, &state_data->parent_position, 0, 16);
            packet->read_axes("attachment-axes", &state_data->parent_forward, &state_data->parent_up);
        }
        else
        {
            state_data->parent_gamestate_index = -1;
            state_data->parent_entity_index = -1;
            state_data->parent_node_index = -1;
            state_data->parent_attached = false;
        }
        *update_mask |= 2048;
    }
    if (packet->read_bool("multiplayer-properties-exists"))
    {
        state_data->multiplayer_flags = (unsigned short)packet->read_integer("multiplayer-properties-data-flags", 12);
        *update_mask |= 4096;
    }
    if (packet->read_bool("map-variant-index-exists"))
    {
        unsigned long map_variant_index = packet->read_integer("map-variant-index", 16);
        if (map_variant_index & 0x8000)
            map_variant_index |= 0xffff0000;
        state_data->map_variant_index = (short)map_variant_index;
        *update_mask |= 8192;
    }
    return valid && !packet->overflowed();
}


bool c_simulation_object_entity_definition::object_state_compare(s_simulation_object_state_data *first, s_simulation_object_state_data *second)
{
    bool equal = true;
    equal = equal && simulation_compare_quantized_positions(&first->position, &second->position, 16, 0);
    first->position = second->position = *global_origin3d;
    equal = equal && c_bitstream::compare_axes(&first->forward, &first->up, &second->forward, &second->up);
    first->forward = second->forward = *global_zero_vector3d;
    first->up = second->up = *global_zero_vector3d;
    equal = equal && c_bitstream::compare_quantized_reals(first->scale, second->scale, 0.0f,
        k_simulation_object_maximum_scale, 7, false, false);
    first->scale = second->scale = 0.0f;
    equal = equal && c_bitstream::compare_vectors(&first->translational_velocity, &second->translational_velocity,
        k_simulation_object_minimum_velocity, k_simulation_object_maximum_translational_velocity, 10);
    first->translational_velocity = second->translational_velocity = *global_zero_vector3d;
    equal = equal && c_bitstream::compare_vectors(&first->angular_velocity, &second->angular_velocity,
        k_simulation_object_minimum_velocity, k_simulation_object_maximum_angular_velocity, 8);
    first->angular_velocity = second->angular_velocity = *global_zero_vector3d;
    equal = equal && c_bitstream::compare_quantized_reals(first->body_vitality, second->body_vitality,
        k_simulation_object_minimum_body_vitality, k_simulation_object_maximum_body_vitality, 8, true, false);
    first->body_vitality = second->body_vitality = 0.0f;
    equal = equal && c_bitstream::compare_quantized_reals(first->shield_vitality, second->shield_vitality,
        k_simulation_object_minimum_shield_vitality, k_simulation_object_maximum_shield_vitality, 8, true, false);
    first->shield_vitality = second->shield_vitality = 0.0f;
    return equal;
}

// @address 0x40dc30 ?object_attach_to_initial_parent@c_simulation_object_entity_definition@@AAAXJPBUs_simulation_object_creation_data@@PBUs_simulation_object_state_data@@@Z
void c_simulation_object_entity_definition::object_attach_to_initial_parent(
    long object_index, const s_simulation_object_creation_data *creation_data, const s_simulation_object_state_data *state_data)
{
    long parent_object_index = simulation_gamestate_entity_get_object_index(state_data->parent_gamestate_index);
    s_object_datum *child = (s_object_datum *)object_get(object_index);
    if (parent_object_index != -1)
    {
        s_object_datum *parent = (s_object_datum *)object_get(parent_object_index);
        s_object_definition *definition =
            TAG_GET('obje', s_object_definition, parent->definition_index);
        s_model_definition *model = TAG_GET('hlmt', s_model_definition, definition->model.index);
        s_model_variant *variant = (s_model_variant *)model->variants.address + parent->variant_index;
        if (creation_data->child_variant_index >= 0 && creation_data->child_variant_index < variant->attachments.count)
        {
            s_model_variant_attachment *attachment = (s_model_variant_attachment *)variant->attachments.address +
                creation_data->child_variant_index;
            if (attachment->child_object.index != -1 && attachment->child_object.index == creation_data->definition_index)
            {
                object_attach_to_marker_with_type(parent_object_index, attachment->parent_marker, object_index, attachment->child_marker);
                child->flags |= 1 << 26;
                child->marker = creation_data->child_variant_index;
                if (((1 << parent->object_type) & 0x1003) != 0 && ((1 << child->object_type) & 0x1003) != 0)
                    unit_update_driver_and_gunner(parent_object_index);
            }
        }
    }
}

// @address 0x40dd60 ?calculate_relevance@c_simulation_object_refresh_event_definition@@UAAMPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@M@Z
float c_simulation_object_refresh_event_definition::calculate_relevance(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry, float relevance)
{
    return 1.0f;
}

// 0x40dd70
void c_simulation_object_refresh_event_definition::write_description_to_string(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry, float relevance, long buffer_size, char *buffer)
{
    csnzprintf(buffer, buffer_size, "object-refresh");
}

#if !defined(_XBOX) || defined(__clang__)
// The Xbox link folds these slots into identical functions of other units (vtable 0x4732c slots 0, 2-6, 9-12).
e_simulation_event_type c_simulation_object_refresh_event_definition::event_type()
{
    return _simulation_event_type_object_refresh;
}

long c_simulation_object_refresh_event_definition::payload_size()
{
    return 4;
}

long c_simulation_object_refresh_event_definition::number_of_entity_references()
{
    return 1;
}

bool c_simulation_object_refresh_event_definition::reference_delays_entity_deletion()
{
    return false;
}

bool c_simulation_object_refresh_event_definition::event_can_be_transmitted(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    return true;
}

long c_simulation_object_refresh_event_definition::minimum_required_bits(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    return 0;
}

s_simulation_event_requirements c_simulation_object_refresh_event_definition::event_requirements(
    const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry)
{
    s_simulation_event_requirements requirements;
    requirements.reference_count = 1;
    requirements.reference_delays_entity_deletion = 0;
    requirements.can_be_transmitted = 1;
    requirements.minimum_required_bits = 0;
    return requirements;
}

void c_simulation_object_refresh_event_definition::event_payload_encode(long payload_size, const void *payload,
    c_bitstream *packet, bool force)
{
}

bool c_simulation_object_refresh_event_definition::event_payload_decode(long payload_size, void *payload,
    c_bitstream *packet, bool force)
{
    return true;
}

void c_simulation_object_refresh_event_definition::prepare_event_data_for_gameworld(long payload_size, void *payload)
{
}

c_simulation_object_refresh_event_definition g_simulation_object_refresh_event_definition;
#endif

// @address 0x40b3a8 ?object_setup_placement_data@c_simulation_object_entity_definition@@KA_NPBUs_simulation_object_creation_data@@PBUs_simulation_object_state_data@@PAKPAUobject_placement_data@@@Z
bool c_simulation_object_entity_definition::object_setup_placement_data(struct s_simulation_object_creation_data const *p0,
    struct s_simulation_object_state_data const *p1, unsigned long *p2, struct object_placement_data *p3)
{
    bool result = false;
    if (p0->scenario_datum_index == -1)
    {
        object_placement_data_new(p3, p0->definition_index, -1, 0);
        p3->flags |= 0x12;
        if (*p2 & 0x2)
        {
            p3->position = p1->position;
            *p2 &= ~0x2;
        }
        if (*p2 & 0x4)
        {
            p3->forward = p1->forward;
            p3->up = p1->up;
            *p2 &= ~0x4;
        }
        if (*p2 & 0x8)
        {
            p3->scale = p1->scale;
            *p2 &= ~0x8;
        }
        if (*p2 & 0x10)
        {
            p3->linear_velocity = p1->translational_velocity;
            *p2 &= ~0x10;
        }
        if (*p2 & 0x20)
        {
            p3->angular_velocity = p1->angular_velocity;
            *p2 &= ~0x20;
        }
        result = true;
    }
    return result;
}


static __forceinline void simulation_game_objects_set_variant_flag(s_variant_object_datum *variant_object,
    e_variant_object_placement_flags bit, bool enable)
{
    unsigned short mask = (unsigned short)1 << (long)bit;
    if (enable)
        variant_object->flags.m_storage |= mask;
    else
        variant_object->flags.m_storage &= ~mask;
}

// @address 0x40b530
long c_simulation_object_entity_definition::object_create_object(const s_simulation_object_creation_data *creation_data,
    const s_simulation_object_state_data *state_data, unsigned long *flags, object_placement_data *placement_data)
{
    long object_index = -1;
    objects_garbage_collection();
    if (creation_data->definition_index != -1 && tag_try_and_get_unsafe('obje', creation_data->definition_index))
    {
        if (creation_data->scenario_datum_index != -1)
            object_index = object_placement_create_from_simulation(creation_data);
        else
            object_index = object_new(placement_data);

        if (object_index != -1)
        {
            if (state_data->parent_gamestate_index != -1 && state_data->parent_attached && creation_data->child_variant_index != -1)
                object_attach_to_initial_parent(object_index, creation_data, state_data);

            if (creation_data->map_variant_index != -1)
            {
                if (creation_data->allow_duplicate_scenario_objects)
                {
                    s_variant_object_datum variant_object;
                    long variant_object_index = creation_data->map_variant_index;
                    c_map_variant *map_variant = &tls_game_engine_globals()->map_info;
                    map_variant->get_variant_object_datum(variant_object_index, &variant_object);
                    simulation_game_objects_set_variant_flag(&variant_object, _variant_object_placement_preserve_helper_bit, true);
                    map_variant->set_variant_object_datum(variant_object_index, &variant_object, false);
                }

                s_variant_object_datum variant_object;
                tls_game_globals()->options.map_variant.get_variant_object_datum(creation_data->map_variant_index, &variant_object);
                if (variant_object.flags.test(_variant_object_placement_edited_bit))
                    tls_game_engine_globals()->map_info.apply_multiplayer_properties(&variant_object.properties, object_index);
                c_map_variant::find_definition_index(object_index, creation_data->map_variant_index, 1);
            }
        }
    }
    return object_index;
}

