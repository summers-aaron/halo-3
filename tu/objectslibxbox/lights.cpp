// objectslibxbox/lights.obj: from the records 0x10dc58 0x10dcc8 0x10fdd0 0x10ff88 0x1105c0 0x110f70 0x116870 0x15c360 0x17c6b0 0x205e70 0x20cf20 0x2b8e68 0x2b8ee0 0x2b9030 0x2b91e0 0x2b94c0 0x2b95d0 0x300d90 0x300e60 0x333310 0x333328 0x333340 0x333358 0x333370 0x333380 0x333390 0x333488 0x333588 0x3336a0 0x3338b0 0x333b78 0x333cb0 0x333da8 0x333e30 0x334280 0x334470 0x334538 0x334800 0x3348f0 0x3349b0 0x334be8 0x334d00 0x334ed0 0x335168 0x335220 0x335248 0x3352e8 0x335308 0x3353b0 0x3353d8 0x335478 0x335498 0x335538 0x335550 0x335570 0x335610 0x335628 0x5245a8
#include "objects/light_globals.h"
#include "math/real_math_globals.h"
#include "bitmaps/bitmap_group_definitions.h"
#include "bitmaps/bitmap_data.h"
#include "game/game_engine_local_globals.h"
#include "memory/tls_data_arrays.h"
#include "math/matrix_math_functions.h"
#include "math/real_math_functions.h"
#include "objects/object_reference_types.h"
#include "effects/lens_flare_attachment_types.h"
#include <string.h>
#include "memory/data_types.h"
#include "cseries/cseries_wrapped.h"
#include "tag_files/tag_types.h"
#include "game/game_time_types.h"
#include "cseries/crt_xdk.h"
#include <math.h>
#include <ppcintrinsics.h>
#include "cache/restricted_memory_regions.h"
#include "game/game_time.h"
#include "interface/first_person_weapons.h"
#include "math/geometry.h"
#include "math/spherical_harmonics.h"
#include "objects/lights.h"
#include "objects/objects.h"
#include "render/render_objects_static_lighting.h"
#include "scenario/scenario.h"
#include "structures/cluster_partitions.h"
#include "render/camera_fx_settings.h"
#include "memory/data.h"
#include "objects/object_datum.h"
#include "math/matrix_math_types.h"
#include "objects/object_placement.h"
#include "scenario/scenario_tag.h"
#include "physics/collision_bsp_query_types.h"
#include "scenario/scenario_structure_bsp_reference.h"
#include "render/render_camera_types.h"
#include "objects/light_intermediate_geometry_types.h"
#include "objects/light_geometry_types.h"
#include "cache/tag_resource_shortcut_cache_types.h"
#include "physics/collision_bsp_types.h"
#include "cache/restricted_memory.h"
#include "render/cinematic_light_entry_types.h"
#include "objects/light_sh_data_types.h"
#include "physics/collision_bsp.h"
#include "cache/cache_files.h"
#include "visibility/visibility_collection_frame_types.h"
#include "math/function_definitions.h"
#include "multithreading/thread_tls.h"
#include "objects/light_datum.h"
#include "objects/light_definitions.h"
#include "scenario/scenario_light_types.h"
#include "structures/structure_bsp_tag.h"
#include "structures/structure_marker_types.h"
#include "physics/collision_bsp_cache_types.h"
#include "objects/light_cinematic_types.h"
#include "objects/light_runtime_storage.h"
#include "visibility/visibility_projection_types.h"
#include "cutscene/cinematic_globals_types.h"
#include "cutscene/cinematics.h"
#include "game/player_mapping_types.h"
#include "memory/tls_globals.h"
#include "ai/swarm_spawner_globals.h"
#include "cache/restricted_memory_region_members.h"
#include "objects/light_allocation.h"
#include "structures/cluster_partition_new.h"
#include "structures/cluster_partition_types.h"
#include "effects/lens_flares.h"
#include "visibility/visibility_volume_build.h"
#include "objects/object_header_access.h"
#include "objects/object_header.h"
#include "tag_files/tag_groups.h"
#include "math/real_math.h"
#include "scenario/scenario_access.h"
#include "memory/datum_index_inlines.h"
#include "memory/datum_index.h"
#include "game/game_time_inline.h"


// @flags /TP /O2
// @address 0x11df7c4 lights_game_globals
// @address 0x11df6e8 light_data
// @address 0x11df624 light_cluster_partition
// @address 0x11df8e0 g_nondeterministic_light_data
// @address 0x333358 ?__tls_set_g_light_data_allocator@@YAXPAX@Z

__declspec(thread) lights_game_globals_definition *lights_game_globals;
__declspec(thread) s_data_array *light_data;
__declspec(thread) cluster_partition light_cluster_partition;
__declspec(thread) s_nondeterministic_light_data *g_nondeterministic_light_data;




// Restricted allocation members share one layout across accepted units.
typedef unsigned int uint;

namespace {
struct s_first_data_publisher {
    static __forceinline void publish(void *address) { __tls_set_g_light_first_data_allocator(address); }
};
struct s_data_reference_publisher {
    static __forceinline void publish(void *address) { __tls_set_g_light_data_reference_allocator(address); }
};
struct s_cluster_reference_publisher {
    static __forceinline void publish(void *address) { __tls_set_g_light_cluster_reference_allocator(address); }
};
struct s_render_data_publisher {
    static __forceinline void publish(void *address) { g_nondeterministic_light_data = (s_nondeterministic_light_data *)address; }
};
struct s_game_globals_publisher {
    static __forceinline void publish(void *address) { lights_game_globals = (lights_game_globals_definition *)address; }
};
}

namespace {
static __forceinline void data_set_new_base_address_inline(s_data_array **out, s_data_array *block)
{
    if (block != 0) {
        if (block->data_offset != 0) {
            char *rebased_address = (char *)block + block->data_offset;
            if (block->data != rebased_address)
                block->data = rebased_address;
        } else {
            block->data = 0;
        }
        if (block->in_use_bit_vector_offset != 0) {
            char *rebased_address = (char *)block + block->in_use_bit_vector_offset;
            if (block->in_use_bit_vector != rebased_address)
                block->in_use_bit_vector = rebased_address;
        } else {
            block->in_use_bit_vector = 0;
        }
    }
    *out = block;
}

}


// match: 0x334538: the accessor-local index keeps the bounding-sphere multiplies in place.
static __forceinline light_datum *lights_datum_get_inline(long light_index)
{
    long absolute_index = light_index & 0xffff;
    light_datum *light = (light_datum *)((s_data_array *)light_data)->data + absolute_index;
    return light;
}

static __forceinline bool lights_type_is_spherical(short type)
{
    return type == 0;
}


static __forceinline float lights_tangent(float angle)
{
    return (float)tan(angle);
}


static __forceinline void lights_get_geometry_from_definition(const light_definition *definition, light_geometry *geometry)
{
    memset(geometry, 0, sizeof(*geometry));
    geometry->type = definition->type;
    if (lights_type_is_spherical((short)geometry->type)) {
        geometry->near_distance = geometry->far_distance = definition->radius;
    } else {
        geometry->near_distance = definition->near_distance > k_real_epsilon ? definition->near_distance : k_real_epsilon;
        geometry->far_distance = definition->far_distance;
        geometry->field_of_view = definition->field_of_view;
        geometry->near_width = definition->radius;
        geometry->far_width = definition->radius;
    }
}
static __forceinline void lights_get_geometry_from_scenario(
    const light_definition *definition, const s_scenario_light *scenario_light, light_geometry *geometry)
{
    memset(geometry, 0, sizeof(*geometry));
    geometry->type = scenario_light->type;
    if (lights_type_is_spherical((short)geometry->type)) {
        geometry->near_distance = scenario_light->far_width;
        geometry->far_distance = scenario_light->far_width;
    } else {
        geometry->near_distance = scenario_light->near_distance > k_real_epsilon ? scenario_light->near_distance : k_real_epsilon;
        geometry->far_distance = scenario_light->far_distance;
        geometry->field_of_view = scenario_light->field_of_view;
        geometry->near_width = scenario_light->near_width;
        geometry->far_width = scenario_light->far_width;
    }
}

static __forceinline bool lights_location_valid(const s_location *location)
{
    return location->cluster_reference.bsp_index != -1;
}
static __forceinline long lights_first_active_bsp(unsigned long mask)
{
    return 31 - _CountLeadingZeros((0 - mask) & mask);
}
static __forceinline long lights_next_active_bsp(unsigned long mask, long index)
{
    unsigned long remaining = mask & ~((unsigned long)(1i64 << (index + 1)) - 1);
    return lights_first_active_bsp(remaining);
}
void light_disconnect_from_map(long light_index)
{
    light_datum *light = datum_get<light_datum>(light_data, light_index);
    if ((light->flags & 2) != 0) {
        cluster_partition_disconnect((cluster_partition *)&light_cluster_partition, light_index, &light->first_cluster_reference_index);
        light->flags &= ~8;
    }
}
// @address 0x10dcc8 ?light_reconnect_to_map@@YAXJ@Z
void light_reconnect_to_map(long light_index)
{
    light_datum *light = datum_get<light_datum>(light_data, light_index);
    light_definition *definition = (light_definition *)tag_get('ligh', light->definition_index);
    if ((light->flags & 2) && !(light->flags & 8)) {
        if (light->flags & 4)
            light_find_valid_point(&light->position, &light->endpoint, &light->valid_point, &light->offset_distance);
        if (!light_update_position_from_attachment(light)) {
            light_set_active_bit(light_index, false);
            return;
        }
        light_update_bounding_sphere(light_index);
        scenario_location_from_point(&light->location, &light->collision_sphere_center);
        if (!lights_location_valid(&light->location) && !((light->flags & 1) && (definition->flags & 2))) {
            if (light->attachment_object_index != -1)
                object_get_location(light->attachment_object_index, &light->location);
            else if (light->owner_object_index != -1)
                object_get_location(light->owner_object_index, &light->location);
            else
                scenario_location_from_point(&light->location, &light->bounding_sphere_center);
        }
        if (!lights_location_valid(&light->location)) {
            float radius = 0.01f;
            bool found = false;
            c_flags<e_collision_bsp_test_sphere_flag, unsigned long, k_collision_bsp_test_sphere_flag_count> flags;
            flags.clear();
            flags.set(_collision_bsp_test_sphere_stop_at_first_hit_bit, true);
            flags.set(_collision_bsp_test_sphere_surfaces_bit, true);
            unsigned long active_mask = g_active_structure_bsp_mask;
            for (long structure_bsp_index = lights_first_active_bsp(active_mask); structure_bsp_index != -1 && !found;
                structure_bsp_index = lights_next_active_bsp(active_mask, structure_bsp_index))
            {
                collision_bsp_test_sphere_result result;
                const collision_bsp *bsp =
                    (const collision_bsp *)g_collision_bsp_cache.thread_users[get_current_thread_index()].collision_bsps[structure_bsp_index];
                collision_bsp_test_sphere(bsp, flags, 0, 0, &light->collision_sphere_center, radius, &result);
                found = result.surface_count > 0;
                if (found)
                    scenario_location_from_leaf(&light->location, structure_bsp_index, result.surface_planes[0]);
            }
        }
        bool overflow = false;
        cluster_partition_reconnect((cluster_partition *)&light_cluster_partition, light_index, &light->first_cluster_reference_index,
            &light->bounding_sphere_center, light->bounding_sphere_radius, &light->location, 0, 0, 0, &overflow);
        light->flags |= 8;
    }
}
// @address 0x10fdd0 ?light_adjust_position_from_attachment_last_minute@@YA_NPAUlight_datum@@PBUrender_camera@@@Z
bool light_adjust_position_from_attachment_last_minute(light_datum *light, const render_camera *camera)
{
    bool result = true;
    long attachment = light->attachment_object_index;
    long object_index = attachment == -1 ? light->owner_object_index : attachment;
    if (object_index != -1) {
        light_definition *definition = (light_definition *)tag_get('ligh', light->definition_index);
        if (definition->flags & 4) {
            if (definition->flags & 128) {
                light->position = camera->position;
                light->forward = camera->forward;
                light->up = camera->up;
            } else if (attachment != -1) {
                if (!first_person_weapon_adjust_light(attachment, object_get_attachment_marker_name(attachment, light->attachment_index),
                        &light->position, &light->forward, &light->up))
                    result = false;
            }
            point_from_line3d(&light->position, &light->forward, 1.0f, &light->endpoint);
            force_orthogonal(&light->forward, &light->up);
        }
    }
    return result;
}

// @address 0x3348f0 ?force_orthogonal@@YAXPATreal_vector3d@@0@Z
void force_orthogonal(real_vector3d *forward, real_vector3d *up)
{
    const real_vector3d *up2 = up;
    const real_vector3d *fwd = forward;
    real_vector3d left;
    left.i = fwd->k * up2->j - fwd->j * up2->k;
    left.j = up2->k * fwd->i - fwd->k * up2->i;
    left.k = up2->i * fwd->j - fwd->i * up2->j;
    normalize3d(&left);
    float k = fwd->i * left.j - forward->j * left.i;
    float j = forward->k * left.i - fwd->i * left.k;
    float i = forward->j * left.k - forward->k * left.j;
    up->i = i;
    up->j = j;
    up->k = k;
    normalize3d(up);
}

// @address 0x1105c0 ?light_get_geometry@@YAXJPAUlight_geometry@@@Z
void light_get_geometry(long light_index, light_geometry *geometry)
{
    light_datum *light = datum_get<light_datum>(light_data, light_index);
    light_definition *definition = (light_definition *)tag_get('ligh', light->definition_index);
    s_scenario_light *scenario_light = 0;
    if (light->scenario_index != -1) {
        scenario_light = &((s_scenario_light *)global_scenario->light_volumes.address)[light->scenario_index];
        if (!(scenario_light->flags & 1))
            scenario_light = 0;
    }
    if (scenario_light == 0)
        lights_get_geometry_from_definition(definition, geometry);
    else
        lights_get_geometry_from_scenario(definition, scenario_light, geometry);
}


// @address 0x110f70 ?lights_handle_deleted_object@@YAXJ@Z
void lights_handle_deleted_object(long object_index)
{
    object_datum *object = (object_datum *)object_header_get(object_index)->object;
    if ((bool)((object->flags >> 6) & 1)) {
        c_data_iterator<light_datum> delete_iter;
        delete_iter.begin((s_data_array *)light_data);
        while (delete_iter.next()) {
            if (delete_iter.get_datum()->owner_object_index == object_index)
                light_delete_internal(delete_iter.get_index(), false);
        }
    }
    object->flags &= ~64;
}

// @address 0x116870 ?lights_update@@YAXXZ
void lights_update(void)
{
    long current_time = game_time_globals->time;
    for (long light_index = data_next_index((s_data_array *)light_data, -1); light_index != -1;
        light_index = inl_data_next_index((s_data_array *)light_data, light_index))
    {
        light_datum *light = datum_get<light_datum>(light_data, light_index);
        light_definition *definition = (light_definition *)tag_get('ligh', light->definition_index);
        light_update_state(light_index);
        if (light->flags & 8) {
            bool reconnect = false;
            if (light->attachment_object_index != -1) {
                const s_object_header *object = object_header_get(light->attachment_object_index);
                reconnect = (bool)((object->flags >> 2) & 1);
            } else if (light->owner_object_index != -1) {
                const s_object_header *object = object_header_get(light->owner_object_index);
                reconnect = (bool)((object->flags >> 2) & 1);
            }
            if (reconnect) {
                light_disconnect_from_map(light_index);
                light_reconnect_to_map(light_index);
            }
        }
        if (light->flags & 16) {
            if ((float)(current_time - light->creation_time) > (float)game_time_globals->ticks_per_second * definition->duration)
                light_delete_internal(light_index, true);
        }
    }
}


// @address 0x17c6b0 ?light_build_intermediate_geometry@@YAXPBTreal_point3d@@0PBTreal_vector3d@@1MJPAUlight_geometry@@@Z
void light_build_intermediate_geometry(const real_point3d *position, const real_point3d *target_position, const real_vector3d *forward,
    const real_vector3d *up, float scale, long gel_index, light_geometry *geometry)
{
    s_light_intermediate_geometry *intermediate = 0;
    if (lights_type_is_spherical((short)geometry->type))
    {
        s_light_intermediate_geometry *sphere = &geometry->intermediate;
        sphere->sphere.near_radius = geometry->near_distance * scale;
        sphere->sphere.far_radius = geometry->far_distance * scale;
        sphere->sphere.radius = sphere->sphere.near_radius > sphere->sphere.far_radius ? sphere->sphere.near_radius : sphere->sphere.far_radius;
    }
    else
    {
        intermediate = &geometry->intermediate;
        vector_from_points3d(position, target_position, &intermediate->frustum.direction);
        intermediate->frustum.length = dot_product3d(forward, &intermediate->frustum.direction);
        if (fabsf(intermediate->frustum.length - 0.0f) < k_real_epsilon || intermediate->frustum.length < 0.0f)
        {
            intermediate->frustum.direction = *forward;
            intermediate->frustum.length = 1.0f;
        }
        scale_vector3d(&intermediate->frustum.direction, 1.0f / intermediate->frustum.length, &intermediate->frustum.direction);
        intermediate->frustum.distance = geometry->far_distance;
        if (gel_index != -1)
        {
            bitmap_group *gel_definition = (bitmap_group *)tag_get(0, gel_index);
            bitmap_data *gel = (bitmap_data *)gel_definition->bitmaps.address;
            intermediate->frustum.distance = intermediate->frustum.distance * ((float)gel->height / (float)gel->width);
        }
        float field_of_view = geometry->field_of_view;
        field_of_view = (field_of_view > 0.0055555557f ? field_of_view : 0.0055555557f) < 3.136037f
            ? (field_of_view > 0.0055555557f ? field_of_view : 0.0055555557f)
            : 3.136037f;
        field_of_view = lights_tangent(field_of_view / 2.0f);
        intermediate->frustum.slope.i = field_of_view;
        intermediate->frustum.slope.j = field_of_view * intermediate->frustum.distance;
        intermediate->frustum.near_extent.x = geometry->near_distance;
        intermediate->frustum.near_extent.y = geometry->near_distance * intermediate->frustum.distance;
        intermediate->frustum.far_offset = geometry->far_width * scale;
        intermediate->frustum.near_offset = geometry->near_width * scale;
        point_from_line2d(&intermediate->frustum.near_extent, &intermediate->frustum.slope, intermediate->frustum.far_offset,
            &intermediate->frustum.far_extent);
        point_from_line3d(position, &intermediate->frustum.direction, intermediate->frustum.far_offset, &intermediate->frustum.far_point);
    }
    cross_product3d(forward, up, &geometry->left);
    geometry->origin_offset = 0.0f;
    geometry->origin = *position;
    if (geometry->type == 1 && intermediate->frustum.slope.i > 0.0001f)
    {
        geometry->origin_offset = geometry->near_distance / intermediate->frustum.slope.i / 2.0f;
        point_from_line3d(position, &intermediate->frustum.direction, -geometry->origin_offset, &geometry->origin);
    }
    geometry->valid = true;
}


// @address 0x2b8e68 lights_delete_cinematic_light_entry
void lights_delete_cinematic_light_entry(s_lights_reference_table *table, long index)
{
    s_lights_reference_entry *entry = &table->entries[index];
    if (entry->light_index != -1)
    {
        if (datum_try_and_get((s_data_array *)light_data, entry->light_index) != 0)
            light_delete_internal(entry->light_index, true);
        entry->light_index = -1;
    }
}

// @address 0x2b8ee0 lights_find_cinematic_object_entry
long lights_find_cinematic_object_entry(s_cinematic_light_globals *cinematic, long object_index, bool add)
{
    long result = -1;
    for (long entry_index = 0; entry_index < cinematic->object_entry_count; entry_index++) {
        if (cinematic->object_entries[entry_index].object_index == object_index) {
            result = entry_index;
            break;
        }
    }
    if (result == -1 && add) {
        if (cinematic->object_entry_count < 64) {
            result = cinematic->object_entry_count;
            cinematic->object_entry_count++;
        } else {
            for (long entry_index = 0; entry_index < cinematic->object_entry_count; entry_index++) {
                if (cinematic->object_entries[entry_index].object_index == -1) {
                    result = entry_index;
                    break;
                }
            }
        }
        if (result != -1) {
            s_cinematic_object_entry *entry = &cinematic->object_entries[result];
            object_datum *object = (object_datum *)object_header_get(object_index)->object;
            object->flags |= 0x01000000;
            entry->object_index = object_index;
            float *scale = &entry->scale;
            *scale = 1.0f;
            s_cinematic_object_entry *entry2 = &cinematic->object_entries[result];
            for (long i = 0; i < 9; i++)
                entry2->values0[i] = 0.0f;
            for (long i = 0; i < 9; i++)
                entry->values1[i] = 0.0f;
            for (long i = 0; i < 9; i++)
                entry->values2[i] = 0.0f;
        }
    }
    return result;
}


// @address 0x2b94c0 lights_reset_cinematic_lighting
void lights_reset_cinematic_lighting(void)
{
    int light_index_count = 0;
    s_cinematic_light_globals *cinematic = (s_cinematic_light_globals *)g_light_cinematic_globals_storage;
    if (cinematic->light_entry_count > 0) {
        s_cinematic_light_entry *light_entry = cinematic->light_entries;
        do {
            long light_index = light_entry->light_index;
            if (light_index != -1) {
                int index = light_index & 0xffff;
                s_data_array *array = (s_data_array *)light_data;
                if (index < array->first_unallocated) {
                    const short *datum = (const short *)(array->data + array->size * index);
                    const short &salt = *datum;
                    if (salt != 0 && salt == light_index >> 16) {
                        light_delete_internal(light_index, true);
                    }
                }
                light_entry->light_index = -1;
            }
            light_index_count++;
            light_entry++;
        } while (light_index_count < cinematic->light_entry_count);
    }
    int object_entry_count = cinematic->object_entry_count;
    int object_index_count = 0;
    cinematic->light_entry_count = 0;
    if (object_entry_count > 0) {
        s_cinematic_object_entry *object_entry = cinematic->object_entries;
        do {
            long object_index = object_entry->object_index;
            if (object_index != -1) {
                object_datum *object = (object_datum *)object_try_and_get_and_verify_type(object_index, 0xffffffff);
                if (object != 0) {
                    object->flags &= ~0x01000000;
                }
                object_entry->object_index = -1;
            }
            object_index_count++;
            object_entry++;
        } while (object_index_count < cinematic->object_entry_count);
    }
    cinematic->object_entry_count = 0;
    camera_fx_set_exposure(0.0f);
}

// @address 0x2b95d0 lights_delete_cinematic_object_lighting
void lights_delete_cinematic_object_lighting(long object_index)
{
    if (object_index != -1 && object_try_and_get_and_verify_type(object_index, -1) != 0) {
        s_cinematic_light_globals *cinematic = (s_cinematic_light_globals *)g_light_cinematic_globals_storage;
        long object_entry_index = lights_find_cinematic_object_entry(cinematic, object_index, false);
        if (object_entry_index != -1) {
            s_cinematic_object_entry *object_entry = &cinematic->object_entries[object_entry_index];
            if (object_entry->object_index != -1) {
                object_datum *object = (object_datum *)object_try_and_get_and_verify_type(object_entry->object_index, -1);
                if (object != 0)
                    object->flags &= ~0x01000000;
                object_entry->object_index = -1;
            }
        }
        s_cinematic_light_owners *owners = (s_cinematic_light_owners *)cinematic;
        for (long light_entry_index = 0; light_entry_index < owners->light_entry_count; light_entry_index++) {
            if (owners->light_entries[light_entry_index].object_index == object_index)
                lights_delete_cinematic_light_entry((s_lights_reference_table *)owners, light_entry_index);
        }
    }
}


// @address 0x300d90 lights_compute_direction_from_angles
void lights_compute_direction_from_angles(const real_euler_angles2d *angles, const real_matrix4x3 *matrix, real_vector3d *result, real_vector3d *up)
{
    float yaw = angles->yaw * 0.017453292f;
    float pitch = angles->pitch * 0.017453292f;
    float sin_pitch = (float)sin((double)pitch);
    real_vector3d direction;
    direction.k = -((float)cos((double)yaw) * sin_pitch);
    direction.j = (float)sin((double)yaw) * sin_pitch;
    direction.i = (float)cos((double)pitch);
    matrix3x3_transform_vector(&matrix->rotation, &direction, result);
    *up = matrix->rotation.up;
}

static __forceinline float lights_negate_and_power(real_vector3d *direction, const s_light_sh_element *element)
{
    direction->i = -direction->i;
    direction->j = -direction->j;
    float negative_k = -direction->k;
    float exponent = element->exponent;
    direction->k = negative_k;
    return (float)pow(2.0, (double)exponent) - 1.0f;
}

// @address 0x2b9030 lights_update_cinematic_sh_lighting
void lights_update_cinematic_sh_lighting(s_light_sh_data *light, long definition_index, const real_matrix4x3 *matrix)
{
    s_cinematic_lighting_definition *definition = (s_cinematic_lighting_definition *)tag_get(0, definition_index);
    light->definition_index = definition_index;
    memcpy(&light->matrix, matrix, sizeof(light->matrix));
    long index = 0;
    light->scale *= definition->scale;
    if (definition->sh_light_count <= 0)
        return;
    float red_coefficients[12];
    float green_coefficients[12];
    float blue_coefficients[12];
    do {
        s_light_sh_element *element = &definition->sh_lights[index];
        real_vector3d forward;
        real_vector3d up;
        lights_compute_direction_from_angles(&element->angles, matrix, &forward, &up);
        float power = lights_negate_and_power(&forward, element);
        power = PIN(power, 0.0f, 1024.0f);
        sh_eval_cone_light(3, &forward, element->angle * 3.1415927f, element->coeff_a *power, element->coeff_b *power,
            element->coeff_c *power, red_coefficients, green_coefficients, blue_coefficients);
        sh_add(light->sh0, 3, light->sh0, red_coefficients);
        sh_add(light->sh1, 3, light->sh1, green_coefficients);
        sh_add(light->sh2, 3, light->sh2, blue_coefficients);
        ++index;
    } while (index < definition->sh_light_count);
}

void __tls_set_g_lights_game_globals_allocator(void *allocation)
{
    g_local_game_engine_globals = (s_local_game_engine_globals *)allocation;
}

// @address 0x205e70 ?__tls_set_g_local_game_engine_globals_allocator@@YAXPAX@Z
void __tls_set_g_local_game_engine_globals_allocator(void *allocation)
{
    g_local_game_engine_globals = (s_local_game_engine_globals *)allocation;
}

template <>
void t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_lights_game_globals_allocator>::deallocate(void *allocation)
{
    __tls_set_g_lights_game_globals_allocator(0);
    restricted_region_free_member(3, m_member_index);
}

void __tls_set_g_light_first_data_allocator(void *allocation)
{
    light_cluster_partition.cluster_heads = (long *)allocation;
}

void __tls_set_g_light_data_reference_allocator(void *allocation)
{
    data_set_new_base_address((s_data_array **)&light_cluster_partition.data, (s_data_array *)allocation);
}

void __tls_set_g_light_cluster_reference_allocator(void *allocation)
{
    data_set_new_base_address((s_data_array **)&light_cluster_partition.references, (s_data_array *)allocation);
}

void __tls_set_g_light_data_allocator(void *allocation)
{
    data_set_new_base_address((s_data_array **)&light_data, (s_data_array *)allocation);
}

// @address 0x333370 lights_set_render_data_address
void lights_set_render_data_address(void *address)
{
    g_nondeterministic_light_data = (s_nondeterministic_light_data *)address;
}

// @address 0x333380 lights_set_game_globals_address
void lights_set_game_globals_address(void *address)
{
    lights_game_globals = (lights_game_globals_definition *)address;
}


// @address 0x333390 ?lights_initialize@@YAXXZ
void lights_initialize(void)
{
    light_data = data_new("lights", 400, 228, 0, (c_allocation_base *)&g_light_data_allocator);
    g_nondeterministic_light_data = (s_nondeterministic_light_data *)
        g_light_render_data_allocator.reserve_memory("nondet_light_data", "render lights", 9600, 0);
    lights_game_globals_definition *globals = (lights_game_globals_definition *)g_lights_game_globals_allocator.reserve_memory("lights globals", 0, 64, 0);
    globals->enabled = true;
    lights_game_globals = globals;
    if ((s_data_array *)light_data != 0)
        cluster_partition_new((cluster_partition *)&light_cluster_partition, "light", 0, (c_allocation_base *)&g_light_first_data_allocator,
            (c_allocation_base *)&g_light_data_reference_allocator, (c_allocation_base *)&g_light_cluster_reference_allocator);
}


// @address 0x333488 ?lights_initialize_for_new_structure_bsp@@YAXK@Z
void lights_initialize_for_new_structure_bsp(unsigned long structure_bsp_mask)
{
    c_data_iterator<light_datum> iterator;
    iterator.m_iterator.absolute_index = -1;
    iterator.m_iterator.index = -1;
    iterator.m_iterator.data = light_data;
    while (iterator.next()) {
        if (iterator.get_datum()->attachment_object_index == -1 && iterator.get_datum()->owner_object_index == -1)
            light_reconnect_to_map(iterator.get_index());
    }
    long structure_bsp_index = 0;
    do {
        if ((1 << structure_bsp_index) & structure_bsp_mask) {
            structure_bsp *bsp = global_structure_bsp_get(structure_bsp_index);
            long marker_index = 0;
            if (bsp->marker_light_palette_index.count > 0) {
                do {
                    lights_add_structure_bsp_marker_lights(structure_bsp_index, marker_index);
                    ++marker_index;
                } while (marker_index < bsp->marker_light_palette_index.count);
            }
        }
        ++structure_bsp_index;
    } while (structure_bsp_index < 16);
}

// @address 0x333588 ?lights_dispose_from_old_structure_bsp@@YAXK@Z
void lights_dispose_from_old_structure_bsp(unsigned long old_structure_bsp_mask)
{
    c_data_iterator<light_datum> delete_iter;
    delete_iter.begin((s_data_array *)light_data);
    while (delete_iter.next()) {
        if (delete_iter.get_datum()->attachment_object_index == -1 && delete_iter.get_datum()->owner_object_index == -1) {
            if ((delete_iter.get_datum()->flags & 8) != 0)
                light_disconnect_from_map(delete_iter.get_index());
        }
    }
    for (long bit = 0; bit < 16; bit++) {
        if ((old_structure_bsp_mask & (1 << bit)) == 0)
            continue;
        delete_iter.begin((s_data_array *)light_data);
        while (delete_iter.next()) {
            if (delete_iter.get_datum()->location.cluster_reference.bsp_index == bit && (delete_iter.get_datum()->flags & 64) != 0)
                light_delete_internal(delete_iter.get_index(), true);
        }
    }
}

// @address 0x3336a0 ?light_new_unattached@@YAJJJFPBTreal_point3d@@PBTreal_vector3d@@1M@Z
long light_new_unattached(long definition_index, long object_index, short node_index, const real_point3d *position,
    const real_vector3d *forward, const real_vector3d *up, float scale)
{
    long light_index = datum_new((s_data_array *)light_data);
    if (light_index != -1) {
        light_datum *light = datum_get<light_datum>(light_data, light_index);
        light_definition *definition = (light_definition *)tag_get('ligh', definition_index);
        light->flags = 0;
        light->definition_index = definition_index;
        light->creation_time = game_time_globals->time;
        light->scenario_index = -1;
        light->attachment_object_index = -1;
        light->attachment_index = -1;
        light->attachment_flags = 0;
        light->change_color = 0;
        light->initial_scale = scale;
        light->scale = scale;
        light->flags |= 1;
        if (definition->duration > 0.0f)
            light->flags |= 16;
        else
            light->flags &= ~16;
        light->first_cluster_reference_index = -1;
        if (object_index == -1) {
            light->position = *position;
            point_from_line3d(position, forward, 1.0f, &light->endpoint);
            light->forward = *forward;
            light->up = *up;
            light->owner_object_index = -1;
            light->owner_node_index = -1;
        } else {
            object_datum *object = (object_datum *)object_header_get(object_index)->object;
            light->local_position = *position;
            light->local_forward = *forward;
            light->local_up = *up;
            light->owner_object_index = object_index;
            light->owner_node_index = node_index;
            object->flags |= 1 << 6;
        }
        light_update_state(light_index);
        light->reservede0 = -1;
    }
    return light_index;
}


static __forceinline float lights_scenario_dot_product3d(const real_vector3d &a, const real_vector3d &b)
{
    float x_product = a.i * b.i;
    float y_product = a.j * b.j;
    float z_product = a.k * b.k;
    float result = x_product + (y_product + z_product);
    return result;
}

// @address 0x3338b0 ?light_new_from_scenario@@YAJJJ_NPBUs_scenario_light@@@Z
long light_new_from_scenario(long definition_index, long scenario_light_index, bool lightmap_mode, const s_scenario_light *scenario_light)
{
    light_definition *definition = (light_definition *)tag_get('ligh', definition_index);
    long light_index = -1;
    bool create = true;
    if (scenario_light->lightmap_type == 3 && !lightmap_mode)
        create = false;
    if (definition->lens_flare_index != -1)
        create = true;
    if (create)
    {
        light_index = datum_new((s_data_array *)light_data);
        if (light_index != -1)
        {
            light_datum *light = datum_get<light_datum>(light_data, light_index);
            real_vector3d direction;
            light_geometry geometry;
            real_matrix4x3 matrix;
            light->flags = 0;
            light->definition_index = definition_index;
            light->creation_time = -1;
            light->scenario_index = scenario_light_index;
            light->attachment_object_index = -1;
            light->attachment_index = -1;
            light->owner_object_index = -1;
            light->owner_node_index = -1;
            light->attachment_flags = 0;
            light->change_color = 0;
            light->initial_scale = 1.0f;
            light->scale = 1.0f;
            light->flags |= 1;
            light->flags |= 4;
            light->flags &= ~16;
            if (scenario_light->flags & 4)
                light->flags |= 32;
            else
                light->flags &= ~32;
            light->first_cluster_reference_index = -1;
            light_get_geometry(light_index, &geometry);
            matrix4x3_rotation_from_angles(&matrix, scenario_light->rotation.yaw, scenario_light->rotation.pitch, scenario_light->rotation.roll);
            light->position = scenario_light->position;
            light->forward = matrix.rotation.up;
            light->up = matrix.rotation.left;
            vector_from_points3d(&scenario_light->position, &scenario_light->endpoint, &direction);
            if (normalize3d(&direction) == 0.0f)
                direction = light->forward;
            else if (!lights_type_is_spherical((short)geometry.type) && lights_scenario_dot_product3d(direction, light->forward) < 0.5f)
                direction = light->forward;
            if (lights_type_is_spherical((short)geometry.type))
                geometry.near_distance = geometry.near_distance > geometry.far_distance ? geometry.near_distance : geometry.far_distance;
            else
                geometry.near_distance = lights_scenario_dot_product3d(light->forward, direction) * geometry.far_width;
            point_from_line3d(&light->position, &direction, geometry.near_distance, &light->endpoint);
            if (definition->flags & 2)
                point_from_line3d(&light->position, &direction, 0.001f, &light->position);
            light_update_state(light_index);
            light->reservede0 = -1;
        }
    }
    return light_index;
}

// @address 0x333b78 ?light_find_valid_point@@YA_NPBTreal_point3d@@0PAT1@PAM@Z
bool light_find_valid_point(const real_point3d *position, const real_point3d *endpoint, real_point3d *valid_point, float *radius)
{
    bool success = false;
    float offset_distance = 0.0f;
    float increment = 0.001f;
    real_vector3d direction;
    vector_from_points3d(position, endpoint, &direction);
    float length = normalize3d(&direction);
    while (length > 0.0f && offset_distance < length) {
        real_point3d candidate;
        point_from_line3d(position, &direction, offset_distance, &candidate);
        s_location location;
        scenario_location_from_point(&location, &candidate);
        if (lights_location_valid(&location)) {
            *radius = offset_distance;
            *valid_point = candidate;
            success = true;
            break;
        } else {
            offset_distance += increment;
            increment = increment * 2.0f > 0.1f ? 0.1f : increment * 2.0f;
        }
    }
    return success;
}

// @address 0x333cb0 ?lights_add_structure_bsp_marker_lights@@YAJJJ@Z
long lights_add_structure_bsp_marker_lights(long structure_bsp_index, long marker_index)
{
    long light_index = -1;
    structure_bsp *structure_bsp = global_structure_bsp_get(structure_bsp_index);
    structure_marker *marker = &((structure_marker *)structure_bsp->markers.address)[marker_index];
    long palette_index = ((short *)structure_bsp->marker_light_palette_index.address)[marker_index];
    if (palette_index >= 0 && palette_index < structure_bsp->marker_light_palette.count) {
        long definition_index = ((s_tag_reference *)structure_bsp->marker_light_palette.address)[palette_index].index;
        if (definition_index != -1) {
            real_matrix3x3 matrix;
            matrix3x3_rotation_from_quaternion(&matrix, &marker->rotation);
            light_index = light_new_unattached(definition_index, -1, -1, &marker->position, &matrix.forward, &matrix.left, 1.0f);
            if (light_index != -1) {
                light_datum *light = datum_get<light_datum>(light_data, light_index);
                light->flags |= 64;
            }
        }
    }
    return light_index;
}


// @address 0x333da8 ?light_set_active_bit@@YAXJ_N@Z
void light_set_active_bit(long index, bool enable)
{
    long i = index & 0xffff;
    s_data_array *array = (s_data_array *)light_data;
    light_datum *datum = (light_datum *)array->data + i;
    if (enable)
        datum->flags |= 2;
    else
        datum->flags &= ~2;
    unsigned long *bits = ((lights_game_globals_definition *)lights_game_globals)->enabled_lights;
    if (enable)
        bits[i >> 5] |= 1 << (i & 31);
    else
        bits[i >> 5] &= ~(1 << (i & 31));
}


static __forceinline bool lights_objects_can_connect_to_map()
{
    return object_globals && object_globals->can_connect_to_map;
}
static __forceinline void lights_evaluate_color(const c_function_definition *function, float input, float range, real_rgb_color *color)
{
    function->map_to_color_range_legacy(function->evaluate_legacy(input, range), color);
}
static __forceinline void lights_rgb_colors_multiply(const real_rgb_color *a, const real_rgb_color *b, real_rgb_color *result)
{
    result->red = a->red * b->red;
    result->green = a->green * b->green;
    result->blue = a->blue * b->blue;
}
static __forceinline void lights_rgb_color_scale(const real_rgb_color *color, float scale, real_rgb_color *result)
{
    result->red = color->red * scale;
    result->green = color->green * scale;
    result->blue = color->blue * scale;
}

// @address 0x333e30 ?light_update_state@@YAXJ@Z
void light_update_state(long light_index)
{
    light_datum *light = datum_get<light_datum>(light_data, light_index);
    light_definition *definition = (light_definition *)tag_get('ligh', light->definition_index);
    long object_index = -1;
    object_datum *object = 0;
    bool unattached_weapon = false;
    if (light->attachment_object_index != -1)
    {
        object_index = light->attachment_object_index;
        object = object_get(object_index);
        unattached_weapon = object_header_get(object_index)->object_type == 2 && object->parent_object_index == -1;
    }
    else if (light->owner_object_index != -1)
    {
        object_index = light->owner_object_index;
        object = object_get(object_index);
        unattached_weapon = object_header_get(object_index)->object_type == 2 && object->parent_object_index == -1;
    }
    bool active = false;
    if (!unattached_weapon)
    {
        float color_input = 0.0f;
        if (game_time_initialized())
        {
            color_input = (float)(game_time_globals->time - light->creation_time) * game_time_globals->tick_length;
            if (light->flags & 0x10)
                color_input /= definition->duration > k_real_epsilon ? definition->duration : k_real_epsilon;
        }
        float intensity_input = color_input;
        float color_range = 0.5f;
        float intensity_range = 0.5f;
        if (definition->color.input_name && object_index != -1)
            color_input = object_get_function_value_simple(object_index, definition->color.input_name, light->definition_index);
        if (definition->intensity.input_name && object_index != -1)
            intensity_input = object_get_function_value_simple(object_index, definition->intensity.input_name, light->definition_index);
        if (definition->color.range_name && object_index != -1)
            color_range = object_get_function_value_simple(object_index, definition->color.range_name, light->definition_index);
        if (definition->intensity.range_name && object_index != -1)
            intensity_range = object_get_function_value_simple(object_index, definition->intensity.range_name, light->definition_index);
        real_rgb_color color;
        lights_evaluate_color(&definition->color.function, color_input, color_range, &color);
        float intensity = definition->intensity.function.evaluate_scalar(intensity_input, intensity_range);
        light->initial_scale = light->scale;
        if (light->attachment_index != -1)
        {
            real_rgb_color change_color = k_light_default_change_color.rgb;
            object_get_change_color(object_index, light->change_color, &change_color);
            lights_rgb_colors_multiply(&color, &change_color, &color);
            light->initial_scale = object_get_function_value_simple(object_index, light->attachment_flags, light->definition_index);
            intensity *= light->initial_scale;
        }
        lights_rgb_color_scale(&color, intensity, &light->color);
        light->reservedc0 = 0.0f;
        if (object)
        {
            long ultimate_parent_index = inl_object_get_ultimate_parent(object_index);
        }
        light->custom_radius = 1.0f;
        if (light->flags & 1)
            active = light->color.red != 0.0f || light->color.green != 0.0f || light->color.blue != 0.0f;
    }
    if (active && !(light->flags & 2))
    {
        bool connect = true;
        light_set_active_bit(light_index, true);
        if (object_index != -1)
        {
            if (object_is_hidden(object_index))
                connect = false;
            else
                connect = object_is_connected_to_map(object_index);
        }
        if (connect && lights_objects_can_connect_to_map())
            light_reconnect_to_map(light_index);
    }
    else if (!active && (light->flags & 2))
    {
        if (light->flags & 8)
            light_disconnect_from_map(light_index);
        light_set_active_bit(light_index, false);
    }
}

// @address 0x7fc358 g_visibility_collection_frame
// @rdata 0x82029fe4..0x8202a05c
static const float k_light_fade_near_intensities[12] = { 5.0f, 20.0f, 9.0f, 8.0f, 7.0f, 6.0f, 5.0f, 4.0f, 3.0f, 2.0f, 1.0f, -5.0f };
static const float k_light_fade_far_intensities[12] = { 0.0f, 10.0f, 3.0f, 2.0f, 1.0f, 0.5f, 0.0f, -0.5f, -1.0f, -2.0f, -3.0f, -10.0f };
static const float k_light_fade_exponents[6] = { 1.0f, 3.0f, 1.7f, 1.0f, 0.6f, 0.3f };
// @rdata-end
static __forceinline s_nondeterministic_light_data *get_nondeterministic_light_data(long light_index)
{
    return &((s_nondeterministic_light_data *)g_nondeterministic_light_data)[datum_index_to_absolute_index(light_index)];
}

static __forceinline void nondeterministic_light_data_reset(long light_index)
{
    s_nondeterministic_light_data *data = get_nondeterministic_light_data(light_index);
    data->intensity = 1.0f;
    data->visibility_mark = g_visibility_collection_frame.light_mark - 1;
    for (long i = 0; i < 4; i++)
    {
        data->players[i].fade = 0xff;
        data->players[i].cooldown = 0;
    }
}

// @address 0x334280 light_update_fade_intensity
void light_update_fade_intensity(long light_index, long player_index, const real_matrix4x3 *camera, float scale)
{
    light_datum *light = datum_get<light_datum>(light_data, light_index);
    s_nondeterministic_light_data *data = get_nondeterministic_light_data(light_index);
    s_nondeterministic_light_player_state *player = &data->players[player_index];
    light_definition *definition = (light_definition *)tag_get('ligh', light->definition_index);
    long &cached_light_index = data->light_index;
    if (cached_light_index != light_index)
    {
        nondeterministic_light_data_reset(light_index);
        cached_light_index = light_index;
    }
    real_vector3d delta;
    delta.i = light->lod_bounding_sphere_center.x - camera->position.x;
    delta.j = light->lod_bounding_sphere_center.y - camera->position.y;
    delta.k = light->lod_bounding_sphere_center.z - camera->position.z;
    float distance = sqrtf(delta.i * delta.i + delta.j * delta.j + delta.k * delta.k);
    if (!(distance > 0.1f))
        distance = 0.1f;
    float fraction = light->lod_bounding_sphere_radius / (distance * scale);
    fraction = PIN(fraction, 0.0f, 1.0f);
    float fade = (float)pow(fraction, k_light_fade_exponents[definition->fade_exponent_index]);
    data->intensity = k_light_fade_near_intensities[definition->fade_near_index] * fade +
        k_light_fade_far_intensities[definition->fade_far_index] * (1.0f - fade);
    float specular_fade = (data->intensity - 0.3f) * 5.0f;
    data->specular_fade = PIN(specular_fade, 0.0f, 1.0f);
    if (player->cooldown)
        data->intensity *= 0.8f;
}

// @address 0x334470 ?light_build_render_geometry@@YA_NJPAUlight_geometry@@@Z
bool light_build_render_geometry(long light_index, light_geometry *geometry)
{
    light_datum *light = datum_get<light_datum>(light_data, light_index);
    light_definition *definition = (light_definition *)tag_get('ligh', light->definition_index);
    bool valid = false;
    light_get_geometry(light_index, geometry);
    light_build_intermediate_geometry(&light->position, &light->endpoint, &light->forward, &light->up, light->custom_radius, definition->gel_index, geometry);
    if (lights_type_is_spherical((short)geometry->type))
        valid = geometry->intermediate.sphere.radius > k_real_epsilon;
    else
        valid = geometry->intermediate.frustum.far_offset >
            k_real_epsilon &&
            geometry->intermediate.frustum.far_extent.x * geometry->intermediate.frustum.far_extent.x +
                geometry->intermediate.frustum.far_extent.y * geometry->intermediate.frustum.far_extent.y >
                    k_real_epsilon * k_real_epsilon;
    return valid;
}

// @address 0x334ed0 ?light_get_frustum_points@@YA_NJPATreal_point3d@@J@Z
bool light_get_frustum_points(long light_index, real_point3d *out_points, long max_points)
{
    light_datum *light = datum_get<light_datum>(light_data, light_index);
    const s_lights_placement *placement = &light->placement;
    bool result = false;
    light_geometry geometry;
    if (light_build_render_geometry(light_index, &geometry))
    {
        float near_distance = 0.0f;
        float near_width = geometry.intermediate.frustum.near_extent.x;
        float near_height = geometry.intermediate.frustum.near_extent.y;
        float far_distance = geometry.far_width;
        float far_width = geometry.intermediate.frustum.slope.i * 2.0f * far_distance + near_width;
        float far_height = geometry.intermediate.frustum.distance * far_width;
        real_vector3d forward = geometry.intermediate.frustum.direction;
        normalize3d(&forward);
        real_point3d near_points[4];
        real_point3d far_points[4];
        for (long corner_index = 0; corner_index < 4; corner_index++)
        {
            const long corner_order[4] = { 0, 1, 3, 2 };
            float x = 0.5f * ((corner_order[corner_index] & 1) ? 1.0f : -1.0f);
            float y = 0.5f * ((corner_order[corner_index] & 2) ? 1.0f : -1.0f);
            point_from_line3d(&placement->position, &forward, near_distance, &near_points[corner_index]);
            point_from_line3d(&placement->position, &forward, far_distance, &far_points[corner_index]);
            point_from_line3d(&near_points[corner_index], &geometry.left, x * near_width, &near_points[corner_index]);
            point_from_line3d(&far_points[corner_index], &geometry.left, x * far_width, &far_points[corner_index]);
            float near_vertical_offset = y * near_height;
            point_from_line3d(&near_points[corner_index], &placement->up, near_vertical_offset, &near_points[corner_index]);
            point_from_line3d(&far_points[corner_index], &placement->up, y * far_height, &far_points[corner_index]);
        }
        memcpy(out_points, near_points, sizeof(near_points));
        memcpy(out_points + 4, far_points, sizeof(far_points));
        result = true;
    }
    return result;
}


static __forceinline float lights_bounding_dot(const real_vector3d *a, const real_vector3d *b)
{
    return a->i * b->i + (a->j * b->j + a->k * b->k);
}

// @address 0x334538 ?light_update_bounding_sphere@@YAXJ@Z
void light_update_bounding_sphere(long light_index)
{
    light_datum *light = lights_datum_get_inline(light_index);
    light_definition *definition = (light_definition *)tag_get('ligh', light->definition_index);
    light_geometry geometry;
    light_get_geometry(light_index, &geometry);
    if (lights_type_is_spherical((short)geometry.type))
    {
        float max_radius = geometry.near_distance > geometry.far_distance ? geometry.near_distance : geometry.far_distance;
        if (light->offset_distance > 0.0f)
        {
            light->bounding_sphere_center = light->valid_point;
            float radius = light->offset_distance + max_radius;
            light->bounding_sphere_radius = radius;
        }
        else
        {
            light->bounding_sphere_center = light->position;
            light->bounding_sphere_radius = max_radius;
        }
        light->collision_sphere_center = light->bounding_sphere_center;
        light->lod_bounding_sphere_center = light->bounding_sphere_center;
        light->lod_bounding_sphere_radius = light->bounding_sphere_radius;
    }
    else
    {
        float custom_radius = light->custom_radius;
        light_build_intermediate_geometry(&light->position, &light->endpoint, &light->forward, &light->up, custom_radius, definition->gel_index, &geometry);
        float far_width = geometry.far_width;
        float width = geometry.intermediate.frustum.slope.i * far_width * 2.0f + geometry.intermediate.frustum.near_extent.x;
        float height = geometry.intermediate.frustum.distance * width;
        float left_dot = lights_bounding_dot(&geometry.left, &geometry.intermediate.frustum.direction);
        float s1 = left_dot < 0.0f ? -1.0f : 1.0f;
        float up_dot = lights_bounding_dot(&light->up, &geometry.intermediate.frustum.direction);
        float s2 = up_dot < 0.0f ? -1.0f : 1.0f;
        real_vector3d direction = geometry.intermediate.frustum.direction;
        normalize3d(&direction);
        real_point3d corner;
        corner.x = direction.i * far_width;
        corner.y = direction.j * far_width;
        corner.z = direction.k * far_width;
        point_from_line3d(&corner, &geometry.left, s1 * width * 0.5f, &corner);
        point_from_line3d(&corner, &light->up, s2 * height * 0.5f, &corner);
        float radius_squared = corner.x * corner.x + (corner.z * corner.z + corner.y * corner.y);
        light->bounding_sphere_radius = sqrtf(radius_squared);
        if (light->offset_distance > 0.0f)
            light->bounding_sphere_center = light->valid_point;
        else
            light->bounding_sphere_center = light->position;
        light->collision_sphere_center = light->bounding_sphere_center;
    }
    light_calculate_lod_bounding_sphere(light_index);
}

// @address 0x334800 ?light_calculate_lod_bounding_sphere@@YAXJ@Z
void light_calculate_lod_bounding_sphere(long light_index)
{
    light_datum *light = datum_get<light_datum>(light_data, light_index);
    bool spherical;
    {
        light_geometry geometry;
        light_get_geometry(light_index, &geometry);
        spherical = lights_type_is_spherical((short)geometry.type);
    }
    if (spherical) {
        light->lod_bounding_sphere_center = light->bounding_sphere_center;
        light->lod_bounding_sphere_radius = light->bounding_sphere_radius;
    } else {
        real_point3d points[8];
        if (light_get_frustum_points(light_index, points, 8)) {
            geometry_compute_points_minimum_bounding_sphere(points, 8, &light->lod_bounding_sphere_center, &light->lod_bounding_sphere_radius);
        } else {
            light->lod_bounding_sphere_radius = 0.0f;
            const real_point3d *origin = global_origin3d;
            light->lod_bounding_sphere_center = *origin;
        }
    }
}

// @address 0x334be8 ?light_delete_internal@@YAXJ_N@Z
void light_delete_internal(long light_index, bool update_object)
{
    light_datum *light = datum_get<light_datum>(light_data, light_index);
    if ((light->flags & 8) != 0)
        light_disconnect_from_map(light_index);
    if (light->owner_object_index != -1 && update_object) {
        object_datum *object = (object_datum *)object_header_get(light->owner_object_index)->object;
        bool object_has_light = false;
        c_data_iterator<light_datum> delete_iter;
        delete_iter.begin((s_data_array *)light_data);
        while (delete_iter.next()) {
            if (delete_iter.get_index() != light_index && delete_iter.get_datum()->owner_object_index == light->owner_object_index) {
                object_has_light = true;
                break;
            }
        }
        if (object_has_light)
            object->flags |= 64;
        else
            object->flags &= ~64;
    }
    light_set_active_bit(light_index, false);
    ::datum_delete((s_data_array *)light_data, light_index);
}

template <>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_light_first_data_allocator>::allocate(unsigned int size, const char *name)
{
    m_member_index = restricted_region_add_member<s_first_data_publisher>(3, name, "", size, 0, __tls_set_g_light_first_data_allocator);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(3, m_member_index);
}

template <>
void t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_light_first_data_allocator>::deallocate(void *allocation)
{
    __tls_set_g_light_first_data_allocator(0);
    restricted_region_free_member(3, m_member_index);
}

template <>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_light_data_reference_allocator>::allocate(unsigned int size, const char *name)
{
    m_member_index = restricted_region_add_member<s_data_reference_publisher>(3, name, "", size, 0, __tls_set_g_light_data_reference_allocator);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(3, m_member_index);
}

template <>
void t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_light_data_reference_allocator>::deallocate(void *allocation)
{
    data_set_new_base_address_inline((s_data_array **)&light_cluster_partition.data, 0);
    restricted_region_free_member(3, m_member_index);
}

template <>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_light_cluster_reference_allocator>::allocate(unsigned int size, const char *name)
{
    m_member_index = restricted_region_add_member<s_cluster_reference_publisher>(3, name, "", size, 0, __tls_set_g_light_cluster_reference_allocator);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(3, m_member_index);
}

template <>
void t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_light_cluster_reference_allocator>::deallocate(void *allocation)
{
    data_set_new_base_address_inline((s_data_array **)&light_cluster_partition.references, 0);
    restricted_region_free_member(3, m_member_index);
}

template <>
void t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_light_data_allocator>::deallocate(void *allocation)
{
    light_data = 0;
    restricted_region_free_member(3, m_member_index);
}

template <>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_light_data_allocator>::allocate(unsigned int size, const char *name)
{
    int member_index = g_restricted_regions[3].add_member(name, "", size, 0, __tls_set_g_light_data_allocator, 0, 0);
    uint offset = restricted_region_get_member_offset(3, member_index);
    data_set_new_base_address((s_data_array **)&light_data, (s_data_array *)(g_restricted_address[3] + offset));
    m_member_index = member_index;
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(3, member_index);
}

template <>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_light_render_data_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment)
{
    m_member_index = restricted_region_add_member<s_render_data_publisher>(2, name, type, size, 0, lights_set_render_data_address);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(2, m_member_index);
}

template <>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_light_render_data_allocator>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

template <>
void t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_light_render_data_allocator>::deallocate(void *allocation)
{
    s_render_data_publisher::publish(0);
    restricted_region_free_member(2, m_member_index);
}

// @address 0x335570 ?reserve_memory@?$t_restricted_allocation_manager@$02$0A@$0A@$1?lights_set_game_globals_address@@YAXPAX@Z@@QAAPAXPBD0IJ@Z
template <>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0), &lights_set_game_globals_address>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment)
{
    m_member_index = restricted_region_add_member<s_game_globals_publisher>(3, name, type, size, 0, lights_set_game_globals_address);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(3, m_member_index);
}

// @address 0x335610 ?allocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?lights_set_game_globals_address@@YAXPAX@Z@@UAAPAXIPBD@Z
template <>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &lights_set_game_globals_address>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

// @address 0x335628 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?lights_set_game_globals_address@@YAXPAX@Z@@UAAXPAX@Z
template <>
void t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &lights_set_game_globals_address>::deallocate(void *allocation)
{
    long value = m_member_index;
    lights_game_globals = 0;
    restricted_region_free_member(3, value);
}

void __tls_set_g_light_render_data_allocator(void *allocation)
{
    g_swarm_nondeterministic_counter = (unsigned short *)allocation;
}

static __forceinline real_matrix4x3 *lights_object_node_matrix(long object_index, short node_index)
{
    object_datum *object = (object_datum *)object_header_get(object_index)->object;
    real_matrix4x3 *matrices = (real_matrix4x3 *)((char *)object + object->node_matrices.offset);
    return &matrices[node_index];
}
static __forceinline void lights_rotate_vector(const real_matrix3x3 *matrix, const real_vector3d *v, real_vector3d *out)
{
    float i = v->i, j = v->j, k = v->k;
    out->i = matrix->left.i * j + (matrix->up.i * k + matrix->forward.i * i);
    out->j = matrix->left.j * j + (matrix->forward.j * i + matrix->up.j * k);
    out->k = matrix->up.k * k + (matrix->left.k * j + matrix->forward.k * i);
}

// @address 0x300e60 light_new_from_attachment
long light_new_from_attachment(const s_lights_attachment_definition *definition, const real_point3d *origin,
    const real_matrix4x3 *matrix, long object_index, long marker_name)
{
    long result = -1;
    if (definition->light.index != -1) {
        object_marker marker;
        bool has_marker = object_index != -1 && marker_name != 0 && object_try_and_get_and_verify_type(object_index, -1) != 0 &&
            internal_object_get_markers_by_string_id(object_index, marker_name, &marker, 1, false) > 0;
        real_point3d position;
        real_vector3d forward, up;
        real_vector3d left;
        if ((definition->flags & 4) != 0 && has_marker) {
            position = marker.matrix.position;
            forward = marker.matrix.rotation.forward;
            up = marker.matrix.rotation.up;
        } else {
            lights_compute_direction_from_angles(&definition->angles, matrix, &forward, &up);
            point_from_line3d(origin, &forward, -definition->offset, &position);
        }
        real_vector3d_build_axes_from_vectors(&forward, &up, &forward, &left, &up);
        if ((definition->flags & 2) != 0) {
            long node_index = 0;
            if (has_marker)
                node_index = marker.node_index;
            real_matrix4x3 inverse;
            matrix4x3_inverse(lights_object_node_matrix(object_index, node_index), &inverse);
            matrix4x3_transform_point(&inverse, &position, &position);
            lights_rotate_vector(&inverse.rotation, &forward, &forward);
            result = light_new_unattached(definition->light.index, object_index, (short)node_index, &position, &forward, &left, 1.0f);
        } else {
            result = light_new_unattached(definition->light.index, -1, -1, &position, &forward, &left, 1.0f);
        }
    }
    return result;
}

static __forceinline float lights_attachment_endpoint_xy(const real_point3d *point, const real_vector3d *vector, real_point3d *result)
{
    result->x = point->x + vector->i;
    result->y = point->y + vector->j;
    return point->z;
}

// @address 0x3349b0 light_update_position_from_attachment
bool light_update_position_from_attachment(light_datum *light)
{
    bool result = true;
    if (light->attachment_index != -1) {
        light_definition *definition = (light_definition *)tag_get('ligh', light->definition_index);
        bool adjusted = false;
        if (!adjusted) {
            long marker_name = object_get_attachment_marker_name(light->attachment_object_index, light->attachment_index);
            object_marker marker;
            if (internal_object_get_markers_by_string_id(light->attachment_object_index, marker_name, &marker, 1, false) <= 0) {
                result = false;
            } else {
                light->position = marker.matrix.position;
                light->forward = marker.matrix.rotation.forward;
                light->up = marker.matrix.rotation.up;
            }
        }
        point_from_line3d(&light->position, &light->forward, 1.0f, &light->endpoint);
        force_orthogonal(&light->forward, &light->up);
    } else if (light->owner_object_index != -1) {
        real_matrix4x3 *matrix = lights_object_node_matrix(light->owner_object_index, light->owner_node_index);
        matrix4x3_transform_point(matrix, &light->local_position, &light->position);
        matrix4x3_transform_normal(matrix, &light->local_forward, &light->forward);
        matrix4x3_transform_normal(matrix, &light->local_up, &light->up);
        point_from_line3d(&light->position, &light->forward, 1.0f, &light->endpoint);
        force_orthogonal(&light->forward, &light->up);
    }
    return result;
}

static __forceinline long *lights_cinematic_light_slot_new(s_cinematic_light_owners *owners, long object_index)
{
    long *light_index;
    long count = owners->light_entry_count;
    for (long entry_index = 0;; entry_index++) {
        if (entry_index < owners->light_entry_count) {
            if (owners->light_entries[entry_index].light_index == -1) {
                light_index = &owners->light_entries[entry_index].light_index;
                owners->light_entries[entry_index].object_index = object_index;
                break;
            }
        } else {
            if (count >= 16)
                return 0;
            light_index = &owners->light_entries[count].light_index;
            owners->light_entries[count].object_index = object_index;
            *light_index = -1;
            owners->light_entry_count++;
            break;
        }
    }
    return light_index;
}

// @address 0x2b91e0 lights_set_cinematic_object_lighting
void lights_set_cinematic_object_lighting(long object_index, long marker_name, long lighting_index, real_matrix4x3 *matrix)
{
    s_cinematic_light_globals *cinematic = (s_cinematic_light_globals *)g_light_cinematic_globals_storage;
    const s_cinematic_lighting_definition *definition = (const s_cinematic_lighting_definition *)tag_get(0, lighting_index);
    if (definition->sh_light_count > 0 || definition->scale != 1.0f) {
        long entry_index = lights_find_cinematic_object_entry(cinematic, object_index, true);
        if (entry_index != -1)
            lights_update_cinematic_sh_lighting((s_light_sh_data *)&cinematic->object_entries[entry_index], lighting_index, matrix);
    }
    real_point3d origin;
    object_marker marker;
    if (marker_name != 0 && internal_object_get_markers_by_string_id(object_index, marker_name, &marker, 1, false) > 0)
        origin = marker.matrix.position;
    else
        origin = ((object_datum *)object_header_get(object_index)->object)->bounding_center;
    for (long attachment_index = 0; attachment_index < definition->attachment_count; attachment_index++) {
        const s_lights_attachment_definition *attachment = &definition->attachments[attachment_index];
        long *light_index = lights_cinematic_light_slot_new((s_cinematic_light_owners *)cinematic, object_index);
        if (light_index)
            *light_index = light_new_from_attachment(attachment, &origin, matrix, object_index, marker_name);
    }
}

extern "C" {
// @address 0x119a463 g_light_multiple_markers_warning_issued
unsigned char g_light_multiple_markers_warning_issued;
}

// @address 0x334d00 light_render_lens_flare
void light_render_lens_flare(long light_index, long player_index)
{
    light_datum *light = datum_get<light_datum>(light_data, light_index);
    light_definition *definition = (light_definition *)tag_get('ligh', light->definition_index);
    if (definition->lens_flare_index != -1)
    {
        if (light->attachment_index != -1)
        {
            long object_index = light->attachment_object_index;
            long marker_name = object_get_attachment_marker_name(object_index, light->attachment_index);
            object_marker markers[2];
            long count = internal_object_get_markers_by_string_id(object_index, marker_name, markers, 2, false);
            if (count > 1)
            {
                if (!g_light_multiple_markers_warning_issued)
                    g_light_multiple_markers_warning_issued = 1;
                count = 1;
            }
            long marker_index = 0;
            if (marker_index < count)
            {
                game_time_globals_definition *time_globals = game_time_globals;
                do
                {
                bool first_person = render_object_is_first_person(player_index, light->attachment_object_index);
                float scale = light->initial_scale;
                float age = game_time_get_safe_in_seconds() - (float)light->creation_time * time_globals->tick_length;
                lens_flares_create_internal(definition->lens_flare_index, _lens_flare_attachment_type_light, light_index,
                    (unsigned short)marker_index, &markers[marker_index].matrix.position, &markers[marker_index].matrix.rotation.forward,
                    &light->color, age, scale, 1.0f, first_person, player_index);
                } while (++marker_index < count);
            }
        }
        else
        {
            bool first_person = render_object_is_first_person(player_index, light->attachment_object_index);
            float scale = light->initial_scale;
            float age = game_time_get_safe_in_seconds() - (float)light->creation_time * game_time_globals->tick_length;
            lens_flares_create_internal(definition->lens_flare_index, _lens_flare_attachment_type_light, light_index, 0xffff,
                &light->position, &light->forward, &light->color, age, scale, 1.0f, first_person, player_index);
        }
    }
}

// match: 0x10ff88: inline copy; calling out-of-line normalize3d emits a bl and reshuffles the caller (xsame: 302 lines).
static __forceinline float normalize3d_inline_lights(real_vector3d *vector)
{
    float length = sqrtf(vector->i * vector->i + vector->j * vector->j + vector->k * vector->k);
    if (fabsf(length) >= 0.0001f) {
        float inverse = 1.0f / length;
        vector->i *= inverse;
        vector->j *= inverse;
        vector->k *= inverse;
        return length;
    }
    return 0.0f;
}

static __forceinline long lights_polygon2d_from_bounds(const real_rectangle2d *bounds, long count, real_point2d *polygon)
{
    polygon[0].x = bounds->x0;
    polygon[0].y = bounds->y0;
    polygon[1].x = bounds->x1;
    polygon[1].y = bounds->y0;
    polygon[2].x = bounds->x1;
    polygon[2].y = bounds->y1;
    polygon[3].x = bounds->x0;
    polygon[3].y = bounds->y1;
    return 4;
}

// @address 0x10ff88 ?light_build_visibility_projections@@YAXPBTreal_vector3d@@0PBUlight_geometry@@PAUvisibility_projection@@PAF@Z
void light_build_visibility_projections(const real_vector3d *forward, const real_vector3d *left, const light_geometry *geometry,
    visibility_projection *projections, short *projection_count)
{
    *projection_count = lights_type_is_spherical((short)geometry->type) ? 6 : 1;
    for (long projection_index = 0; projection_index < *projection_count; projection_index++)
    {
        visibility_projection *projection = &projections[projection_index];
        real_rectangle2d bounds = *global_null_rectangle2d;
        memset(projection, 0, sizeof(*projection));
        if (lights_type_is_spherical((short)geometry->type))
        {
            float extent = geometry->intermediate.sphere.radius > 0.0001f ? geometry->intermediate.sphere.radius : 0.0001f;
            real_vector3d points[8] = {
                { extent, extent, -extent },
                { -extent, extent, -extent },
                { -extent, -extent, -extent },
                { extent, -extent, -extent },
                { extent, extent, extent },
                { -extent, extent, extent },
                { -extent, -extent, extent },
                { extent, -extent, extent },
            };
            long face_points[6][4] = {
                { 0, 1, 2, 3 },
                { 0, 3, 7, 4 },
                { 0, 4, 5, 1 },
                { 1, 5, 6, 2 },
                { 2, 6, 7, 3 },
                { 4, 7, 6, 5 },
            };
            real_vector3d face_directions[6] = {
                { 0.0f, 0.0f, -1.0f },
                { 1.0f, 0.0f, 0.0f },
                { 0.0f, 1.0f, 0.0f },
                { -1.0f, 0.0f, 0.0f },
                { 0.0f, -1.0f, 0.0f },
                { 0.0f, 0.0f, 1.0f },
            };
            long face_index = projection_index;
            projection->far_distance = geometry->intermediate.sphere.radius;
            projection->mirrored = false;
            projection->near_plane_valid = false;
            negate_vector3d(&face_directions[face_index], &projection->basis_to_world.rotation.up);
            real_vector3d axis = *global_forward3d;
            if (face_index == 1 || face_index == 3)
                axis = *global_left3d;
            cross_product3d(&projection->basis_to_world.rotation.up, &axis, &projection->basis_to_world.rotation.left);
            normalize3d_inline_lights(&projection->basis_to_world.rotation.left);
            cross_product3d(&projection->basis_to_world.rotation.left, &projection->basis_to_world.rotation.up,
                &projection->basis_to_world.rotation.forward);
            projection->basis_to_world.position = geometry->origin;
            projection->basis_to_world.scale = 1.0f;
            for (long point_index = 0; point_index < 4; point_index++)
            {
                real_vector3d *point = &points[face_points[face_index][point_index]];
                real_vector3d transformed_point;
                matrix4x3_inverse_transform_vector(&projection->basis_to_world, point, &transformed_point);
                projection->polygon[point_index].x = -transformed_point.i / transformed_point.k;
                projection->polygon[point_index].y = -transformed_point.j / transformed_point.k;
            }
            projection->polygon_count = 4;
            real_rectangle2d_enclose_points(&bounds, 4, projection->polygon);
        }
        else
        {
            projection = projections;
            *projection_count = 1;
            memset(projection, 0, sizeof(*projection));
            projection->basis_to_world.rotation.forward = geometry->left;
            projection->basis_to_world.rotation.left = *left;
            negate_vector3d(forward, &projection->basis_to_world.rotation.up);
            projection->basis_to_world.position = geometry->origin;
            projection->basis_to_world.scale = 1.0f;
            projection->mirrored = geometry->type == 1 && geometry->field_of_view < 0.0001f;
            projection->near_plane_valid = geometry->origin_offset > 0.0f;
            projection->near_clip = geometry->origin_offset;
            real_point3d near_point;
            point_from_line3d(&projection->basis_to_world.position, forward, projection->near_clip, &near_point);
            plane3d_from_point_and_normal(&projection->near_plane, &near_point, forward);
            projection->far_distance = geometry->origin_offset + geometry->intermediate.frustum.far_offset;
            real_vector3d transformed_direction;
            matrix4x3_inverse_transform_vector(&projection->basis_to_world, &geometry->intermediate.frustum.direction, &transformed_direction);
            real_point2d center;
            center.x = -transformed_direction.i / transformed_direction.k;
            center.y = -transformed_direction.j / transformed_direction.k;
            bounds.x0 = center.x - geometry->intermediate.frustum.near_extent.x / geometry->origin_offset / 2.0f;
            bounds.y0 = center.y - geometry->intermediate.frustum.near_extent.y / geometry->origin_offset / 2.0f;
            bounds.x1 = center.x + geometry->intermediate.frustum.near_extent.x / geometry->origin_offset / 2.0f;
            bounds.y1 = center.y + geometry->intermediate.frustum.near_extent.y / geometry->origin_offset / 2.0f;
            projection->polygon_count = lights_polygon2d_from_bounds(&bounds, 4, projection->polygon);
        }
        projection->volume_valid = true;
        projection->far_plane_valid = true;
        projection->far_plane_reversed = false;
        matrix4x3_inverse(&projection->basis_to_world, &projection->world_to_basis);
        visibility_volume_build(projection, 0, &bounds, &projection->volume);
    }
}

static bool lights_allow_dynamic_lighting()
{
    bool allowed = true;
    return allowed;
}
// @address 0x15c360 ?light_theoretically_produces_light@@YA_NJJJ@Z
bool light_theoretically_produces_light(long light_index, long player_index, long)
{
    light_datum *light = datum_get<light_datum>(light_data, light_index);
    s_nondeterministic_light_data *render = &g_nondeterministic_light_data[datum_index_to_absolute_index(light_index)];
    lights_game_globals_definition *globals = (lights_game_globals_definition *)lights_game_globals;
    bool result = globals->enabled && (light->flags & 2);
    result &= light->bounding_sphere_radius > 0.0001f;
    result &= light->color.red > 0.08f || light->color.green > 0.08f || light->color.blue > 0.08f;
    result &= render->specular_fade > 0.0f;
    result &= !(light->flags & 0x20) || inl_cinematic_in_progress();
    if (result)
    {
        light_definition *definition = (light_definition *)tag_get('ligh', light->definition_index);
        bool location_valid = lights_location_valid(&light->location) || ((definition->flags & 4) && (definition->flags & 0x80));
        unsigned long definition_flags = definition->flags;
        result &= location_valid;
        result &= (definition->flags & 0x40) || lights_allow_dynamic_lighting();
        result &= !(definition_flags & 0x10) || tls_player_mapping_globals()->local_player_count == 1;
        result &= !(definition_flags & 0x200) || globals->can_use_flashlights;
        long object_index;
        if (light->attachment_object_index == -1)
            object_index = light->owner_object_index;
        else
            object_index = light->attachment_object_index;
        bool first_person = false;
        if (player_index != -1 && object_index != -1)
            first_person = render_object_is_first_person(player_index, object_index);
        if (((definition_flags & 4) && !first_person) || ((definition_flags & 8) && first_person))
            return false;
        return result;
    }
    return result;
}

t_light_first_data_allocator g_light_first_data_allocator;
t_light_data_reference_allocator g_light_data_reference_allocator;
t_light_cluster_reference_allocator g_light_cluster_reference_allocator;
t_light_data_allocator g_light_data_allocator;
t_light_render_data_allocator g_light_render_data_allocator;
t_lights_game_globals_allocator g_lights_game_globals_allocator;
