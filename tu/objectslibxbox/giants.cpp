// objectslibxbox/giants.obj: from the records 0x35e008 0x35e0e0 0x35e1a0 0x35e390 0x35e4a8 0x35e568 0x35e850 0x35e918 0x35ea48 0x35ec08 0x35edb8 0x35f1c0 0x35f5d0 0x35f948 0x35fb98 0x35fc00 0x35fcb0 0x35fde8 0x3600d0 0x3602e8 0x3607b0 0x360c08 0x360d68
// @flags /TP /O2 /Oc
#include "objects/object_header.h"
#include "objects/object_node_matrices_inline.h"
#include "tag_files/tag_groups.h"
#include "memory/datum_index.h"
#include "memory/data_inline.h"
#include "tag_files/string_ids.h"
#include "ai/sector_geometry.h"
#include "physics/collision_test_vector.h"
#include "math/real_math_constants.h"
#include "math/real_math_globals.h"
#include "animations/animation_definition_storage.h"
#include "physics/physics_constants_globals.h"
#include "objects/object_reference_types.h"
#include "units/giant_tuning.h"
#include "math/real_math.h"
#include "game/game_globals_storage.h"
#include "tag_files/tag_types.h"
#include "animations/animation_id.h"
#include "objects/object_placement.h"
#include "units/motor_enums.h"
#include "units/motor_program_state.h"
#include <float.h>
#include <math.h>
#include "math/matrix_math.h"
#include "ai/sector_ref.h"
#include "objects/objects.h"
#include "units/units.h"
#include "ai/ai_reference_frame.h"
#include "items/weapons.h"
#include "models/models.h"
#include "models/render_model_definitions.h"
#include "objects/object_early_movers.h"
#include "physics/havok.h"
#include "physics/havok_utilities.h"
#include "units/giants.h"
#include "objects/object_datum.h"
#include "animations/animation_interpolation.h"
#include "units/slider_movement_pattern_types.h"
#include "units/scenario_unit_datum_types.h"
#include "cache/cache_files.h"
#include "physics/havok_component.h"
#include "units/unit_animation.h"
#include "units/unit_types.h"
#include "memory/tls_globals.h"
#include "objects/object_definitions.h"
#include "models/model_types.h"
#include "physics/collision_types.h"
#include "units/slider_target_position_types.h"
#include "animations/mixing_board/mixing_board_definition.h"
#include "animations/mixing_board/mixing_board_slider_types.h"
#include "animations/mixing_board/animation_manager_mixing_board.h"
union real_point3d;
union real_vector3d;
// @address 0x35e0e0 ?giant_new@@YA_NJPAUobject_placement_data@@PA_N@Z
// @address 0x35e568 ?giant_clear_foot_tracking_data@@YAXJ@Z
// @address 0x35e850 ?giant_setup_foot_tracking_data@@YAXJ@Z
// @address 0x35fcb0 ?giant_preprocess_node_orientations@@YAXJPBV?$c_static_flags@$0PP@@@JPAUreal_orientation@@@Z

enum e_foot_tracking_state;

#include "units/giant_datum.h"

#include "memory/data_types.h"

#include "game/game_time_types.h"

struct s_giant_foot_tracking_member
{
    long foot_index;
    real_bounds foot_ik_range;
    long ankle_marker;
    real_bounds ankle_ik_range;
    short default_state;
};

#include "animations/animation_graph_node_types.h"


#include "animations/animation_manager.h"
#include "animations/animation_search_path_iterator.h"
template float t_short_fixed<13>::export_real() const;

extern "C" {
const float k_giant_elevation_delta_bias = 1.0f;
const float k_real_max = FLT_MAX;
const float k_real_min = -FLT_MAX;

// @address 0x119a464 g_giant_elevation_include_lowest_foot
unsigned char g_giant_elevation_include_lowest_foot;
}
struct s_data_array;
class c_havok_component;

#include "scenario/scenario_objects.h"

#include "units/giant_definitions.h"




#include "animations/animation_definition_types.h"



#include "physics/havok_component_inline.h"


namespace {
class c_buckling_map_iterator
{
public:
    __forceinline void setup(const s_buckling_map *map, long x, long y)
    {
        m_map = map;
        m_x = x;
        m_y = y;
    }
    __forceinline bool valid() const
    {
        return m_map && m_x >= 0 && m_x < m_map->width && m_y >= 0 && m_y < m_map->height;
    }
    __forceinline const real_point3d *get_point_const() const { return &m_map->points[m_y][m_x]; }
    __forceinline bool next()
    {
        bool result = valid();
        if (result)
        {
            m_x++;
            if (m_x >= m_map->width)
            {
                m_x = 0;
                m_y++;
                if (m_y >= m_map->height)
                    result = false;
            }
        }
        return result;
    }
private:
    const s_buckling_map *m_map;
    long m_x, m_y;
};
}






static __forceinline bool giants_physics_flag_test(const s_giant_datum *giant, long bit)
{
    return ((giant->object.physics_flags >> bit) & 1) != 0;
}

static __forceinline bool giants_unit_flag_test(const s_giant_datum *giant, long bit)
{
    return ((giant->unit.unit_flags >> bit) & 1) != 0;
}

static __forceinline bool giants_control_flag_test(const s_giant_datum *giant, long bit)
{
    return ((giant->unit.control_flags >> bit) & 1) != 0;
}

// 0x35e4a8: the giant's slider value as a real, 0 unless its object is up to date and its flag bit 2 is clear
// @address 0x35e4a8 giant_get_slider_value
extern "C" float giant_get_slider_value(long giant_index)
{
    float result = 0.0f;
    s_giant_datum *giant = OBJECT_GET(giant_index, s_giant_datum, 0x2000);
    if ((unsigned char)giant_is_animation_state_current(giant_index) != 0 && !flag_test(giant->giant_flags, 2L))
    {
        const c_animation_manager *animation = (const c_animation_manager *)((const char *)giant + giant->object.animation.offset);
        result = animation->get_mixing_board_slider(6)->get_value().export_real();
    }
    return result;
}

// @address 0x35e008 ?giant_place@@YAXJPAUs_scenario_giant@@@Z
void giant_place(long giant_index, s_scenario_giant *scenario_giant)
{
    unit_place(giant_index, &scenario_giant->unit);
    giant_setup_foot_tracking_data(giant_index);
    s_giant_datum *giant = OBJECT_GET(giant_index, s_giant_datum, 1 << 13);
    short policy;
    switch (scenario_giant->pathfinding_policy)
    {
    case 0:
    case 1:
    case 2:
    case 4:
        policy = 3;
        break;
    case 3:
        policy = 1;
        break;
    }
    if (policy == 1)
    {
        giant->object.object_flags |= 1 << 22;
        object_refresh_reference_frames(giant_index);
    }
    object_update_collision_culling(giant_index);
}

// 0x35e0e0: the giant's construction: motor program set, identity matrix copied, foot tracking set up, center found
bool giant_new(long giant_index, struct object_placement_data *placement, bool *out_of_memory)
{
    s_giant_datum *giant = OBJECT_GET(giant_index, s_giant_datum, 0x2000);
    c_motor_program_state *state = (c_motor_program_state *)((char *)giant + giant->unit.motor_program.offset);
    state->set_program(giant_index, _motor_program_effect_scenery);
    giant->spare_init_zero = 0;
    giant->identity_copy = k_real_math_identity_matrix_storage;
    giant_setup_foot_tracking_data(giant_index);
    object_get_center_of_mass(giant_index, &giant->pathfinding_point);
    giant->pathfinding_time = -1;
    giant->pathfinding_structure_index = -1;
    giant->pathfinding_surface_index = -1;
    giant->pathfinding_object_index = -1;
    giant->pathfinding_sector.clear();
    giant->pathfinding_flags = 0xffffffff;
    return true;
}

// @rdata 0x820221fc
static const float k_giant_active_interpolation_time = 1.0f;
// @address 0x35e1a0 ?giant_update@@YA_NJ@Z
bool giant_update(long giant_index)
{
    s_giant_datum *giant = OBJECT_GET(giant_index, s_giant_datum, 0x2000);
    do
    {
        if (*(long *)((char *)giant + giant->unit.motor_program.offset) != 11)
        {
            if (flag_test(giant->giant_flags, 5L))
            {
                unsigned long *giant_flags = &giant->giant_flags;
                *giant_flags &= ~32UL;
                object_start_interpolation(giant_index, giant->active_interpolation_time);
            }
            giant->active_interpolation_time -= TLS_GAME_TIME_GLOBALS->tick_length;
            if (giant->active_interpolation_time < 0.0f)
                giant->active_interpolation_time = 0.0f;
            if (giant->active_interpolation_time > 0.0f)
                break;
            if (giants_physics_flag_test(giant, 13))
            {
                long havok_component_index = giant->object.havok_component_index;
                if (havok_component_index != -1)
                    havok_component_update_phantom_state(objects_havok_component_get(havok_component_index), (bool)((giant->object.physics_flags >> 12) & 1));
            }
            s_giant_datum *object = OBJECT_GET(giant_index, s_giant_datum, 0x2000);
            if (giants_control_flag_test(object, 0))
            {
                if (!giants_unit_flag_test(object, 22))
                {
                    unsigned long *object_flags = &object->giant_flags;
                    *object_flags |= 1;
                    object->giant_flags |= 4;
                    unsigned long *object_unit_flags = &object->unit.unit_flags;
                    *object_unit_flags |= 0x400000;
                    break;
                }
                if ((object->unit.control_flags & 1) != 0)
                    break;
            }
            if ((bool)(_rotl(object->unit.unit_flags, 10) & 1))
            {
                unsigned long *object_flags = &object->giant_flags;
                *object_flags &= ~1UL;
                object->giant_flags |= 4;
                unsigned long *object_unit_flags = &object->unit.unit_flags;
                *object_unit_flags &= ~0x400000UL;
            }
        }
        else
        {
            giant->active_interpolation_time = k_giant_active_interpolation_time;
            if (!flag_test(giant->giant_flags, 5L))
            {
                unsigned long *giant_flags = &giant->giant_flags;
                *giant_flags |= 16;
                giant->giant_flags |= 32;
            }
        }
    } while (false);
    giant->pathfinding_structure_index = -1;
    giant->pathfinding_surface_index = -1;
    giant->pathfinding_instanced_geometry_index = -1;
    giant->pathfinding_object_index = -1;
    giant->pathfinding_flags = -1;
    giant->pathfinding_sector.clear();
    object_get_center_of_mass(giant_index, &giant->pathfinding_point);
    return true;
}

// @address 0x35e390 giant_compute_function_value
extern "C" bool giant_compute_function_value(long giant_index, long function_name, long function_instance, float *value, bool *active, bool *deterministic)
{
    float function_value = 0.0f;
    bool result = true;
    switch (function_name)
    {
    case _string_id_buckle:
        function_value = giant_get_slider_value(giant_index);
        break;
    default:
        result = false;
        break;
    }
    if (result)
    {
        function_value = PIN(function_value, 0.0f, 1.0f);
        *value = function_value;
        *active = function_value > 0.0f;
    }
    else
    {
        s_giant_datum *giant = OBJECT_GET(giant_index, s_giant_datum, 0x2000);
        signed char index = giant->unit.current_weapon_index;
        if (index != -1)
        {
            long tag_index = giant->unit.weapon_object_indices[index];
            if (tag_index != -1)
                result = weapon_compute_function_value(tag_index, function_name, function_instance, value, active, deterministic);
        }
    }
    return result;
}

void giant_clear_foot_tracking_data(long giant_index)
{
    s_giant_datum *giant = OBJECT_GET(giant_index, s_giant_datum, 0x2000);

    for (long foot_index = 0; foot_index < 4; foot_index++)
    {
        s_giant_foot *foot = &giant->feet[foot_index];

        foot->marker = -1;
        foot->ground = *global_origin3d;
        foot->future = *global_origin3d;
        foot->ray_origin = *global_origin3d;
        foot->ray_direction = *global_zero_vector3d;
        foot->ground = *global_origin3d;
        foot->weight = 0.0f;
        foot->state = _foot_tracking_none;
        foot->flags.clear();
    }
}

void giant_setup_foot_tracking_data(long giant_index)
{
    s_giant_datum *giant = OBJECT_GET(giant_index, s_giant_datum, 0x2000);
    const c_animation_manager *animation = (const c_animation_manager *)((const char *)giant + giant->object.animation.offset);
    const c_model_animation_graph *graph = (const c_model_animation_graph *)tag_get('jmad', animation->m_graph_tag_index);

    giant_clear_foot_tracking_data(giant_index);
    giant->foot_count = graph->m_foot_tracking.count;
    for (long foot_index = 0; foot_index < giant->foot_count; foot_index++)
    {
        s_giant_foot *foot = &giant->feet[foot_index];

        foot->marker = TAG_BLOCK_GET_ELEMENT(&graph->m_foot_tracking, foot_index, const s_giant_foot_tracking_member)->foot_index;
        foot->state = _foot_tracking_none;
        foot->weight = 0.0f;
    }
}

// 0x35e918: the giant's elevation moves toward the average of its flagged feet's elevations, scaled by the factor
// @address 0x35e918 giant_update_elevation
extern "C" void giant_update_elevation(long object_index, float factor)
{
    s_giant_datum *giant = OBJECT_GET(object_index, s_giant_datum, 0x2000);
    float elevation = giant->object.position.z;
    long count = 0;
    float elevation_delta = 0.0f;
    float maximum = k_real_min;
    float minimum = k_real_max;
    for (long i = 0; i < giant->foot_count; ++i)
    {
        const s_giant_foot *foot = &giant->feet[i];
        if (foot->flags.test(0))
        {
            elevation_delta += foot->ground.z - elevation;
            ++count;
            maximum = maximum > foot->ground.z ? maximum : foot->ground.z;
            minimum = minimum > foot->ground.z ? foot->ground.z : minimum;
        }
    }
    if (maximum != k_real_min)
    {
        float delta = maximum - elevation;
        elevation_delta += delta;
        ++count;
    }
    if (g_giant_elevation_include_lowest_foot && minimum != k_real_max)
    {
        float delta = elevation - minimum;
        elevation_delta += delta;
        ++count;
    }
    if (count > 0)
    {
        elevation_delta += k_giant_elevation_delta_bias;
        float average_delta = elevation_delta / (float)count;
        const s_giant_definition *definition = (const s_giant_definition *)tag_get('gint', giant->object.definition_index);
        giant->object.position.z += average_delta * definition->elevation_scale * factor;
    }
}

class c_model_animation;

// @address 0x35ea48 ?giant_update_foot_tracking@@YAXJ@Z
void giant_update_foot_tracking(long giant_index)
{
    s_giant_datum *giant = OBJECT_GET(giant_index, s_giant_datum, 0x2000);
    const c_animation_manager *manager = (const c_animation_manager *)((const char *)giant + giant->object.animation.offset);
    for (long foot_index = 0; foot_index < giant->foot_count; foot_index++)
    {
        s_giant_foot *foot = &giant->feet[foot_index];
        if (foot->marker != -1)
        {
            long old_state = foot->state.get();
            if (!((const c_animation_manager *)manager)->get_current_foot_tracking_state(foot_index,
                &foot->state, &foot->weight, &foot->frame))
            {
                const c_model_animation_graph *graph =
                    (const c_model_animation_graph *)tag_get('jmad', manager->m_graph_tag_index);
                foot->state = graph->find_foot_tracking_default_state(foot_index, manager->m_current_goal.mode);
                foot->weight = 1.0f;
                foot->frame = (short)(long)manager->m_state_channel.m_frame_position;
            }
            if (foot->state.get() != old_state && foot->state.get() == _foot_tracking_plant)
                foot->flags.set(0, false);
            if (old_state == _foot_tracking_none && foot->state.get() == _foot_tracking_on)
                foot->flags.set(0, false);
            if (!TEST_BIT(foot->flags.get_unsafe(), 0))
            {
                bool success = false;
                real_matrix4x3 matrix;
                if (giant_solve_future_marker_position(giant_index, foot->marker, foot->frame, &matrix))
                {
                    foot->future = matrix.position;
                    success = ::giant_find_ground_point(giant_index, &matrix.position,
                        &foot->ray_origin, &foot->ray_direction,
                        &foot->ground);
                }
                if (success)
                    foot->flags.set(0, true);
            }
        }
    }
}

// 0x35ec08: the ground point under a foot: a collision test straight down from above it, pinned to the giant's ground range
// @address 0x35ec08 ?giant_find_ground_point@@YA_NJPBTreal_point3d@@PAT1@PATreal_vector3d@@1@Z
bool giant_find_ground_point(long giant_index, union real_point3d const *foot_point, union real_point3d *start_out,
    union real_vector3d *vector_out, union real_point3d *ground_out)
{
    bool result = false;
    s_giant_datum *giant = OBJECT_GET(giant_index, s_giant_datum, 0x2000);
    const s_giant_definition *definition = (const s_giant_definition *)tag_get('gint', giant->object.definition_index);
    float height = definition->ground_range * 2.0f;
    real_point3d start;
    start.x = foot_point->x;
    start.y = foot_point->y;
    start.z = foot_point->z + height;
    real_vector3d vector;
    vector.i = 0.0f;
    vector.j = 0.0f;
    vector.k = -(height * 2.0f);
    if (start_out)
        *start_out = start;
    if (vector_out)
        *vector_out = vector;
    *ground_out = *foot_point;
    collision_result collision;
    if (collision_test_vector(k_collision_test_structure, 0, &start, &vector, giant_index, -1, -1, &collision))
    {
        float maximum = giant->object.position.z + definition->ground_range;
        float minimum = giant->object.position.z - definition->ground_range;
        *ground_out = collision.point;
        ground_out->z = PIN(ground_out->z, minimum, maximum);
        result = true;
    }
    return result;
}



static __forceinline short giants_game_difficulty_level_get(void)
{
    return game_globals->options.game_mode == 1 ? game_globals->options.campaign_difficulty_level : 1;
}

// 0x35f948: the giant's variant scale: by difficulty while its state is 2, or its mixing board slider's move while it is 3
// @address 0x35f948 giant_update_variant_scale
extern "C" void giant_update_variant_scale(long object_index)
{
    s_giant_datum *giant = OBJECT_GET(object_index, s_giant_datum, 0x2000);
    bool active = (unsigned char)giant_is_animation_state_current(object_index) && giant->variant_index == 0;
    if (!active)
    {
        unsigned long *flags = &giant->giant_flags;
        *flags |= 1;
        giant->giant_flags &= ~4UL;
        return;
    }
    const s_giant_definition *definition = (const s_giant_definition *)tag_get(0, giant->object.definition_index);
    // match: 0x35f948: tag definition is non-null, folding the variant check below
    __assume(definition != 0);
    const s_giant_variant *variant = &definition->variants[giant->variant_index];
    if (!variant)
        return;
    if (giant->buckling_state == k_giant_buckle_state_buckled)
    {
        short index = PIN(giants_game_difficulty_level_get(), 0, 3);
        giant->buckle_timer = variant->difficulty_scales[index];
    }
    else if (giant->buckling_state == k_giant_buckle_state_ascending)
    {
        const c_animation_manager *animation = (const c_animation_manager *)((const char *)giant + giant->object.animation.offset);
        c_mixing_board_slider *slider = animation->get_mixing_board_slider(6);
        giant->buckle_position = slider->get_value().export_real();
        giant->buckling_state = k_giant_buckle_state_descending;
        float *velocity = &giant->buckle_acceleration;
        *velocity = 0.0f;
        bool settled = (float)fabs(variant->settle_scale) >= k_real_epsilon;
        if (!settled)
            slider->move_to_target(
                    _slider_target_position_full, (e_slider_movement_pattern)variant->level, giant->buckle_position * variant->volume_scale);
    }
}

// 0x35fb98: the giant's animation state is its variant's value, else the giant's flag bit 0
// @address 0x35fb98 giant_is_animation_state_current
extern "C" long giant_is_animation_state_current(long giant_index)
{
    s_giant_datum *giant = OBJECT_GET(giant_index, s_giant_datum, 0x2000);
    // match: 0x35fb98: volatile read preserves memory access order
    long flag = static_cast<volatile unsigned long &>(giant->giant_flags) & 1;
    long value = giant_get_variant_animation_state(giant_index);
    const c_animation_manager *animation = (const c_animation_manager *)((const char *)giant + giant->object.animation.offset);
    return (animation->m_current_goal.state == value) ? 1 : flag;
}

class c_animation_manager;

// 0x35fc00: the giant's animation setup: unit setup, then its channel and slider linked to their parent sliders
// @address 0x35fc00 ?giant_animation_setup_mixing_board@@YAXPAVc_animation_manager@@@Z
void giant_animation_setup_mixing_board(c_animation_manager *animation_manager)
{
    unit_animation_setup_mixing_board(animation_manager);
    c_animation_channel *channel = animation_manager->get_mixing_board_animation_channel(3);
    channel->m_parent_slider_type = _mixing_board_parent_slider_regular;
    channel->m_parent_slider_index = 7;
    c_mixing_board_slider *slider = animation_manager->get_mixing_board_slider(7);
    slider->m_parent_slider_type = _mixing_board_parent_slider_regular;
    slider->m_parent_slider_index = 8;
}

// 0x35fcb0: the giant's node orientation, blended from identity by its mixing board slider and multiplied in
void giant_preprocess_node_orientations(long giant_index, const c_static_flags<255> *node_flags, long node_count, real_orientation *orientations)
{
    s_giant_datum *giant = OBJECT_GET(giant_index, s_giant_datum, 0x2000);
    if (*(long *)((char *)giant + giant->unit.motor_program.offset) != 11 && flag_test(giant->giant_flags, 1L))
    {
        const c_animation_manager *animation = (const c_animation_manager *)((const char *)giant + giant->object.animation.offset);
        if (animation->m_current_goal.state == giant_get_variant_animation_state(giant_index) &&
            giant->node_index >= 0 && giant->node_index < node_count &&
            (!node_flags || node_flags->test(giant->node_index)))
        {
            real_quaternion rotation;
            quaternions_interpolate(&k_real_math_identity_orientation_storage.rotation, &giant->rotation,
                animation->get_mixing_board_slider(6)->get_value().export_real(), &rotation);
            quaternions_multiply(&orientations[giant->node_index].rotation, &rotation, &orientations[giant->node_index].rotation);
        }
    }
}

// @address 0x3600d0 ?compute_score_of_buckle_angles@@YAMPIBUs_buckling_map@@MMPBUreal_matrix4x3@@PAU2@@Z
// match: 0x3600d0: pointer __restrict qualifier prevents alias reload
float compute_score_of_buckle_angles(const s_buckling_map *__restrict map, float pitch, float roll,
    const real_matrix4x3 *max_depth_matrix, real_matrix4x3 *output_world_matrix)
{
    float score = -3.402823466e+38f;
    real_matrix4x3 rotation;
    matrix4x3_rotation_from_angles(&rotation, 0.0f, pitch, roll);
    real_matrix4x3 world;
    matrix4x3_compute_product(max_depth_matrix, &rotation, &world);
    real_vector3d normal = world.up;
    real_point3d origin = max_depth_matrix->position;
    if (normal.k > 0.0f)
    {
        float plane_distance = -(origin.x * normal.i) - origin.y * normal.j - origin.z * normal.k;
        float reciprocal = 1.0f / normal.k;
        score = 0.0f;
        long point_index = 0;
        c_buckling_map_iterator iterator;
        iterator.setup(map, 0, 0);
        while (iterator.valid())
        {
            const real_point3d *point = iterator.get_point_const();
            float x_dot = point->x * normal.i;
            float y_dot = point->y * normal.j;
            float numerator = x_dot + y_dot + plane_distance;
            float plane_z = -(numerator * reciprocal);
            float depth = point->z - plane_z;
            if (depth > 0.0f)
            {
                origin.z += depth;
                world.position.z += depth;
                plane_distance = -(origin.x * normal.i) - origin.y * normal.j - origin.z * normal.k;
                score += -depth * point_index;
            }
            else
                score += depth;
            point_index++;
            iterator.next();
        }
    }
    *output_world_matrix = world;
    return score;
}
// 0x360c08: the giant's variant value, 845 when its variant has none
// @address 0x360c08 giant_get_variant_animation_state
extern "C" long giant_get_variant_animation_state(long giant_index)
{
    long value = 845;
    s_giant_datum *giant = OBJECT_GET(giant_index, s_giant_datum, 0x2000);
    const s_giant_definition *definition = (const s_giant_definition *)tag_get(0, giant->object.definition_index);
    // match: 0x360c08: tag definition is non-null, folding the variant check below
    __assume(definition != 0);
    const s_giant_variant *variant = &definition->variants[giant->variant_index];
    if (variant && variant->value != -1)
        value = variant->value;
    return value;
}

static __forceinline real_point3d *giants_buckling_transform_point(const real_matrix4x3 *matrix, const real_point3d *point, real_point3d *result)
{
    float forward_j = matrix->forward.j;
    float up_j = matrix->up.j;
    float scaled_x = point->x * matrix->scale;
    float y = point->y;
    y *= matrix->scale;
    float z = point->z;
    z *= matrix->scale;
    result->x = scaled_x * matrix->forward.i + y * matrix->left.i + z * matrix->up.i + matrix->position.x;
    result->y = (scaled_x * forward_j + z * up_j) + y * matrix->left.j + matrix->position.y;
    result->z = (y * matrix->left.k + z * matrix->up.k) + scaled_x * matrix->forward.k + matrix->position.z;
    return result;
}

static __forceinline float giants_buckling_distance_squared2d(const real_point3d *a, const real_point3d *b)
{
    float dx = b->x - a->x;
    float dy = b->y - a->y;
    return dx * dx + dy * dy;
}

// @address 0x35fde8 ?setup_buckling_globals@@YAXJJJPBUreal_matrix4x3@@MMMM@Z
void setup_buckling_globals(long giant_index, long x_step_count, long y_step_count, const real_matrix4x3 *world_center_matrix,
    float x_start, float y_start, float x_step, float y_step)
{
    s_giant_datum *giant = OBJECT_GET(giant_index, s_giant_datum, 0x2000);
    giant->buckling_map.width = (short)x_step_count;
    giant->buckling_map.height = (short)y_step_count;
    memcpy(&giant->buckling_center, world_center_matrix, sizeof(real_matrix4x3));
    giant->highest_ground = world_center_matrix->position;
    giant->lowest_ground = world_center_matrix->position;
    real_point3d ground;
    if (::giant_find_ground_point(giant_index, &world_center_matrix->position, 0,
            0, &ground))
    {
        giant->buckling_center.position.z = ground.z;
        giant->highest_ground = ground;
        giant->lowest_ground = ground;
    }
    for (long x = 0; x < x_step_count; ++x)
    {
        for (long y = 0; y < y_step_count; ++y)
        {
            real_point3d local_point;
            local_point.x = x_start + (float)x * x_step;
            local_point.y = y_start + (float)y * y_step;
            local_point.z = 0.0f;
            real_point3d world_point;
            giants_buckling_transform_point(world_center_matrix, &local_point, &world_point);
            real_point3d *point = &giant->buckling_map.points[y][x];
            *point = world_point;
            if (::giant_find_ground_point(giant_index, &world_point, 0,
                    0, &ground))
                *point = ground;
            if (point->z > giant->highest_ground.z)
                giant->highest_ground = *point;
            else if (point->z == giant->highest_ground.z)
            {
                float highest_distance = giants_buckling_distance_squared2d(&giant->buckling_center.position, &giant->highest_ground);
                float point_distance = giants_buckling_distance_squared2d(&giant->buckling_center.position, point);
                if (point_distance < highest_distance)
                    giant->highest_ground = *point;
            }
            if (point->z < giant->lowest_ground.z)
            {
                giant->lowest_ground = *point;
                giant->buckling_center.position.z = point->z;
            }
        }
    }
}
struct render_model_definition;

// 0x35f5d0: the giant's node matrices, adjusted for foot IK when its feet are planted
// @address 0x35f5d0 ?giant_adjust_node_matrices@@YAXJJPAUreal_matrix4x3@@@Z
void giant_adjust_node_matrices(long giant_index, long node_index, real_matrix4x3 *node_matrices)
{
    s_giant_datum *giant = OBJECT_GET(giant_index, s_giant_datum, 0x2000);
    const s_giant_definition *definition = (const s_giant_definition *)tag_get(0, giant->object.definition_index);
    float ik_scale = definition->ik_scale > 0.0f ? definition->ik_scale : 1.0f;
    ik_scale = MIN(ik_scale, 1.0f);
    if (*(long *)((char *)giant + giant->unit.motor_program.offset) != 11)
    {
        c_animation_manager *manager = (c_animation_manager *)((char *)giant + giant->object.animation.offset);
        const c_model_animation_graph *graph = (const c_model_animation_graph *)tag_get(0, manager->m_graph_tag_index);
        float elevation_ratio = 1.0f;
        const c_interpolator_control *elevation_control = &manager->m_interpolator;
        if (giant->active_interpolation_time > 0.0f && elevation_control->enabled())
            elevation_ratio = elevation_control->get_ratio();
        giant_update_foot_tracking(giant_index);
        giant_update_elevation(giant_index, elevation_ratio);
        for (long foot_index = 0; foot_index < giant->foot_count; foot_index++)
        {
            s_giant_foot *foot = &giant->feet[foot_index];
            object_marker foot_marker;
            object_marker ankle_marker;
            if (foot->state.get() != 0 && foot->flags.test(0))
            {
                const s_giant_foot_tracking_member *member = TAG_BLOCK_GET_ELEMENT(&graph->m_foot_tracking, foot_index, const s_giant_foot_tracking_member);
                if (member->foot_index != -1)
                {
                    if (internal_object_get_markers_by_string_id(giant_index, member->foot_index, &foot_marker, 1, false))
                    {
                        const animation_graph_node *node = &((const animation_graph_node *)graph->m_nodes.address)[foot_marker.node_index];
                        if (node->parent_node_index != -1)
                        {
                            long foot_node_index = foot_marker.node_index;
                            long parent_node_index = node->parent_node_index;
                            float parent_distance = distance3d(&node_matrices[parent_node_index].position, &node_matrices[foot_node_index].position);
                            float marker_distance = distance3d(&node_matrices[foot_node_index].position, &foot_marker.matrix.position);
                            float minimum_length = fabsf(marker_distance - parent_distance);
                            float maximum_length = marker_distance + parent_distance;
                            float range = fabsf(maximum_length - minimum_length);
                            minimum_length += range * 0.25f;
                            maximum_length -= range * 0.2f;
                            if (member->ankle_marker != -1)
                            {
                                if (internal_object_get_markers_by_string_id(giant_index, member->ankle_marker, &ankle_marker, 1, false))
                                {
                                    real_vector3d direction;
                                    vector_from_points3d(&foot->ground, &ankle_marker.matrix.position, &direction);
                                    direction.k = MAX(direction.k, minimum_length);
                                    float length = normalize3d(&direction) * ik_scale;
                                    length = PIN(length, minimum_length, maximum_length);
                                    real_point3d ankle_point;
                                    point_from_line3d(&foot->ground, &direction, length, &ankle_point);
                                    giant_ik_foot_chain(giant_index, &ankle_point, foot->weight *elevation_ratio, member->ankle_marker, -100.0f,
                                        100.0f, node_index, node_matrices);
                                }
                            }
                            giant_ik_foot_chain(giant_index, &foot->ground, foot->weight *elevation_ratio, member->foot_index, -100.0f,
                                100.0f, node_index, node_matrices);
                        }
                    }
                }
            }
        }
    }
}

struct s_objects_element_array;

static __forceinline real_vector3d *giants_ik_cross_product(const real_vector3d *a, const real_vector3d *b, real_vector3d *result)
{
    result->i = a->j * b->k - a->k * b->j;
    result->j = a->k * b->i - a->i * b->k;
    result->k = a->i * b->j - a->j * b->i;
    return result;
}

// @address 0x35f1c0 ?giant_ik_foot_chain@@YA_NJPATreal_point3d@@MJMMJPAUreal_matrix4x3@@@Z
bool giant_ik_foot_chain(long giant_index, real_point3d *target_point, float weight, long marker_name, float minimum, float maximum,
    long node_count, real_matrix4x3 *node_matrices)
{
    bool result = false;
    s_giant_datum *giant = OBJECT_GET(giant_index, s_giant_datum, 0x2000);
    const s_model_definition *model = (const s_model_definition *)tag_get(0, 
        ((const s_object_definition *)global_tag_instances[datum_index_to_absolute_index(giant->object.definition_index)].definition)->model.index);
    const c_model_animation_graph *graph = (const c_model_animation_graph *)tag_get(0, 
        ((const c_animation_manager *)((const char *)giant + giant->object.animation.offset))->m_graph_tag_index);
    object_marker marker;
    if (internal_object_get_markers_by_string_id(giant_index, marker_name, &marker, 1, false) != 0)
    {
        const animation_graph_node *node = &((const animation_graph_node *)graph->m_nodes.address)[marker.node_index];
        if (node->parent_node_index != -1)
        {
            long joint_index = marker.node_index;
            long root_index = node->parent_node_index;
            real_matrix4x3 inverse_joint;
            matrix4x3_inverse(&node_matrices[joint_index], &inverse_joint);
            real_point3d foot = marker.matrix.position;
            real_vector3d reach_direction;
            vector_from_points3d(&node_matrices[root_index].position, &foot, &reach_direction);
            normalize3d(&reach_direction);
            real_vector3d forward;
            vector_from_points3d(&node_matrices[joint_index].position, &foot, &forward);
            normalize3d(&forward);
            real_vector3d root_forward;
            vector_from_points3d(&node_matrices[root_index].position, &node_matrices[joint_index].position, &root_forward);
            normalize3d(&root_forward);
            real_vector3d up;
            giants_ik_cross_product(&reach_direction, &root_forward, &up);
            normalize3d(&up);
            real_matrix4x3 end_matrix;
            matrix4x3_from_point_and_nonaxial_vectors(&end_matrix, &marker.matrix.position, &forward, &up);
            real_vector3d displacement;
            vector_from_points3d(&foot, target_point, &displacement);
            displacement.k = PIN(displacement.k, -100.0f, 100.0f);
            real_point3d adjusted_point;
            point_from_line3d(&foot, &displacement, weight, &adjusted_point);
            if (weight < 1.0f)
            {
                real_point3d ground_point;
                if (giant_find_ground_point(giant_index, &adjusted_point, 0, 0, &ground_point) && adjusted_point.z < ground_point.z)
                    adjusted_point.z = ground_point.z;
            }
            real_matrix4x3 target_matrix;
            matrix4x3_from_point_and_nonaxial_vectors(&target_matrix, &adjusted_point, &forward, &up);
            real_matrix4x3 joint_matrix;
            real_matrix4x3 root_matrix;
            matrix4x3_from_point_and_nonaxial_vectors(&root_matrix, &node_matrices[root_index].position, &root_forward, &up);
            matrix4x3_from_point_and_nonaxial_vectors(&joint_matrix, &node_matrices[joint_index].position, &forward, &up);
            real_matrix4x3 inverse_root;
            matrix4x3_inverse(&root_matrix, &inverse_root);
            real_matrix4x3 inverse_basis;
            matrix4x3_inverse(&joint_matrix, &inverse_basis);
            real_matrix4x3 root_offset;
            matrix4x3_compute_product(&inverse_root, &node_matrices[root_index], &root_offset);
            real_matrix4x3 joint_offset;
            matrix4x3_compute_product(&inverse_basis, &node_matrices[joint_index], &joint_offset);
            matrix4x3_inverse_kinematics_adjust_matrices_new(&target_matrix, &root_matrix, &joint_matrix, &end_matrix);
            matrix4x3_compute_product(&root_matrix, &root_offset, &node_matrices[root_index]);
            matrix4x3_compute_product(&joint_matrix, &joint_offset, &node_matrices[joint_index]);
            real_matrix4x3 delta_matrix;
            matrix4x3_compute_product(&node_matrices[joint_index], &inverse_joint, &delta_matrix);
            real_matrix4x3 delta_basis;
            memcpy(&delta_basis, &delta_matrix, sizeof(delta_basis));
            matrix4x3_from_point_and_nonaxial_vectors(&delta_matrix, &delta_basis.position, &delta_basis.forward, &delta_basis.up);
            if (internal_object_get_markers_by_string_id(giant_index, marker_name, &marker, 1, false) != 0)
            {
                float dx = marker.matrix.position.x - adjusted_point.x;
                float dy = marker.matrix.position.y - adjusted_point.y;
                float dz = marker.matrix.position.z - adjusted_point.z;
                if (dx * dx + (dz * dz + dy * dy) < 0.01f)
                    result = true;
            }
            const render_model_definition *render_model = (const render_model_definition *)tag_get(0, model->render_model_index);
            model_adjust_node_matrices_children_recursive(render_model, &delta_matrix, joint_index, node_count, node_matrices);
        }
    }
    return result;
}
// @address 0x806580 g_giant_scratch_orientations
#if !defined(_XBOX) || defined(__clang__)
extern "C" {
real_orientation g_giant_scratch_orientations[255];
}
#endif
// @rdata 0x8202490c
static const float giants_motion_epsilon = 0.0001f;
static __forceinline void giants_matrix_from_orientation(real_matrix4x3 *matrix, const real_orientation *orientation)
{
    matrix3x3_rotation_from_quaternion(&matrix->rotation, &orientation->rotation);
    matrix->position = orientation->translation;
    matrix->scale = orientation->scale;
}
static __forceinline const c_model_animation *giants_channel_animation(const c_animation_channel *channel)
{
    const c_model_animation *animation = 0;
    if (channel->m_animation_id.valid())
        animation = ((const c_model_animation_graph *)tag_get(0, channel->m_graph_index))->get_animation(channel->m_animation_id);
    return animation;
}

// @address 0x35edb8 ?giant_solve_future_marker_position@@YA_NJJJPAUreal_matrix4x3@@@Z
bool giant_solve_future_marker_position(long giant_index, long marker_index, long frame, real_matrix4x3 *matrix)
{
    bool result = false;
    object_marker marker;
    if (internal_object_get_markers_by_string_id(giant_index, marker_index, &marker, 1, false))
    {
        s_giant_datum *giant = OBJECT_GET(giant_index, s_giant_datum, 0x2000);
        memcpy(matrix, &marker.matrix, sizeof(*matrix));
        const c_animation_channel *channel = (const c_animation_channel *)((const char *)giant + giant->object.animation.offset);
        if (channel->valid())
        {
            long current_frame = (long)channel->m_frame_position;
            if (frame >= current_frame)
            {
                const c_model_animation *animation = giants_channel_animation(channel);
                long total_frames = animation->frame_count;
                frame = PIN(frame, 0, total_frames);
                float ratio = animation->get_playback_ratio_from_frame_position((float)frame);
                const s_object_definition *object_definition = (const s_object_definition *)tag_get(0, giant->object.definition_index);
                const s_model_definition *model = (const s_model_definition *)tag_get(0, object_definition->model.index);
                if (model->render_model_index != -1)
                {
                const render_model_definition *render_model = (const render_model_definition *)tag_get(0, model->render_model_index);
                real_orientation *orientations = g_giant_scratch_orientations;
                object_initialize_node_orientations((s_objects_element_array *)render_model, render_model->nodes.count, (char (*)[32])orientations);
                channel->apply_weighted_node_orientations_at_playback_ratio(
                    render_model, ratio, 1.0f, 0, render_model->nodes.count, orientations, 0, giant_index);
                real_matrix4x3 world;
                object_get_world_matrix(giant_index, &world);
                long frame_count = frame - current_frame;
                if (frame_count < 0)
                    frame_count = total_frames - current_frame + frame;
                long sample_frame = current_frame;
                for (long frame_offset = 0; frame_offset < frame_count; ++frame_offset)
                {
                    if (++sample_frame >= total_frames)
                        sample_frame = 0;
                    real_point3d translation;
                    float yaw;
                    channel->get_pill_offset_data_for_frame(sample_frame, &translation, &yaw);
                    real_vector3d transformed;
                    matrix4x3_transform_vector(&world, (const real_vector3d *)&translation, &transformed);
                    real_point3d new_position;
                    point_from_line3d(&world.position, &transformed, 1.0f, &new_position);
                    real_vector3d forward = world.forward;
                    real_vector3d up = world.up;
                    if (fabsf(yaw) >= giants_motion_epsilon)
                    {
                        float cosine = (float)cos(yaw);
                        float sine = (float)sin(yaw);
                        rotate_vector_about_axis(&forward, &world.up, sine, cosine);
                    }
                    matrix4x3_rotation_from_vectors(&world, &forward, &up);
                    world.position = new_position;
                }
                real_matrix4x3 local;
                matrix4x3_from_orientation(&local, &orientations[marker.node_index]);
                real_matrix4x3 combined;
                matrix4x3_compute_product(&local, &marker.node_to_marker, &combined);
                const render_model_node *node = (const render_model_node *)render_model->nodes.address + marker.node_index;
                while (node->parent_node_index != -1)
                {
                    real_matrix4x3 parent_matrix;
                    giants_matrix_from_orientation(&parent_matrix, &orientations[node->parent_node_index]);
                    matrix4x3_compute_product(&parent_matrix, &combined, &combined);
                    node = (const render_model_node *)render_model->nodes.address + node->parent_node_index;
                }
                matrix4x3_compute_product(&world, &combined, &combined);
                memcpy(matrix, &combined, sizeof(*matrix));
                result = true;
                }
            }
            else
                result = true;
        }
    }
    return result;
}




// @address 0x3602e8 ?giant_setup_buckling_parameters@@YAXJ@Z
void giant_setup_buckling_parameters(long giant_index)
{
    s_giant_datum *giant = OBJECT_GET(giant_index, s_giant_datum, 0x2000);
    const c_animation_manager *animation_manager = (const c_animation_manager *)((const char *)giant + giant->object.animation.offset);
    const s_giant_definition *definition = (const s_giant_definition *)tag_get(0, giant->object.definition_index);
    // match: 0x3602e8: tag definition is non-null, folding the variant check below
    __assume(definition != 0);
    const s_giant_variant *variant = &definition->variants[giant->variant_index];
    if (!variant || animation_manager->m_current_goal.state != variant->value)
        return;

    giant->buckle_fraction = 1.0f;
    unsigned long *giant_flags = &giant->giant_flags;
    *giant_flags &= ~2UL;
    giant->node_index = -1;
    giant->rotation = *global_identity_quaternion;
    object_get_world_matrix(giant_index, &giant->identity_copy);
    if (variant->buckle_width_steps <= 0 || variant->buckle_length_steps <= 0)
        return;

    object_marker marker;
    if (!internal_object_get_markers_by_string_id(giant_index, variant->buckle_marker_name, &marker, 1, false))
        return;

    giant->buckle_fraction = 0.0f;
    const c_model_animation *animation = giants_channel_animation((const c_animation_channel *)animation_manager);
    short frame_count = animation->frame_count;
    real_matrix4x3 best_matrix;
    real_matrix4x3 matrix;
    if (!giant_solve_future_marker_position(giant_index, variant->buckle_marker_name, 0, &best_matrix) ||
        !giant_solve_future_marker_position(giant_index, variant->buckle_marker_name, frame_count - 1, &matrix))
        return;

    float x_start = -variant->buckle_width;
    float y_start = -variant->buckle_length;
    float x_step = variant->buckle_width / (float)variant->buckle_width_steps;
    float y_step = variant->buckle_length / (float)variant->buckle_length_steps;
    long x_step_count = variant->buckle_width_steps * 2;
    long y_step_count = variant->buckle_length_steps * 2;
    real_vector3d forward = giant->object.forward;
    forward.k = 0.0f;
    normalize3d(&forward);
    real_matrix4x3 world_center_matrix;
    real_matrix4x3 max_depth_matrix;
    matrix4x3_from_point_and_nonaxial_vectors(&world_center_matrix, &best_matrix.position, &forward, global_up3d);
    matrix4x3_from_point_and_nonaxial_vectors(&max_depth_matrix, &matrix.position, &forward, global_up3d);
    setup_buckling_globals(giant_index, x_step_count, y_step_count, &world_center_matrix, x_start, y_start, x_step, y_step);

    float pitch_minimum = variant->buckle_pitch_minimum;
    float pitch_maximum = variant->buckle_pitch_maximum;
    float roll_minimum = variant->buckle_roll_minimum;
    float roll_maximum = variant->buckle_roll_maximum;
    if (pitch_minimum > pitch_maximum)
        pitch_maximum = pitch_minimum;
    if (roll_minimum > roll_maximum)
        roll_maximum = roll_minimum;
    const s_buckling_map *map = &giant->buckling_map;
    float best_score = compute_score_of_buckle_angles(map, 0.0f, 0.0f, &max_depth_matrix, &best_matrix);
    float pitch_step = MAX((pitch_maximum - pitch_minimum) * 0.1f, 0.01f);
    float roll_step = MAX((roll_maximum - roll_minimum) * 0.1f, 0.01f);
    for (float roll = roll_minimum; roll <= roll_maximum; roll += roll_step)
    {
        for (float pitch = pitch_minimum; pitch <= pitch_maximum; pitch += pitch_step)
        {
            float score = compute_score_of_buckle_angles(map, pitch, roll, &max_depth_matrix, &matrix);
            if (score > best_score)
            {
                best_score = score;
                memcpy(&best_matrix, &matrix, sizeof(real_matrix4x3));
            }
        }
    }

    short node_index = marker.node_index;
    real_matrix4x3 *node_matrix = object_get_node_matrix(giant_index, node_index);
    real_matrix4x3 relative;
    real_matrix4x3 rotation_matrix;
    matrix4x3_inverse(&max_depth_matrix, &matrix);
    matrix4x3_compute_product(&matrix, node_matrix, &relative);
    matrix4x3_compute_product(&best_matrix, &relative, &matrix);
    matrix4x3_inverse(node_matrix, &relative);
    matrix4x3_compute_product(&relative, &matrix, &rotation_matrix);
    matrix3x3_rotation_to_quaternion(&rotation_matrix.rotation, &giant->rotation);
    giant->node_index = node_index;
    *giant_flags |= 2UL;
    memcpy(&giant->identity_copy, &best_matrix, sizeof(real_matrix4x3));

    float center_z = world_center_matrix.position.z;
    float depth = max_depth_matrix.position.z - center_z;
    giant->buckle_position = 0.0f;
    giant->buckle_fraction = 0.0f;
    giant->buckle_acceleration = giants_motion_epsilon;
    if (fabs(depth) < giants_motion_epsilon)
        return;

    giant->buckle_fraction = (best_matrix.position.z - center_z) / depth;
    giant->buckle_fraction = PIN(giant->buckle_fraction, 0.0f, 1.0f);
    giant->buckle_velocity = 0.0f;
    float marker_offset = marker.matrix.position.z - center_z;
    giant->buckle_position = PIN(marker_offset / depth, 0.0f, 1.0f);
    float speed = global_gravity_get() * variant->settle_scale;
    giant->buckle_acceleration = speed / (float)fabs(depth);
    giant->buckle_acceleration = MAX(giant->buckle_acceleration, giants_motion_epsilon);
}
struct s_location;

class c_animation_channel;
class c_model_animation_graph;

static __forceinline bool giant_animation_channel_active(const c_animation_channel *channel)
{
    return channel->m_enabled != 0 && channel->m_graph_index != -1 && (unsigned short)channel->m_animation_id.m_animation_index < 0x8000;
}

static __forceinline bool giant_object_flag_test(unsigned long flags, long bit)
{
    return ((flags >> bit) & 1) != 0;
}

static __forceinline bool giant_animation_is_world_relative(const c_model_animation *animation)
{
    return ((animation->internal_flags >> 1) & 1) != 0;
}

static __forceinline const c_model_animation *giants_get_active_animation(const c_animation_manager *motion)
{
    const c_animation_channel *channel = &motion->m_state_channel;
    if (giant_animation_channel_active(&motion->m_transition_channel))
        channel = &motion->m_transition_channel;
    if ((unsigned short)channel->m_animation_id.m_animation_index < 0x8000)
        return ((const c_model_animation_graph *)tag_get('jmad', channel->m_graph_index))->get_animation(channel->m_animation_id);
    return 0;
}
static __forceinline void giants_read_state_motion_offsets(const c_animation_manager *motion, real_vector3d *translation, float *yaw)
{
    if (giant_animation_channel_active(&motion->m_transition_channel))
    {
        ((const c_animation_channel *)&motion->m_transition_channel)->get_motion_offsets(translation, yaw);
    }
    else if (giant_animation_channel_active(&motion->m_state_channel))
    {
        ((const c_animation_channel *)&motion->m_state_channel)->get_motion_offsets(translation, yaw);
    }
    else
    {
        memcpy(translation, &k_real_math_origin3d, sizeof(*translation));
        *yaw = 0.0f;
    }
}

// @address 0x3607b0 ?giant_handle_motor_update@@YA_NJ_N@Z
bool giant_handle_motor_update(long giant_index, bool update_node_matrices)
{
    object_datum *object = (object_datum *)OBJECT_GET(giant_index, s_giant_datum, 0x2000);
    c_animation_manager *motion = (c_animation_manager *)((char *)object + ((s_giant_datum *)object)->object.animation.offset);
    c_animation_manager *manager = (c_animation_manager *)motion;
    bool result = false;
    if (manager->current_state_contains_motion_data()
        && !manager->state_channel_finished()
        && !manager->state_channel_paused()
        && (motion->m_flags & 1) == 0)
    {
        real_vector3d forward = object->forward;
        const c_model_animation *animation = giants_get_active_animation(motion);
        if (animation != 0 && !giant_animation_is_world_relative(animation))
        {
            real_vector3d translation;
            float yaw;
            giants_read_state_motion_offsets(motion, &translation, &yaw);
            real_matrix4x3 matrix;
            matrix4x3_rotation_from_vectors(&matrix, &object->forward, &object->up);
            real_vector3d offset;
            matrix4x3_transform_vector(&matrix, &translation, &offset);
            real_point3d new_position;
            point_from_line3d(&object->position, &offset, 1.0f, &new_position);
            if (fabs(yaw) >= 0.0001f)
            {
                float cosine = (float)cos(yaw);
                float sine = (float)sin(yaw);
                rotate_vector_about_axis(&forward, &object->up, sine, cosine);
            }
            object_set_position_direct(giant_index, &new_position, &forward, &object->up, 0, false);
        }
        result = true;
    }
    havok_object_halt_keyframed_rigid_bodies(giant_index);
    if (flag_test(((s_giant_datum *)object)->giant_flags, 4L))
        object_disconnect_from_physics(giant_index);
    object_compute_node_matrices(giant_index);
    if (update_node_matrices)
    {
        long node_count = (unsigned long)object->node_matrices.size / sizeof(real_matrix4x3);
        real_matrix4x3 *node_matrices = (real_matrix4x3 *)((char *)object + object->node_matrices.offset);
        giant_adjust_node_matrices(giant_index, node_count, node_matrices);
    }
    bool reconnected = false;
    if (flag_test(((s_giant_datum *)object)->giant_flags, 4L))
    {
        object_reconnect_to_physics(giant_index);
        reconnected = true;
    }
    s_giant_datum *giant = (s_giant_datum *)object;
    unsigned long *flags = &giant->giant_flags;
    *flags &= ~0x10UL;
    if (flag_test(giant->giant_flags, 6L))
    {
        giant->havok_rebuild_ticks = (unsigned char)MIN(giant->havok_rebuild_ticks + 1, 254);
        float rebuild_time = (float)TLS_GAME_TIME_GLOBALS->ticks_per_second * 0.7f + 0.5f;
        long rebuild_tick = (long)rebuild_time;
        if (giant->havok_rebuild_ticks == rebuild_tick)
        {
            *flags |= 0x80;
            havok_object_rebuild(giant_index, 0);
            if (object_early_mover_local_space_index(giant_index) != -1)
            {
                long child_index = ((object_datum *)OBJECT_GET(giant_index, s_giant_datum, 0x2000))->first_child_object_index;
                while (child_index != -1)
                {
                    object_datum *child = object_get(child_index);
                    long next_index = child->next_object_index;
                    if (giant_object_flag_test(child->physics_flags, 4))
                        object_delete(child_index);
                    child_index = next_index;
                }
            }
        }
    }
    else
    {
        havok_object_keyframed_rigid_bodies_to_node_transforms(giant_index, reconnected);
    }
    return result;
}


// @address 0x360d68 ?giant_find_pathfinding_location@@YAXJPAFPAJPAVc_sector_ref@@PATreal_point3d@@1PAK@Z
void giant_find_pathfinding_location(
    // match: 0x360d68: pointer __restrict qualifier prevents alias reload
    long giant_index, short *, long *, c_sector_ref *__restrict sector, real_point3d *point, long *object_index, unsigned long *flags)
{
    s_giant_datum *giant = OBJECT_GET(giant_index, s_giant_datum, 0x2000);
    bool result = false;
    long game_time = TLS_GAME_TIME_GLOBALS->time;
    if (game_time > giant->pathfinding_time)
    {
        collision_result collision;
        giant->pathfinding_time = game_time;
        real_point3d start;
        object_get_origin(giant_index, &start);
        point_from_line3d(&start, global_up3d, 0.4f, &start);
        real_vector3d vector;
        scale_vector3d(global_down3d, 4.0f, &vector);
        if (collision_test_vector(k_biped_ground_collision_flags, 0, &start, &vector, -1, -1, -1, &collision))
        {
            giant->pathfinding_point = collision.point;
            giant->pathfinding_structure_index = (short)collision.structure_bsp_index;
            giant->pathfinding_surface_index = collision.surface_index;
            giant->pathfinding_object_index = collision.object_index;
            giant->pathfinding_flags = collision.bsp_reference;
            if (collision.surface_index != -1)
                giant->pathfinding_sector = collision_surface_get_sector((short)collision.structure_bsp_index, collision.surface_index,
                    collision.instance_index, collision.object_index, collision.bsp_reference, &collision.point);
            else
            {
                giant->pathfinding_sector.clear();
            }
            result = true;
        }
    }
    else
    {
        result = true;
    }
    if (result)
    {
        if (point)
            *point = giant->pathfinding_point;
        if (object_index)
            *object_index = giant->pathfinding_object_index;
        if (flags)
            *flags = giant->pathfinding_flags;
        if (sector)
        {
            *sector = giant->pathfinding_sector;
            return;
        }
    }
    else
    {
        if (point)
            object_get_origin(giant_index, point);
        if (object_index)
            *object_index = -1;
        if (flags)
            *flags = -1;
        if (sector)
        {
            sector->clear();
        }
    }
}
