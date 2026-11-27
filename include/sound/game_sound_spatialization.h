#ifndef SOUND_GAME_SOUND_SPATIALIZATION_H
#define SOUND_GAME_SOUND_SPATIALIZATION_H

#include <stddef.h>
#include "memory/static_arrays.h"
#include "structures/structure_bsp_tag.h"
#include "tag_files/tag_groups.h"

struct real_decibel;
struct real_matrix4x3;
struct s_cluster_reference;

struct s_scenario_acoustics_environment
{
    s_tag_reference sound_environment;
    float cutoff_distance;
    float interpolation_speed;
};

struct s_scenario_acoustics_ambience
{
    s_tag_reference background_sound;
    s_tag_reference inside_cluster_sound;
    float cutoff_distance;
    unsigned long scale_flags;
    float interior_scale;
    float portal_scale;
    float exterior_scale;
    float interpolation_speed;
};

struct s_scenario_acoustics_palette_entry
{
    long name;
    s_scenario_acoustics_environment reverb;
    s_scenario_acoustics_ambience ambience;
};

typedef char scenario_acoustics_palette_entry_reverb_cutoff_offset[
    (sizeof(void *) != 4 || offsetof(s_scenario_acoustics_palette_entry, reverb.cutoff_distance) == 0x14) ? 1 : -1];
typedef char scenario_acoustics_palette_entry_ambience_offset[
    (sizeof(void *) != 4 || offsetof(s_scenario_acoustics_palette_entry, ambience) == 0x1c) ? 1 : -1];
typedef char scenario_acoustics_palette_entry_ambience_cutoff_offset[
    (sizeof(void *) != 4 || offsetof(s_scenario_acoustics_palette_entry, ambience.cutoff_distance) == 0x3c) ? 1 : -1];
typedef char scenario_acoustics_palette_entry_size[
    (sizeof(void *) != 4 || sizeof(s_scenario_acoustics_palette_entry) == 0x54) ? 1 : -1];

struct s_structure_bsp_sound_cluster_definition
{
    long palette_index;
    s_tag_block enclosing_portal_designators;
    s_tag_block interior_cluster_indices;
};

typedef char structure_bsp_sound_cluster_portals_offset[
    (sizeof(void *) != 4 || offsetof(s_structure_bsp_sound_cluster_definition, enclosing_portal_designators) == 0x4) ? 1 : -1];
typedef char structure_bsp_sound_cluster_size[
    (sizeof(void *) != 4 || sizeof(s_structure_bsp_sound_cluster_definition) == 0x1c) ? 1 : -1];

// Scenario acoustics palette accessors. game_sound_spatialization.cpp instantiates its cluster-attribute
// search over them (they are template arguments of the two game_sound_find_cluster_attributes instances).

const s_tag_reference &get_acoustic_ambience_background_sound(const s_scenario_acoustics_palette_entry *entry);
float get_acoustic_ambience_background_cutoff(const s_scenario_acoustics_palette_entry *entry);
const s_tag_reference &get_acoustic_reverb(const s_scenario_acoustics_palette_entry *entry);
float get_acoustic_reverb_cutoff(const s_scenario_acoustics_palette_entry *entry);

template <typename t_entry, const s_tag_reference &(*get_sound)(const t_entry *), float (*get_cutoff)(const t_entry *),
    long k_palette_index_offset, long k_palette_count_offset, long k_sound_cluster_offset, long k_sound_clusters_offset>
class c_game_sound_structure_palette_helper
{
public:
    static __forceinline const s_tag_block *get_palette(const structure_bsp *bsp)
    {
        return &bsp->acoustics_palette;
    }
    static __forceinline const s_tag_block *get_sound_clusters(const structure_bsp *bsp)
    {
        return &bsp->acoustics_sound_clusters;
    }
    static __forceinline short get_palette_index(const structure_cluster *cluster)
    {
        return cluster->acoustics;
    }
    static __forceinline short get_sound_cluster_index(const structure_cluster *cluster)
    {
        return cluster->acoustics_sound_cluster_index;
    }
    static __forceinline float get_cutoff_distance(const structure_bsp *bsp, long index)
    {
        const t_entry *entries = static_cast<const t_entry *>(get_palette(bsp)->address);
        return entries[index].ambience.cutoff_distance;
    }
};

typedef c_game_sound_structure_palette_helper<s_scenario_acoustics_palette_entry,
    get_acoustic_ambience_background_sound, get_acoustic_ambience_background_cutoff, 28, 264, 30, 588> c_ambience_background_helper;
typedef c_game_sound_structure_palette_helper<s_scenario_acoustics_palette_entry,
    get_acoustic_reverb, get_acoustic_reverb_cutoff, 28, 264, 30, 588> c_reverb_helper;

template <typename t_helper, long k_maximum_count>
void game_sound_find_cluster_attributes(long scenario_index, const s_cluster_reference &cluster_reference,
    const real_matrix4x3 *matrix, long, float,
    c_static_array<long, k_maximum_count> *out_palette_indices, c_static_array<real_decibel, k_maximum_count> *out_gains,
    c_static_array<long, k_maximum_count> *out_portal_indices, c_static_array<long, k_maximum_count> *out_cluster_indices,
    long *out_count);

template <>
void game_sound_find_cluster_attributes<c_reverb_helper, 2>(long scenario_index, const s_cluster_reference &cluster_reference,
    const real_matrix4x3 *matrix, long, float,
    c_static_array<long, 2> *out_palette_indices, c_static_array<real_decibel, 2> *,
    c_static_array<long, 2> *out_portal_indices, c_static_array<long, 2> *out_cluster_indices,
    long *out_count);

bool game_sound_distance_compare(long index_a, long index_b, const void *distances);

extern float k_game_sound_spatialization_golden_ratio;
extern float g_game_sound_spatialization_golden_ratio;

#endif // SOUND_GAME_SOUND_SPATIALIZATION_H
