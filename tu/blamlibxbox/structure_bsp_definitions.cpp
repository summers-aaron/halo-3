// blamlibxbox/structure_bsp_definitions.obj: from the records 0xd55c8 0xd5a40 0xd5c88 0x1c17c8
// @flags /TP /O2 /Oc
#include "visibility/visibility_portal_activation_types.h"
#include "objects/object_reference_types.h"
#include "memory/tls_globals.h"
#include "ai/actor_globals.h"
#include <string.h>
#include <ppcintrinsics.h>
#include "ai/actors.h"
#include "ai/actor_iterator.h"
#include "game/game.h"
#include "scenario/scenario_pvs.h"
#include "structures/structure_bsp_definitions.h"
#include "cache/cache_files.h"
#include "scenario/scenario.h"
#include "scenario/scenario_pvs_types.h"
#include "structures/cluster_bit_vectors.h"
#include "ai/actor_datum.h"
#include "scenario/scenario_tag.h"
#include "memory/data_types.h"
#include "memory/datum_index.h"
#include "tag_files/tag_types.h"
#include "structures/structure_bsp_tag.h"
#include "structures/structure_cluster_portal.h"
#include "scenario/scenario_structure_bsp_reference.h"
#include "memory/bit_vector_iterator.h"
#include "physics/collision_model_definitions.h"
#include "cache/cache_file_tag_resource_vtable_list.h"

// @address 0x58e5c0 memset

struct structure_bsp_portal_reference {
    short value;
};

// @address 0xd5a40 ?next_index@c_bit_vector_iterator@@QAA_NPAJ@Z
bool c_bit_vector_iterator::next_index(long *out_index)
{
    while (m_word == 0) {
        m_index = m_index + 1;
        if (m_index < m_count) {
            m_word = m_bits[m_index];
            continue;
        } else {
            break;
        }
    }
    if ((short)m_index < (short)m_count) {
        long word = m_word;
        long bit;
        if (word != 0) {
            bit = 0;
            while (((1 << bit) & word) == 0)
                bit = bit + 1;
        } else {
            bit = -1;
        }
        *out_index = bit + ((short)m_index << 5);
        m_word = word & ~(1 << bit);
        return true;
    } else {
        return false;
    }
}

// @address 0xd5c88 structure_bsp_get_actor_clusters
extern "C" bool structure_bsp_get_actor_clusters(long index, unsigned long *bit_vector, long count)
{
    bool result = false;
    memset(bit_vector, 0, ((count + 31) >> 5) * 4);
    if (tls_actor_globals()->enabled) {
        result = true;
        actor_iterator iterator;
        iterator.m_iterator.absolute_index = -1;
        iterator.m_iterator.index = -1;
        iterator.active_only = result;
        iterator.m_iterator.data = tls_actor_data();
        actor_datum *actor = actor_iterator_next(&iterator);
        while (actor != 0) {
            if (actor->position.location.cluster_reference.bsp_index != -1 &&
                actor->position.location.cluster_reference.bsp_index == index &&
                actor->position.location.cluster_reference.cluster_index < count) {
                bit_vector[actor->position.location.cluster_reference.cluster_index >> 5] |=
                    result << (actor->position.location.cluster_reference.cluster_index & 31);
            }
            actor = actor_iterator_next(&iterator);
        }
    }
    return result;
}

static __forceinline structure_bsp *structure_bsp_definitions_get(scenario *scenario, const cache_file_tag_instance *tags, long index)
{
    return (structure_bsp *)tags[datum_index_to_absolute_index(
        ((scenario_structure_bsp_reference *)scenario->structure_bsps.address)[index].structure_bsp.index)].definition;
}
static __forceinline structure_cluster *structure_bsp_definitions_cluster(structure_bsp *bsp, const s_cluster_reference &reference)
{
    return &((structure_cluster *)bsp->clusters.address)[reference.cluster_index];
}
static __forceinline long structure_bsp_definitions_first_bit(unsigned long bits)
{
    return 31 - _CountLeadingZeros(bits & -bits);
}
static __forceinline bool structure_bsp_definitions_portal_active(long bsp_index, long portal_index)
{
    structure_bsp_portal_reference reference;
    reference.value = (short)((portal_index & 0x3ff) | (bsp_index << 10));
    long index = (reference.value << 22) >> 22;
    return (g_active_portal_bitvectors[((reference.value << 16) >> 26) * 16 + (index >> 5)] & (1 << (index & 31))) != 0;
}
static __forceinline void structure_bsp_definitions_intersect(
    const s_game_cluster_bit_vectors *a, const s_game_cluster_bit_vectors *b, s_game_cluster_bit_vectors *out)
{
    for (long bsp = 0; bsp < 16; ++bsp) {
        unsigned long *destination = out->bit_vectors[bsp].m_storage;
        long word = 7;
        do {
            if (destination)
                destination[word] = a->bit_vectors[bsp].m_storage[word] & b->bit_vectors[bsp].m_storage[word];
        } while (--word >= 0);
    }
}
// @address 0xd55c8 ?structure_bsp_compute_cluster_active_pvs@@YAXUs_cluster_reference@@PAUs_game_cluster_bit_vectors@@@Z
void structure_bsp_compute_cluster_active_pvs(s_cluster_reference cluster_reference, s_game_cluster_bit_vectors *pvs)
{
    memset(pvs, 0, sizeof(*pvs));
    if (cluster_reference.bsp_index != -1) {
        long zone_index = global_zone_set_index;
        long scenario_index = global_scenario_index;
        s_scenario_pvs_row row;
        s_game_cluster_bit_vectors potential;
        scenario_zone_set_pvs_get_row(scenario_index, &row, zone_index, cluster_reference, false);
        scenario_zone_set_pvs_write_row(&potential, &row);
        if (structure_bsp_definitions_cluster(structure_bsp_definitions_get(((scenario *)global_scenario),
                                                  ((const cache_file_tag_instance *)global_tag_instances), cluster_reference.bsp_index),
                cluster_reference)
                ->flags &
            2)
        {
            scenario *scenario_definition = global_scenario;
            const cache_file_tag_instance *tags = ((const cache_file_tag_instance *)global_tag_instances);
            scenario_zone_set_pvs_get_row(scenario_index, &row, zone_index, cluster_reference, true);
            scenario_zone_set_pvs_write_row(pvs, &row);
            s_game_cluster_bit_vectors expanded = *pvs;
            union { unsigned long rows[16][16]; long words[256]; } visited;
            memset(&visited, 0, sizeof(visited));
            unsigned long active_mask = g_active_structure_bsp_mask;
            bool changed;
            do {
                long bsp_index = structure_bsp_definitions_first_bit(active_mask);
                changed = false;
                for (; bsp_index != -1; bsp_index = structure_bsp_definitions_first_bit(active_mask & ~((1i64 << (bsp_index + 1)) - 1))) {
                    c_bit_vector_iterator iterator;
                    iterator.m_index = 0;
                    iterator.m_bits = (long *)pvs->bit_vectors[bsp_index].m_storage;
                    iterator.m_word = iterator.m_bits[0];
                    structure_bsp *bsp = structure_bsp_definitions_get(scenario_definition, tags, bsp_index);
                    iterator.m_count = (short)((bsp->clusters.count + 31) >> 5);
                    long cluster_index;
                    while (iterator.next_index(&cluster_index)) {
                        structure_cluster *cluster = &((structure_cluster *)bsp->clusters.address)[cluster_index];
                        long portal_count = cluster->portal_indices.count;
                        if (portal_count > 0) {
                        long word = 0;
                        short *portal_indices = (short *)cluster->portal_indices.address;
                        long bsp_word_offset = bsp_index << 4;
                        unsigned long remaining = portal_count;
                        do {
                            long portal_index = *portal_indices;
                            word = portal_index >> 5;
                            unsigned long mask = 1 << (portal_index & 31);
                            if (!(visited.words[bsp_word_offset + word] & mask)) {
                                cluster_portal *portal = &((cluster_portal *)bsp->cluster_portals.address)[portal_index];
                                if ((portal->flags & 4) && structure_bsp_definitions_portal_active(bsp_index, portal_index)) {
                                    s_cluster_reference front, back;
                                    front.bsp_index = (signed char)bsp_index;
                                    front.cluster_index = (unsigned char)portal->cluster_indices[0];
                                    back.bsp_index = (signed char)bsp_index;
                                    back.cluster_index = (unsigned char)portal->cluster_indices[1];
                                    s_scenario_pvs_row back_row;
                                    scenario_zone_set_pvs_get_row(scenario_index, &row, zone_index, front, true);
                                    scenario_zone_set_pvs_get_row(scenario_index, &back_row, zone_index, back, true);
                                    s_game_cluster_bit_vectors front_pvs, back_pvs;
                                    scenario_zone_set_pvs_write_row(&front_pvs, &row);
                                    scenario_zone_set_pvs_write_row(&back_pvs, &back_row);
                                    game_clusters_or(&front_pvs, &back_pvs, &front_pvs);
                                    structure_bsp_definitions_intersect(&front_pvs, &potential, &front_pvs);
                                    game_clusters_or(&front_pvs, &expanded, &expanded);
                                }
                                unsigned long *portal_words = visited.rows[bsp_index];
                                portal_words[word] |= mask;
                            }
                            ++portal_indices;
                        } while (--remaining != 0);
                        }
                    }
                }
                if (memcmp(&expanded, pvs, sizeof(expanded)) != 0) {
                    *pvs = expanded;
                    changed = true;
                }
            } while (changed);
        } else {
            *pvs = potential;
        }
    }
}

// @address 0x1c17c8 ?structure_bsp_tag_resources_postprocess@@YA_NJPAX@Z
bool structure_bsp_tag_resources_postprocess(long tag_index, void *resources)
{
    return collision_model_instance_list_fixup(tag_index, resources);
}

// @address 0x745d78 ?scenario_structure_bsp_cache_file_tag_resource_vtable@@3Us_cache_file_tag_resource_vtable@@A
s_cache_file_tag_resource_vtable scenario_structure_bsp_cache_file_tag_resource_vtable = {
    { { 0x72d3359d, 0x5bcf4e28, 0xa26904a5, 0xd923e60c } },
    structure_bsp_tag_resources_postprocess,
    0
};

// @rdata 0x82035954..0x8203596c
static const s_cache_file_tag_resource_vtable k_null_cache_file_tag_resource_vtable = { 0 };
// @rdata-end

// @address 0x745d90 ?g_cache_file_tag_resource_vtable_list@@3PAPBUs_cache_file_tag_resource_vtable@@A
const s_cache_file_tag_resource_vtable *g_cache_file_tag_resource_vtable_list[] = {
    &scenario_structure_bsp_cache_file_tag_resource_vtable,
    &k_null_cache_file_tag_resource_vtable
};
