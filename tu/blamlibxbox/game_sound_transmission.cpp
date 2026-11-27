// blamlibxbox/game_sound_transmission.obj: from the records 0x13b680 0x1fbd20 0x1fb868 0x1fb3d0
#include "sound/game_sound_globals.h"
// @flags /TP /O2 /Oc
#include "tag_files/tag_types.h"
#include "sound/sound_manager_types.h"
#include "structures/structure_portal_reference_types.h"
#include "physics/collision_types.h"
#include "objects/object_reference_types.h"
#include <math.h>
#include "math/real_math.h"
#include "physics/water_physics.h"
#include "structures/structures.h"
#include "sound/game_sound.h"
#include "cache/cache_files.h"
#include "scenario/scenario.h"
#include "scenario/scenario_access.h"
#include "physics/water_physics_types.h"
#include "physics/collision_bsp_types.h"
#include "scenario/scenario_tag.h"
#include "structures/structure_bsp_tag.h"


#include "structures/structure_cluster_portal.h"
#include "scenario/scenario_structure_bsp_reference.h"
#include "physics/collision_test_vector.h"

#include "sound/game_sound_transmission.h"

class c_game_sound_transmission_flags
{
public:
    __forceinline c_game_sound_transmission_flags() : m_bits(0), m_count(0) {}
    __forceinline long count() const { return m_count; }
    __forceinline bool test(long index) const { return (m_bits[index >> 5] & (1 << (index & 31))) != 0; }
    const unsigned long *m_bits;
    unsigned long m_count;
};

// @address 0x13b680 game_sound_transmission_get_cluster_count
extern "C" long game_sound_transmission_get_cluster_count(const game_sound_transmission_map *map)
{
    unsigned long scratch;
    unsigned long *p = &scratch;
    *p = 0;
    if (map->valid == 0)
    {
        return 0;
    }
    return map->valid->room_count;
}

static __forceinline long game_sound_transmission_index_get(const game_sound_transmission_map *map, s_cluster_reference cluster)
{
    long result = -1;
    if (map->valid) result = (short)map->bsps[cluster.bsp_index].cluster_map[cluster.cluster_index];
    return result;
}


// @address 0x1fbd20 compute_sound_portal_propagation
extern "C" void compute_sound_portal_propagation(const game_sound_transmission_map *map, const float *values, const real_point3d *point,
    s_cluster_reference cluster, const real_point3d *point2, s_cluster_reference cluster2, float *output)
{
    float closest_distance;
    s_portal_reference closest_portal = game_sound_find_closest_portal(point, cluster, &closest_distance);
    s_portal_reference portal = closest_portal;
    bool portal_valid = portal.bsp_index != -1;
    if (portal_valid) {
        int transmission_index = game_sound_transmission_index_get(map, cluster2);
        int portal_index = portal.portal_index;
        float value = values[transmission_index];
        const structure_bsp *bsp = global_structure_bsp_get(portal.bsp_index);
        const cluster_portal *cluster_portal_entry = &((const cluster_portal *)bsp->cluster_portals.address)[portal_index];
        float distance = structure_distance_to_portal(bsp, cluster_portal_entry, point2);
        float radius = cluster_portal_entry->bounding_radius;
        float ratio = distance / radius;
        output[1] = value;
        output[0] = (float)(1.0 - 1.0f / (ratio + 1.0f));
    } else {
        output[0] = 1.0f;
        output[1] = 0.0f;
    }
}




static __forceinline bool game_sound_transmission_locations_equal(const s_location *a, const s_location *b)
{
    return *(const long *)a == *(const long *)b;
}

static __forceinline bool game_sound_transmission_cluster_references_equal(const s_cluster_reference *a, const s_cluster_reference *b)
{
    return *(const unsigned short *)a == *(const unsigned short *)b;
}

static __forceinline bool game_sound_transmission_location_valid(const s_location *location)
{
    return (unsigned char)location->cluster_reference.bsp_index != 255;
}

// @address 0x1fb868 game_sound_transmission_compute_source
extern "C" void game_sound_transmission_compute_source(
    const game_sound_transmission_map *map, const float *values, const s_game_sound_transmission_listener *listener, s_game_sound_transmission_source *source)
{
    if (!game_sound_transmission_locations_equal(&listener->location, &source->location))
    {
        float obstruction = 0.0f;
        s_collision_test_flags flags;
        flags.collision_flags = 0x801;
        flags.object_flags = 0;
        real_vector3d vector;
        collision_result collision;
        vector_from_points3d(&listener->position, &source->position, &vector);
        bool clear = !collision_test_vector(flags, false, &listener->position, &vector, -1, -1, -1, &collision);
        if (!source->use_audibility)
        {
            if (!clear)
            {
                real_vector3d reverse_vector;
                negate_vector3d(&vector, &reverse_vector);
                collision_result reverse_collision;
                if (collision_test_vector(flags, false, &source->position, &reverse_vector, -1, -1, -1, &reverse_collision))
                {
                    obstruction = 1.0f - reverse_collision.fraction - collision.fraction;
                    obstruction = PIN(obstruction, 0.0f, 1.0f);
                    if (obstruction > 0.0f)
                    {
                        source->has_obstruction_points = true;
                        point_from_line3d(&listener->position, &vector, collision.fraction, &source->obstruction_point);
                        source->obstruction_fraction = collision.fraction;
                        point_from_line3d(&source->position, &reverse_vector, reverse_collision.fraction, &source->reverse_obstruction_point);
                        source->reverse_obstruction_fraction = reverse_collision.fraction;
                    }
                }
                else
                {
                    obstruction = 1.0f - collision.fraction;
                }
            }
        }
        else
        {
            if (!source->portals_ignored &&
                !game_sound_transmission_cluster_references_equal(
                    &listener->location.cluster_reference, &source->location.cluster_reference) &&
                map)
            {
                float audibility = structure_audibility_get_distance(map, listener->location.cluster_reference, source->location.cluster_reference);
                float distance = sqrt(distance_squared3d(&listener->position, &source->position));
                float total = MAX(distance + audibility, 0.001f);
                float amount = 1.0f - distance / total;
                if (collision.material_index != 0xffff)
                    amount += 0.5f;
                if (amount < 0.1f)
                    obstruction = 0.0f;
                else
                    obstruction = amount - 0.1f;
            }
        }
        if (game_sound_transmission_location_valid(&listener->location) && game_sound_transmission_location_valid(&source->location))
        {
            long index = game_sound_transmission_index_get(map, source->location.cluster_reference);
            if (game_sound_transmission_index_get(map, listener->location.cluster_reference) !=
                game_sound_transmission_index_get(map, source->location.cluster_reference))
            {
                if (source->portals_ignored)
                {
                    source->portal_obstruction = 0.0f;
                    source->water_depth = 0.0f;
                    source->water_material = -1;
                    source->portal_propagation = values[index];
                }
                else
                {
                    float propagation[2];
                    compute_sound_portal_propagation(map, values, &listener->position, listener->location.cluster_reference,
                        &source->position, source->location.cluster_reference, propagation);
                    const structure_bsp *listener_bsp =
                        (const structure_bsp *)global_structure_bsp_get(
                            listener->location.cluster_reference.bsp_index);
                    const structure_bsp *source_bsp =
                        (const structure_bsp *)global_structure_bsp_get(
                            source->location.cluster_reference.bsp_index);
                    long listener_cluster = ((const structure_cluster *)listener_bsp->clusters.address)[
                        listener->location.cluster_reference.cluster_index].acoustics_sound_cluster_index;
                    long source_cluster = ((const structure_cluster *)source_bsp->clusters.address)[
                        source->location.cluster_reference.cluster_index].acoustics_sound_cluster_index;
                    if (listener->location.cluster_reference.bsp_index != source->location.cluster_reference.bsp_index || listener_cluster != source_cluster)
                    {
                        source->portal_obstruction = propagation[0];
                        source->portal_propagation = propagation[1];
                    }
                    else
                    {
                        source->portal_obstruction = 0.0f;
                        source->portal_propagation = propagation[1];
                    }
                }
            }
        }
        source->obstruction += obstruction;
    }
    if (game_sound_transmission_location_valid(&source->location))
    {
        s_water_physics_test_sphere_result water;
        if (water_physics_test_sphere(true, &source->position, 0.0f, &water))
        {
            source->water_material = water.material_type.m_index;
            source->water_depth = water_physics_get_depth_at_point(&source->position);
        }
    }
}


static __forceinline void game_sound_transmission_cluster_door_set(
    const game_sound_transmission_map *map, s_cluster_reference cluster, c_game_sound_transmission_flags *doors)
{
    const scenario_zone_set_audibility *data = map->valid;
    long cluster_index = game_sound_transmission_index_get(map, cluster);
    if (data->door_portal_count > 0)
    {
        doors->m_bits = data->room_door_portal_encoded_pas.begin() + ((data->door_portal_count + 31) >> 5) * cluster_index;
        doors->m_count = data->door_portal_count;
    }
}

static __forceinline bool game_sound_transmission_door_get(
    const game_sound_transmission_map *map, long door_index, long *out_bsp_index, long *out_machine_index)
{
    const scenario_zone_set_audibility *data = map->valid;
    *out_bsp_index = -1;
    *out_machine_index = -1;
    bool found = false;
    for (long range_index = 0; range_index < data->game_portal_to_door_occluder_mappings.count(); range_index++)
    {
        const cache_file_allocation_range *range = &data->game_portal_to_door_occluder_mappings[range_index];
        if (door_index - range->start >= 0 && door_index - range->start < (unsigned long)range->size)
        {
            *out_bsp_index = range_index;
            *out_machine_index = door_index - range->start;
            found = true;
        }
    }
    return found;
}

static __forceinline void game_sound_transmission_door_portal_pas(
    const game_sound_transmission_map *map, long door_index, c_game_sound_transmission_flags *pas0, c_game_sound_transmission_flags *pas1)
{
    const scenario_zone_set_audibility *data = map->valid;
    if (data)
    {
        long first_index = door_index * 2;
        long pas_size = (data->room_count + 31) >> 5;
        const unsigned long *front = data->encoded_door_pas.begin() + first_index * pas_size;
        const unsigned long *back = data->encoded_door_pas.begin() + (first_index + 1) * pas_size;
        pas0->m_bits = front;
        pas0->m_count = data->room_count;
        pas1->m_bits = back;
        pas1->m_count = data->room_count;
    }
}

static __forceinline float game_sound_transmission_door_occlusion(long bsp_index, long machine_index)
{
    long occlusion_index = game_sound_compute_group_item_offset(bsp_index, machine_index);
    return dequantize_real(((unsigned short *)g_game_sound_globals)[occlusion_index + 28], 0.0f, 1.0f, 16, false);
}

// @address 0x1fb3d0 game_sound_transmission_update_sources
extern "C" void game_sound_transmission_update_sources(
    const game_sound_transmission_map *map, const s_game_sound_transmission_listener *listener, s_game_sound_obstruction_parameters *parameters)
{
    if (map->valid && game_sound_transmission_location_valid(&listener->location))
    {
        float transmission[1024];
        for (long i = 0; i < 1024; i++)
            transmission[i] = 0.0f;

        c_game_sound_transmission_flags door_set;
        game_sound_transmission_cluster_door_set(map, listener->location.cluster_reference, &door_set);
        const game_sound_transmission_map *const *table = &map;
        for (long door_index = 0; door_index < door_set.count(); door_index++)
        {
            if (door_set.test(door_index))
            {
                long bsp_index;
                long machine_index;
                if (game_sound_transmission_door_get(*table, door_index, &bsp_index, &machine_index) && bsp_index != -1 && machine_index != -1)
                {
                    c_game_sound_transmission_flags door_pas[2];
                    game_sound_transmission_door_portal_pas(*table, door_index, &door_pas[0], &door_pas[1]);
                    long listener_cluster = game_sound_transmission_index_get(*table, listener->location.cluster_reference);
                    long side = -1;
                    if (door_pas[0].test(listener_cluster))
                        side = 1;
                    else if (door_pas[1].test(listener_cluster))
                        side = 0;
                    if (side != -1)
                    {
                        float occlusion = game_sound_transmission_door_occlusion(bsp_index, machine_index);
                        long cluster_index = 0;
                        long cluster_count = game_sound_transmission_get_cluster_count(map);
                        for (; cluster_index < cluster_count; cluster_index++)
                        {
                            if (cluster_index != listener_cluster && door_pas[side].test(cluster_index))
                            {
                                float value = transmission[cluster_index];
                                transmission[cluster_index] = value > occlusion ? value : occlusion;
                            }
                        }
                    }
                }
            }
        }

        long source_count = parameters->count;
        for (long source_index = 0; source_index < source_count; source_index++)
        {
            s_game_sound_transmission_source *source = &parameters->sources[source_index];
            if (source->listener_index == listener->listener_index)
                game_sound_transmission_compute_source(map, transmission, listener, source);
        }
    }
}

// Address 0x5bec98 holds a bare blr shared by the folded empty tracker callbacks: the impulse channel
// probe is an empty body (the March release map merges it with the other stubs; the four tracker-table
// slots in game_sound.cpp all point at 0x825bec98).
// @address 0x5bec98 ?object_impulse_playing_in_channel@@YAXJJJPAX@Z
void object_impulse_playing_in_channel(long, long, long, void *)
{
}
