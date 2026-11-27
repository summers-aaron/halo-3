// objectslibxbox/items.obj: from the records 0xeaa88 0xebe68 0x10df30 0x1ce2e8 0x1ce398 0x20a338 0x35c168 0x35c1e8 0x35c360 0x35c388 0x35c440 0x35c768 0x35cda0 0x35ce28 0x35ce88 0x35cf48 0x35d0a8
// @flags /TP /O2 /Oc
#include "memory/datum_index.h"
#include "ai/weapon_datum_types.h"
#include "math/random_math_range.h"
#include "game/game_simulation_types.h"
#include "game/game_engine_notifications.h"
#include "objects/object_reference_types.h"
#include "objects/damage_data.h"
#include "tag_files/tag_index.h"
#include "math/real_math_types.h"
#include "math/matrix_math_types.h"
#include "objects/object_types.h"
#include "objects/object_iterator.h"
#include "items/item_types.h"
#include "items/multiplayer_weapon_tracker.h"
#include "cseries/cseries.h"
#include "cseries/crt_xdk.h"
#include "game/game_engine.h"
#include "game/game_globals_storage.h"
#include "game/game_engine_scoring_globals.h"
#include "items/items.h"
#include "items/weapons.h"
#include "objects/object_early_movers.h"
#include "objects/objects.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "game/map_variant.h"
#include "game/materials.h"
#include "cache/cache_files.h"
#include "physics/havok_component.h"
#include "physics/havok_contact_point_iterator.h"
#include "physics/havok_utilities.h"
#include "effects/effect_vector_types.h"
#include "effects/effect_deterministic_types.h"
#include "math/real_math_globals.h"
#include "game/game_engine_list.h"
#include "game/game_engine_default.h"
#include "memory/data_types.h"
#include "game/game_time_inline.h"
#include "game/game_globals_inline.h"
#include "tag_files/tag_types.h"
#include "items/item_definitions.h"
#include "units/unit_definitions.h"
#include "math/real_math.h"
#include <new>
#include "effects/effect_creation_noalias.h"
#include "objects/damage.h"
// @address 0x20a338 ?game_engine_multiplayer_weapon_drop_internal@@YAXJJ@Z
// @address 0x35d0a8 ?item_adjust_scale@@YAXJJ@Z

extern "C" {
class c_game_engine;
}

// @address 0x1ce2e8 variant_object_datum_initialize
void variant_object_datum_initialize(s_variant_object_datum *datum)
{
    memset(datum, 0, sizeof(*datum));
    datum->object_index = -1;
    datum->helper_object_index = -1;
    datum->quota_index = -1;
    datum->parent_object_identifier.clear();
    datum->properties.initialize_to_default();
}

s_variant_object_datum::s_variant_object_datum()
{
    variant_object_datum_initialize(this);
}

void game_engine_multiplayer_weapon_drop_internal(long item_index, long inventory_context)
{
    s_multiplayer_weapon_tracker *weapon_tracker;
    weapon_tracker = game_engine_multiplayer_weapon_get_internal(item_index);
    ((weapon_datum *)((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(item_index)].datum)->flags &= 65407;
    weapon_tracker->carrier_unit_index = -1;
    weapon_tracker->carrier_player_index = -1;
    c_game_engine *engine = (tls_game_engine_globals()->game_engine_index >= 1 && tls_game_engine_globals()->game_engine_index <= 10
            ? game_engines[tls_game_engine_globals()->game_engine_index]
            : 0);
    engine->multiplayer_weapon_dropped(item_index, inventory_context);
}

bool dangerous_items_near_player(long *result_index)
{
    bool found_item = false;
    c_object_iterator<item_datum> iterator;
    iterator.begin(12, 1);
    while (iterator.next()) {
        if (iterator.get_datum()->remaining_count > 0) {
            *result_index = iterator.get_index();
            found_item = true;
            break;
        }
    }
    return found_item;
}

struct cache_file_tag_instance;
struct s_data_array;

float k_item_default_contact_normal_threshold = 0.86602539f;
extern "C" {
float g_item_contact_normal_threshold = k_item_default_contact_normal_threshold;
}
static __forceinline item_datum *items_object_datum_get(long object_index)
{
    return (item_datum *)((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(object_index)].datum;
}
static __forceinline void *items_tag_definition_get(long tag_index)
{
    return global_tag_instances[datum_index_to_absolute_index(tag_index)].definition;
}
static __forceinline item_datum *items_gravity_item_get(long index)
{
    return (item_datum *)items_object_datum_get(index);
}
static __forceinline c_havok_contact_point *items_gravity_contact_get(const c_havok_contact_point_iterator *it)
{
    long entity = items_gravity_item_get(it->object_index)->object.havok_component_index;
    return ((c_havok_component *)g_havok_component_data->data)[datum_index_to_absolute_index(entity)].get_contact_point(
        it->rigid_body_index, it->contact_point_index);
}

// @address 0xebe68 item_adjust_gravity
void item_adjust_gravity(long item_index, real_vector3d *gravity)
{
    item_datum *item = items_gravity_item_get(item_index);
    s_item_definition *definition = (s_item_definition *)items_tag_definition_get(item->object.definition_index);
    if (item->object.havok_component_index != -1 && definition->linear_damping != 0.0f) {
        float threshold = (item->inventory_flags & 4) ? 0.72f : g_item_contact_normal_threshold;
        float maximum = -3.402823466e+38f;
        real_vector3d normal;
        c_havok_contact_point_iterator iterator(item_index, false);
        for (; iterator.valid(); iterator.next()) {
            c_havok_contact_point *point = items_gravity_contact_get(&iterator);
            if (point->m_normal.k > maximum) {
                normal = point->m_normal;
                maximum = normal.k;
            }
        }
        if (maximum > threshold) {
            float dot = dot_product3d(&normal, gravity);
            float fraction = (maximum - threshold) / (1.0f - threshold);
            float damping = fraction * TLS_GAME_TIME_GLOBALS->tick_length * definition->linear_damping;
            damping = (damping > 0.0f ? damping : 0.0f) < 1.0f ? (damping > 0.0f ? damping : 0.0f) : 1.0f;
            real_vector3d parallel, perpendicular;
            scale_vector3d(gravity, -dot, &parallel);
            subtract_vectors3d(gravity, &parallel, &perpendicular);
            scale_vector3d(&perpendicular, 1.0f - damping, &perpendicular);
            add_vectors3d(&perpendicular, &parallel, gravity);
        }
    }
}

static __forceinline item_datum *items_scale_datum_get(long object_index)
{
    return (item_datum *)((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(object_index)].datum;
}

static __forceinline long items_scale_parent_get(const item_datum *item)
{
    long parent_index = -1;
    if (item->object.m_physics_flags.test(_object_physics_flag_1) || item->object.m_physics_flags.test(_object_physics_flag_2)) {
        parent_index = item->object.local_physics_object_index;
    }
    return parent_index;
}

void item_adjust_scale(long item_index, long unit_index)
{
    item_datum *item = items_scale_datum_get(item_index);
    s_item_definition *definition = (s_item_definition *)items_tag_definition_get(item->object.definition_index);
    long scale_index = -1;
    if (unit_index != -1 && unit_index != items_scale_parent_get(item)) {
        unsigned char relation = item->owner_relation;
        if (relation != 0) {
            item_datum *unit = items_scale_datum_get(item->unit_object_index);
            unit_definition *carrier_definition = (unit_definition *)items_tag_definition_get(unit->object.definition_index);
            unsigned char is_third = relation == 3;
            switch (carrier_definition->scale_class) {
            case 0: scale_index = 2 + (is_third != 0); break;
            case 1: scale_index = 4 + (is_third != 0); break;
            case 2: scale_index = 6 + (is_third != 0); break;
            case 3: scale_index = 8 + (is_third != 0); break;
            }
        }
    } else {
        scale_index = game_globals->options.game_mode == 2;
    }
    float scale;
    if (scale_index != -1) {
        scale = definition->scale[scale_index];
        if (scale <= 0.0f) {
            scale = 1.0f;
        }
    } else {
        scale = 1.0f;
    }
    object_set_scale(item_index, scale, 0.0f, false);
}

struct object_placement_data;

static __forceinline void items_flags_set(unsigned long *flags, long bit, bool value)
{
    if (value) *flags |= 1 << bit;
    else *flags &= ~(1 << bit);
}

static __forceinline void items_mark_relevance(long item_index)
{
    item_datum *item = items_object_datum_get(item_index);
    item->relevance_time = TLS_GAME_TIME_GLOBALS->time;
}

// @address 0x35c168 ?item_new@@YA_NJPAUobject_placement_data@@PA_N@Z
bool item_new(long item_index, object_placement_data *placement, bool *out_of_memory)
{
    item_datum *item = items_object_datum_get(item_index);
    items_flags_set(&item->object.object_flags, 12, true);
    item->toss_unit_index = -1;
    item->unit_object_index = -1;
    item->last_unit_object_index = -1;
    items_mark_relevance(item_index);
    item_adjust_scale(item_index, -1);
    return true;
}

static __forceinline bool items_object_flag_test(const item_datum *item, long bit)
{
    return ((item->object.object_flags >> bit) & 1) != 0;
}

// @address 0x35c1e8 ?item_maintain_z_up@@YAXJ@Z
void item_maintain_z_up(long item_index)
{
    item_datum *item = items_object_datum_get(item_index);
    unit_definition *definition = (unit_definition *)items_tag_definition_get(item->object.definition_index);
    if (item->object.parent_object_index == -1 && !items_object_flag_test(item, 7) && (definition->flags & 1) && !(fabsf(item->object.up.k - 1.0f) < 0.0001f)) {
        item->object.up = *global_up3d;
        real_vector3d left;
        cross_product3d(&item->object.up, &item->object.forward, &left);
        cross_product3d(&left, &item->object.up, &item->object.forward);
        if (normalize3d(&item->object.forward) == 0.0f) {
            item->object.forward = *global_forward3d;
        }
        object_set_position_direct(item_index, &item->object.position, &item->object.forward, &item->object.up, &item->object.location, false);
    }
}

// @address 0x35c360 item_adjust_scale_to_unit
void item_adjust_scale_to_unit(long unit_index, long unused_parameter, long item_index)
{
    item_adjust_scale(item_index, unit_index);
    return;
}

// @address 0x35c370 ?item_attach_to_node@@YAXJJF@Z
#if !defined(_XBOX) || defined(__clang__)
void item_attach_to_node(long parent_object_index, long item_index, short node_index)
{
    item_adjust_scale(item_index, parent_object_index);
}
#endif

// @address 0x35c380 ?item_detach_from_parent@@YAXJ@Z
#if !defined(_XBOX) || defined(__clang__)
void item_detach_from_parent(long item_index)
{
    item_adjust_scale(item_index, -1);
}
#endif

static __forceinline bool items_unit_does_not_show_readied_weapon(long unit_index)
{
    item_datum *unit = items_object_datum_get(unit_index);
    unit_definition *definition = (unit_definition *)items_tag_definition_get(unit->object.definition_index);
    return (bool)((definition->flags >> 5) & 1);
}

// @address 0x35ce88 ?item_unstow_inventory@@YAXJJJJ@Z
void item_unstow_inventory(long item_index, long parent_object_index, long parent_marker_name, long child_marker_name)
{
    item_datum *item = items_object_datum_get(item_index);
    if (item->object.parent_object_index != -1) {
        object_detach(item_index);
    }
    item->owner_relation = 1;
    if (parent_marker_name != 0) {
        object_attach_to_marker_with_type(parent_object_index, parent_marker_name, item_index, child_marker_name);
    }
    if (items_unit_does_not_show_readied_weapon(parent_object_index)) {
        ::object_set_hidden(item_index, true);
    }
}

// @rdata 0x820224f4
static const float items_reposition_cast_scale = 1.1f;
// @rdata 0x8202974c
static const float items_reposition_diagonal = 0.70710677f;
// @rdata 0x8202b10c
static const float items_reposition_minimum_radius = 0.15f;

static __forceinline bool items_gravity_item_flag_test(const item_datum *item, long bit)
{
    return ((item->inventory_flags >> bit) & 1) != 0;
}

// @address 0x35c768 item_update_position
extern "C" void item_update_position(long object_index)
{
    item_datum *item = items_gravity_item_get(object_index);
    bool reposition = items_gravity_item_flag_test(item, 3);
    if (reposition && ((item_datum *)item)->object.parent_object_index == -1 && item->object.havok_component_index != -1)
    {
        c_havok_component *component = &((c_havok_component *)g_havok_component_data->data)[datum_index_to_absolute_index(item->object.havok_component_index)];
        if (component->get_rigid_body_count() != 0)
        {
            const s_object_definition *definition = (const s_object_definition *)items_tag_definition_get(item->object.definition_index);
            real_matrix4x3 world_matrix;
            real_matrix4x3 best_matrix;
            bool found = false;
            long best_index = 0x80000000;
            object_get_world_matrix(object_index, &world_matrix);
            for (long orientation = 0; !found && orientation < 4; orientation++)
            {
                real_matrix4x3 matrix = world_matrix;
                switch (orientation)
                {
                case 0:
                    break;
                case 1:
                    matrix.rotation = global_identity4x3->rotation;
                    break;
                case 2:
                    matrix.up = world_matrix.left;
                    cross_product3d(&matrix.up, &matrix.forward, &matrix.left);
                    break;
                case 3:
                    matrix.up = world_matrix.forward;
                    cross_product3d(&matrix.left, &matrix.up, &matrix.forward);
                    break;
                default:
                    // match: 0x35c768: unreachable hint omits dead branch check
                    __assume(0);
                }
                for (long direction_index = 0; direction_index < 5; direction_index++)
                {
                    real_vector3d direction = *global_down3d;
                    if (direction_index > 0)
                    {
                        long axis = (direction_index - 1) >> 1;
                        float sign = ((direction_index - 1) & 1) == 0 ? 1.0f : -1.0f;
                        direction = *global_zero_vector3d;
                        direction.k = -0.70710677f;
                        direction.n[axis] = sign * items_reposition_diagonal;
                    }
                    float radius = definition->bounding_radius > items_reposition_minimum_radius ? definition->bounding_radius
                                                                                                 : items_reposition_minimum_radius;
                    real_vector3d cast_vector;
                    scale_vector3d(&direction, radius * items_reposition_cast_scale, &cast_vector);
                    real_matrix4x3 start_matrix = matrix;
                    start_matrix.position.x -= cast_vector.i;
                    start_matrix.position.y -= cast_vector.j;
                    start_matrix.position.z -= cast_vector.k;
                    long hit_index;
                    real_matrix4x3 hit_matrix;
                    if (havok_ray_cast(component, 0, 2, &start_matrix, &cast_vector, 3, &hit_index, &hit_matrix) && hit_index > best_index)
                    {
                        best_index = hit_index;
                        best_matrix = hit_matrix;
                        found = true;
                        break;
                    }
                }
            }
            if (found)
                object_set_position_direct(object_index, &best_matrix.position, &best_matrix.forward, &best_matrix.up, 0, false);
        }
        item->inventory_flags &= ~8;
    }
}

// @address 0x35cda0 ?item_hide_inventory@@YAXJ@Z
void item_hide_inventory(long item_index)
{
    item_datum *item = items_object_datum_get(item_index);
    if (item->object.parent_object_index != -1) {
        object_detach(item_index);
    }
    item->owner_relation = 2;
    ::object_set_hidden(item_index, true);
    object_set_in_limbo(item_index, true);
    object_activate(item_index);
}

// @address 0x35ce28 ?item_show_inventory_unattached@@YAXJJ@Z
void item_show_inventory_unattached(long item_index, long unit_index)
{
    item_datum *item = items_object_datum_get(item_index);
    item->owner_relation = 1;
    object_set_in_limbo(item_index, false);
    ::object_set_hidden(item_index, false);
}

// @address 0x10df30 ?item_move@@YAXJ@Z
void item_move(long item_index)
{
    object_in_early_mover_physics_move(item_index, 0);
}

union real_rgb_color;
struct effect_vector_field;
static __forceinline void items_object_get_damage_owner(long object_index, s_damage_owner *damage_owner)
{
    *damage_owner = items_object_datum_get(object_index)->object.damage_owner;
}

// @address 0x35cf48 ?item_detonate@@YAXJ@Z
void item_detonate(long item_index)
{
    item_datum *item = items_object_datum_get(item_index);
    const s_item_definition *definition = (const s_item_definition *)items_tag_definition_get(item->object.definition_index);
    if (item->remaining_count == 0) {
        float seconds = _real_random_range(tls_random_seed(), definition->detonation_time_lower, definition->detonation_time_upper);
        s_damage_owner damage_owner;
        items_object_get_damage_owner(item_index, &damage_owner);
        long effect_storage;
        c_tag_index *effect = new (&effect_storage) c_tag_index(definition->detonation_effect.index);
        effect_new_from_object(*effect, &damage_owner, 0, item_index, 0.0f, 0.0f, 0, 0, 1);
        item->remaining_count = (short)game_seconds_to_ticks_round(seconds);
    }
}

const float k_real_min = -3.4028234663852886e+38f;

static __forceinline bool item_update_game_is_predicted()
{
    bool result = false;
    if (game_globals->options.game_simulation == _game_simulation_distributed_client)
        result = true;
    return result;
}
static __forceinline bool havok_object_has_contact_points(long object_index)
{
    c_havok_contact_point_iterator iterator(object_index, false);
    return iterator.valid();
}
static __forceinline bool object_get_early_mover_velocity(
    long object_index, real_vector3d *linear_velocity, real_vector3d *angular_velocity, bool check_owner, bool primary)
{
    real_point3d center;
    object_get_center_of_mass(object_index, &center);
    return object_get_early_mover_local_space_velocity(object_index, &center, linear_velocity, angular_velocity, check_owner, primary);
}
static __forceinline float item_update_maximum_contact_normal(long item_index)
{
    float maximum = k_real_min;
    c_havok_contact_point_iterator iterator(item_index, false);
    for (; iterator.valid(); iterator.next()) {
        c_havok_contact_point *contact_point = iterator.get_contact_point();
        const real_vector3d *normal = contact_point->get_normal();
        if (normal->k > maximum)
            maximum = normal->k;
    }
    return maximum;
}
static __forceinline void item_update_damp_velocity(real_vector3d *velocity, float damping)
{
    damping = PIN(damping, 0.0f, 1.0f);
    scale_vector3d(velocity, 1.0f - damping, velocity);
}

// @address 0xeaa88 ?item_update@@YA_NJ@Z
bool item_update(long item_index)
{
    item_datum *item = (item_datum *)items_object_datum_get(item_index);
    s_item_definition *definition = (s_item_definition *)items_tag_definition_get(item->object.definition_index);
    bool result = false;
    if (items_object_flag_test(item, _object_connected_to_map_bit)) {
        item_update_position(item_index);
        if (havok_object_has_contact_points(item_index) ||
            item->object.m_physics_flags.test(_object_physics_in_water_bit) ||
            item->object.m_physics_flags.test(_object_physics_bit9))
            item->toss_unit_index = -1;
        object_in_early_mover_physics_update(item_index);
        if (item->remaining_count > 0) {
            item->remaining_count--;
            if (!item_update_game_is_predicted() && item->remaining_count == 0) {
                s_damage_owner damage_owner;
                items_object_get_damage_owner(item_index, &damage_owner);
                if (definition->detonation_damage_effect.index != -1) {
                    s_damage_data damage_data;
                    real_point3d origin;
                    object_get_origin(item_index, &origin);
                    damage_data_new(&damage_data, definition->detonation_damage_effect.index);
                    damage_data.reporting_info = 16;
                    damage_data.owner_data = damage_owner;
                    damage_data.scale[0] = 1.0f;
                    object_get_location(item_index, &damage_data.location);
                    damage_data.position = origin;
                    damage_data.origin = damage_data.position;
                    area_of_effect_cause_damage(&damage_data, -1, -1);
                }
                s_effect_vector vector;
                s_location location;
                object_get_origin(item_index, &vector.point);
                object_get_location(item_index, &location);
                long effect_storage;
                c_tag_index *effect = new (&effect_storage) c_tag_index(definition->detonating_effect.index);
                vector.vector = *global_up3d;
                vector.marker_name = 187;
                effect_new_with_damage_owner(*effect, &damage_owner, 16, 1, &vector, 0, &location, _effect_deterministic);
                object_delete(item_index);
            }
            result = true;
        }
    }
    if (!item->object.m_physics_flags.test(_object_physics_bit9))
        item->object.last_motion_time = TLS_GAME_TIME_GLOBALS->time;
    if (item->object.havok_component_index != -1 && !item->object.m_physics_flags.test(_object_physics_bit9)) {
        float angular_damping = definition->angular_damping;
        float linear_damping = definition->linear_damping;
        float threshold = g_item_contact_normal_threshold;
        if ((item->inventory_flags & 4) != 0) {
            angular_damping = MAX(angular_damping, 11.0f);
            linear_damping = MAX(linear_damping, 6.0f);
            threshold = 0.72f;
        }
        if (angular_damping != 0.0f || linear_damping != 0.0f) {
            float maximum = item_update_maximum_contact_normal(item_index);
            if (maximum > threshold) {
                float fraction = (maximum - threshold) / (1.0f - threshold);
                real_vector3d linear_velocity;
                real_vector3d angular_velocity;
                object_get_velocities(item_index, &linear_velocity, &angular_velocity);
                real_vector3d early_mover_linear_velocity;
                real_vector3d early_mover_angular_velocity;
                bool early_mover = object_get_early_mover_velocity(item_index, &early_mover_linear_velocity, &early_mover_angular_velocity, true, false);
                if (early_mover) {
                    subtract_vectors3d(&linear_velocity, &early_mover_linear_velocity, &linear_velocity);
                    subtract_vectors3d(&angular_velocity, &early_mover_angular_velocity, &angular_velocity);
                }
                if (angular_damping != 0.0f) {
                    float damping = TLS_GAME_TIME_GLOBALS->tick_length * fraction * angular_damping;
                    damping = PIN(damping, 0.0f, 1.0f);
                    scale_vector3d(&angular_velocity, 1.0f - damping, &angular_velocity);
                }
                if (linear_damping != 0.0f) {
                    item_update_damp_velocity(&linear_velocity, TLS_GAME_TIME_GLOBALS->tick_length * fraction * linear_damping);
                }
                if (early_mover) {
                    add_vectors3d(&linear_velocity, &early_mover_linear_velocity, &linear_velocity);
                    add_vectors3d(&angular_velocity, &early_mover_angular_velocity, &angular_velocity);
                }
                object_set_velocities(item_index, &linear_velocity, &angular_velocity);
            }
        }
    }
    return result;
}

static __forceinline unsigned char items_header_object_type_get(long object_index)
{
    return ((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(object_index)].object_type;
}

static __forceinline bool items_inventory_weapon_flag_test(const item_datum *item, long bit)
{
    return ((((const weapon_datum *)item)->flags >> bit) & 1) != 0;
}
static __forceinline bool items_inventory_variant_flag_test(const s_variant_object_datum *object, long bit)
{
    return ((object->flags.get_unsafe() >> bit) & 1) != 0;
}
static __forceinline void items_inventory_flags_set(unsigned char *flags, long bit, bool value)
{
    if (value)
        *flags |= 1 << bit;
    else
        *flags &= ~(1 << bit);
}
// @rdata 0x82029738
static const float items_inventory_extended_lifetime = 300.0f;
// @address 0x35c440 ?item_in_unit_inventory@@YAXJJ@Z
void item_in_unit_inventory(long item_index, long unit_index)
{
    item_datum *item = items_object_datum_get(item_index);
    if (unit_index != -1)
    {
        if ((bool)((item->object.object_flags >> 29) & 1))
            object_freeze_node_orientations(item_index, false);
        items_inventory_flags_set(&item->inventory_flags, 0, true);
        item->owner_relation = 1;
        item->unit_object_index = unit_index;
        item->last_unit_object_index = unit_index;
        object_set_hidden(item_index, false);
        object_set_garbage(item_index, false, -1);
        items_inventory_flags_set(&item->inventory_flags, 2, false);
        location_invalidate(&item->object.location);
        simulation_action_object_delete(item_index);
        if (((1 << items_header_object_type_get(item_index)) & 4) && items_inventory_weapon_flag_test(item, 6))
            game_engine_multiplayer_weapon_picked_up(item_index, unit_index);
    }
    else
    {
    long previous_unit_index = item->unit_object_index;
    if (game_is_campaign())
    {
        long garbage_time = game_seconds_to_ticks_round(30.0f);
        if ((item->inventory_flags & 2) && items_header_object_type_get(item_index) == 2 && weapon_can_be_fired(item_index) && !(item->inventory_flags & 16))
            garbage_time = game_seconds_to_ticks_round(items_inventory_extended_lifetime);
        object_set_garbage(item_index, true, garbage_time);
    }
    else if (item->object.scenario_datum_index == -1)
    {
        object_set_garbage(item_index, true, game_seconds_to_ticks_round(30.0f));
    }
    items_mark_relevance(item_index);
    items_inventory_flags_set(&item->inventory_flags, 1, false);
    item->owner_relation = 0;
    item->unit_object_index = -1;
    if (item->object.scenario_datum_index == -1 && item->object.map_variant_index != -1)
    {
        s_variant_object_datum variant;
        tls_game_engine_globals()->map_info.get_variant_object_datum(item->object.map_variant_index, &variant);
        if (items_inventory_variant_flag_test(&variant, _variant_object_placement_runtime_candidate_bit))
            item->object.map_variant_index = -1;
    }
    simulation_action_object_create(item_index);
    if (((1 << items_header_object_type_get(item_index)) & 4) && items_inventory_weapon_flag_test(item, 6))
    {
        long engine_index = tls_game_engine_globals()->game_engine_index;
        if (tls_game_engine_globals()->game_engine_index >= 1 &&
            tls_game_engine_globals()->game_engine_index <= 10 && game_engines[engine_index])
            game_engine_multiplayer_weapon_drop_internal(item_index, previous_unit_index);
    }
    }
    item_update_player_inventory(item_index);
}
