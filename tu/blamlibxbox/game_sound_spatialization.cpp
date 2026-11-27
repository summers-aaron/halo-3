// blamlibxbox/game_sound_spatialization.obj: from the records 0x127618 0x127df0 0x28e050
// @flags /TP /O2 /Oc
#include "cseries/cseries.h"
#include "cseries/long_sort.h"
#include "structures/structure_bsp_tag.h"
#include "tag_files/tag_groups.h"
#include "objects/object_header.h"
#include "math/matrix_math.h"
#include "structures/structures.h"
#include "structures/structure_audibility.h"
#include "sound/game_sound_definitions.h"
#include "scenario/scenario_structure_bsp_reference.h"
#include "cache/cache_files.h"
#include "sound/game_sound_spatialization.h"
#include "scenario/scenario_tag.h"
#include "structures/structure_cluster_portal.h"

bool game_sound_distance_compare(long index_a, long index_b, const void *distances)
{
    return static_cast<const float *>(distances)[index_a] > static_cast<const float *>(distances)[index_b];
}

// @rdata 0x8202490c k_game_sound_normalize_epsilon
static const float k_game_sound_normalize_epsilon = 0.0001f;

static __forceinline float game_sound_normalize_and_clamp_linear(float lower, float upper, float value, bool reverse)
{
    float normalized;
    if (fabs(lower - upper) >= k_game_sound_normalize_epsilon)
        normalized = (value - lower) / (upper - lower);
    else
        normalized = ((value < lower) == reverse) ? 0.0f : 1.0f;
    return (normalized > 0.0f ? normalized : 0.0f) < 1.0f ? (normalized > 0.0f ? normalized : 0.0f) : 1.0f;
}

static const real_decibel k_minimum_gain_db = { -64.0f };
static const real_decibel k_zero_gain_db = { 0.0f };

static __forceinline float game_sound_ease_in_ease_out(float ratio)
{
    float square = ratio * ratio;
    float result = square * 3.0f - square * ratio * 2.0f;
    return PIN(result, 0.0f, 1.0f);
}
static __forceinline float game_sound_interpolate_gain(float lower, float upper, float fraction)
{
    return (upper - lower) * fraction + lower;
}

// @address 0x127618 ??$game_sound_find_cluster_attributes@V?$c_game_sound_structure_palette_helper@Us_scenario_acoustics_palette_entry@@$1?get_acoustic_ambience_background_sound@@YAABUs_tag_reference@@PBU1@@Z$1?get_acoustic_ambience_background_cutoff@@YAM0@Z$0BM@$0BAI@$0BO@$0CEM@@@$0EA@@@YAXJABUs_cluster_reference@@PBUreal_matrix4x3@@JMPAV?$c_static_array@J$0EA@@@PAV?$c_static_array@Ureal_decibel@@$0EA@@@22PAJ@Z
template <typename t_helper, long k_maximum_count>
void game_sound_find_cluster_attributes(long scenario_index, const s_cluster_reference &cluster_reference,
    const real_matrix4x3 *matrix, long, float,
    c_static_array<long, k_maximum_count> *out_palette_indices, c_static_array<real_decibel, k_maximum_count> *out_gains,
    c_static_array<long, k_maximum_count> *out_portal_indices, c_static_array<long, k_maximum_count> *out_cluster_indices,
    long *out_count)
{
    c_static_flags<k_maximum_count> visited;
    visited.clear();
    c_static_array<float, k_maximum_count> distances;
    c_static_array<long, k_maximum_count> portal_indices;
    c_static_array<long, k_maximum_count> sorted_indices;
    c_static_array<long, k_maximum_count> cluster_indices;
    const real_point3d *listener = &matrix->position;
    const scenario *game_scenario = static_cast<const scenario *>(tag_get('scnr', scenario_index));
    const scenario_structure_bsp_reference *bsp_reference = static_cast<const scenario_structure_bsp_reference *>(
        tag_block_get_element_with_size(
            &game_scenario->structure_bsps, cluster_reference.bsp_index, sizeof(scenario_structure_bsp_reference)));
    const structure_bsp *bsp = static_cast<const structure_bsp *>(
        tag_get('sbsp', bsp_reference->structure_bsp.index));
    long cluster_index = cluster_reference.cluster_index;
    const structure_cluster *cluster = static_cast<const structure_cluster *>(
        tag_block_get_element_with_size(&bsp->clusters, cluster_index, sizeof(structure_cluster)));
    short palette_index = c_ambience_background_helper::get_palette_index(cluster);
    if (palette_index != -1)
    {
        portal_indices[palette_index] = -1;
        cluster_indices[palette_index] = cluster_index;
        distances[palette_index] = 0.0f;
        visited.set(palette_index, true);
    }
    float best_distance = 3.4028234663852886e+38f;
    short sound_cluster_index = c_ambience_background_helper::get_sound_cluster_index(cluster);
    const s_tag_block *sound_clusters = c_ambience_background_helper::get_sound_clusters(bsp);
    const s_structure_bsp_sound_cluster_definition *sound_entry = static_cast<const s_structure_bsp_sound_cluster_definition *>(
        tag_block_get_element_with_size(
            sound_clusters, sound_cluster_index, sizeof(s_structure_bsp_sound_cluster_definition)));
    for (long i = 0; i < sound_entry->enclosing_portal_designators.count; ++i)
    {
        // match: 0x127618: const reference binding preserves temporary lifetime
        const short *const &references = static_cast<const short *>(sound_entry->enclosing_portal_designators.address);
        long portal_ref = references[i];
        unsigned short portal_index = portal_ref & 0x7fff;
        const cluster_portal *portal = static_cast<const cluster_portal *>(
            tag_block_get_element_with_size(&bsp->cluster_portals, portal_index, sizeof(cluster_portal)));
        long other_cluster = portal->cluster_indices[((unsigned long)portal_ref >> 15) & 1];
        const structure_cluster *other = static_cast<const structure_cluster *>(
            tag_block_get_element_with_size(&bsp->clusters, other_cluster, sizeof(structure_cluster)));
        short other_palette = c_ambience_background_helper::get_palette_index(other);
        if ((portal->flags & (1 << 5)) == 0 && (portal->flags & (1 << 3)) == 0 && other_palette != palette_index)
        {
            float audibility = structure_audibility_get_cluster_distance(
                bsp,
                cluster_index, other_cluster);
            float portal_distance = structure_distance_to_portal(bsp, portal, listener);
            portal_distance += audibility;
            if (portal_distance <= best_distance && palette_index != -1)
            {
                portal_indices[palette_index] = portal_index;
                best_distance = portal_distance;
            }
            if (other_palette != -1)
            {
                float cutoff = c_ambience_background_helper::get_cutoff_distance(bsp, other_palette);
                if (audibility <= cutoff)
                {
                    float previous_distance;
                    if (visited.test(other_palette))
                        previous_distance = distances[other_palette];
                    else
                        previous_distance = cutoff;
                    if (portal_distance <= previous_distance)
                    {
                        distances[other_palette] = portal_distance;
                        portal_indices[other_palette] = portal_index;
                        cluster_indices[other_palette] = other_cluster;
                        visited.set(other_palette, true);
                    }
                }
            }
        }
    }
    const s_tag_block *palette = c_ambience_background_helper::get_palette(bsp);
    for (long i = 0; i < palette->count; ++i)
    {
        sorted_indices[i] = i;
        if (!visited.test(i))
            distances[i] = 3.4028234663852886e+38f;
    }
    if ((unsigned long)palette->count >= 2)
        templated_sort<long>::qsort(sorted_indices.get_elements(), palette->count, game_sound_distance_compare, distances.get_elements());
    if (out_count)
        *out_count = 0;
    long k = 0;
    long output_count = MIN(palette->count, out_palette_indices->get_count());
    if (output_count > 0)
    {
        do
        {
            long entry_palette = sorted_indices[k];
            if (visited.test(entry_palette))
            {
                (*out_palette_indices)[k] = entry_palette;
                if (out_gains)
                {
                    if (entry_palette != -1)
                    {
                        float cutoff = c_ambience_background_helper::get_cutoff_distance(bsp, entry_palette);
                        if (!(cutoff > 0.001f))
                            cutoff = 0.001f;
                        float fraction = game_sound_normalize_and_clamp_linear(cutoff, 0.0f, distances[entry_palette], false);
                        fraction = PIN(fraction, 0.0f, 1.0f);
                        real_decibel lower_db = k_minimum_gain_db;
                        float lower_gain = (float)exp(lower_db.value * 0.115129255f);
                        real_decibel upper_db = k_zero_gain_db;
                        float upper_value = upper_db.value;
                        float upper_gain = (float)exp(upper_value * 0.115129255f);
                        float curve = game_sound_ease_in_ease_out(fraction);
                        (*out_gains)[k] = real_gain_to_decibels(game_sound_interpolate_gain(lower_gain, upper_gain, curve));
                    }
                    else
                        (*out_gains)[k] = k_minimum_gain_db;
                }
                if (out_portal_indices)
                    (*out_portal_indices)[k] = portal_indices[entry_palette];
                if (out_cluster_indices)
                    (*out_cluster_indices)[k] = cluster_indices[entry_palette];
                if (out_count)
                    (*out_count)++;
            }
            else
            {
                if (out_count)
                    return;
                (*out_palette_indices)[k] = -1;
                if (out_gains)
                    (*out_gains)[k] = k_minimum_gain_db;
                if (out_portal_indices)
                    (*out_portal_indices)[k] = -1;
                if (out_cluster_indices)
                    (*out_cluster_indices)[k] = -1;
            }
            ++k;
        } while (k < output_count);
    }
}

template void game_sound_find_cluster_attributes<c_ambience_background_helper, 64>(long, const s_cluster_reference &,
    const real_matrix4x3 *, long, float, c_static_array<long, 64> *, c_static_array<real_decibel, 64> *,
    c_static_array<long, 64> *, c_static_array<long, 64> *, long *);

static __forceinline float game_sound_reverb_cutoff_distance(const structure_bsp *bsp, long index)
{
    const s_scenario_acoustics_palette_entry *entries
        = static_cast<const s_scenario_acoustics_palette_entry *>(c_reverb_helper::get_palette(bsp)->address);
    return entries[index].reverb.cutoff_distance;
}

// @rdata 0x82021694 k_game_sound_maximum_distance
static const float k_game_sound_maximum_distance = 3.4028234663852886e+38f;
// @address 0x127df0 ??$game_sound_find_cluster_attributes@V?$c_game_sound_structure_palette_helper@Us_scenario_acoustics_palette_entry@@$1?get_acoustic_reverb@@YAABUs_tag_reference@@PBU1@@Z$1?get_acoustic_reverb_cutoff@@YAM0@Z$0BM@$0BAI@$0BO@$0CEM@@@$01@@YAXJABUs_cluster_reference@@PBUreal_matrix4x3@@JMPAV?$c_static_array@J$01@@PAV?$c_static_array@Ureal_decibel@@$01@@22PAJ@Z
template <>
void game_sound_find_cluster_attributes<c_reverb_helper, 2>(long scenario_index, const s_cluster_reference &cluster_reference,
    const real_matrix4x3 *matrix, long, float,
    c_static_array<long, 2> *out_palette_indices, c_static_array<real_decibel, 2> *,
    c_static_array<long, 2> *out_portal_indices, c_static_array<long, 2> *out_cluster_indices,
    long *out_count)
{
    c_static_flags<64> visited;
    visited.clear();
    c_static_array<float, 64> distances;
    c_static_array<long, 64> portal_indices;
    c_static_array<long, 64> sorted_indices;
    c_static_array<long, 64> cluster_indices;
    const real_point3d *listener = &matrix->position;
    const scenario *game_scenario = static_cast<const scenario *>(tag_get('scnr', scenario_index));
    const scenario_structure_bsp_reference *bsp_reference = static_cast<const scenario_structure_bsp_reference *>(
        tag_block_get_element_with_size(
            &game_scenario->structure_bsps, cluster_reference.bsp_index, sizeof(scenario_structure_bsp_reference)));
    const structure_bsp *bsp = static_cast<const structure_bsp *>(
        tag_get('sbsp', bsp_reference->structure_bsp.index));
    long cluster_index = cluster_reference.cluster_index;
    const structure_cluster *cluster = static_cast<const structure_cluster *>(
        tag_block_get_element_with_size(&bsp->clusters, cluster_index, sizeof(structure_cluster)));
    short palette_index = c_reverb_helper::get_palette_index(cluster);
    if (palette_index != -1)
    {
        portal_indices[palette_index] = -1;
        cluster_indices[palette_index] = cluster_index;
        distances[palette_index] = 0.0f;
        visited.set(palette_index, true);
    }
    const float *maximum_distance = &k_game_sound_maximum_distance;
    float best_distance = *maximum_distance;
    const long &sound_cluster_index = c_reverb_helper::get_sound_cluster_index(cluster);
    const s_tag_block *sound_clusters = c_reverb_helper::get_sound_clusters(bsp);
    const s_structure_bsp_sound_cluster_definition *sound_entry = static_cast<const s_structure_bsp_sound_cluster_definition *>(
        tag_block_get_element_with_size(
            sound_clusters, (short)sound_cluster_index, sizeof(s_structure_bsp_sound_cluster_definition)));
    for (long i = 0; i < sound_entry->enclosing_portal_designators.count; ++i)
    {
        long portal_ref = *static_cast<const short *>(
            tag_block_get_element_with_size(&sound_entry->enclosing_portal_designators, i, sizeof(short)));
        unsigned short portal_index = portal_ref & 0x7fff;
        const cluster_portal *portal = static_cast<const cluster_portal *>(
            tag_block_get_element_with_size(&bsp->cluster_portals, portal_index, sizeof(cluster_portal)));
        long other_cluster = portal->cluster_indices[((unsigned long)portal_ref >> 15) & 1];
        const structure_cluster *other = static_cast<const structure_cluster *>(
            tag_block_get_element_with_size(&bsp->clusters, other_cluster, sizeof(structure_cluster)));
        short other_palette = c_reverb_helper::get_palette_index(other);
        if ((portal->flags & (1 << 5)) == 0 && (portal->flags & (1 << 3)) == 0 && other_palette != palette_index)
        {
            float audibility = structure_audibility_get_cluster_distance(
                bsp,
                cluster_index, other_cluster);
            float portal_distance = structure_distance_to_portal(bsp, portal, listener);
            portal_distance += audibility;
            if (portal_distance <= best_distance && palette_index != -1)
            {
                portal_indices[palette_index] = portal_index;
                best_distance = portal_distance;
            }
            if (other_palette != -1)
            {
                float cutoff = game_sound_reverb_cutoff_distance(bsp, other_palette);
                if (audibility <= cutoff)
                {
                    float limit = visited.test(other_palette) ? distances[other_palette] : cutoff;
                    if (portal_distance <= limit)
                    {
                        distances[other_palette] = portal_distance;
                        portal_indices[other_palette] = portal_index;
                        cluster_indices[other_palette] = other_cluster;
                        visited.set(other_palette, true);
                    }
                }
            }
        }
    }
    const s_tag_block *palette = c_reverb_helper::get_palette(bsp);
    for (long i = 0; i < palette->count; ++i)
    {
        sorted_indices[i] = i;
        if (!visited.test(i))
            distances[i] = *maximum_distance;
    }
    if ((unsigned long)palette->count >= 2)
        templated_sort<long>::qsort(sorted_indices.get_elements(), palette->count, game_sound_distance_compare, distances.get_elements());
    if (out_count)
        *out_count = 0;
    long k = 0;
    long output_count = MIN(palette->count, out_palette_indices->get_count());
    if (output_count > 0)
    {
        do
        {
            long entry_palette = sorted_indices[k];
            if (visited.test(entry_palette))
            {
                (*out_palette_indices)[k] = entry_palette;
                if (out_portal_indices)
                    (*out_portal_indices)[k] = portal_indices[entry_palette];
                if (out_cluster_indices)
                    (*out_cluster_indices)[k] = cluster_indices[entry_palette];
                if (out_count)
                    (*out_count)++;
            }
            else
            {
                if (out_count)
                    return;
                (*out_palette_indices)[k] = -1;
                if (out_portal_indices)
                    (*out_portal_indices)[k] = -1;
                if (out_cluster_indices)
                    (*out_cluster_indices)[k] = -1;
            }
            ++k;
        } while (k < output_count);
    }
}

float k_game_sound_spatialization_golden_ratio = 1.618034f;
float g_game_sound_spatialization_golden_ratio = k_game_sound_spatialization_golden_ratio;
