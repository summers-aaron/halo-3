// blamlibxbox/structure_audibility.obj: from the records 0xfb4d0 0xfb5b0 0x127b38 0x290230
// @flags /TP /O2 /Oc
// @address 0xfb4d0 structure_get_cluster_ai_deafening
// @address 0xfb5b0 structure_audibility_get_distance
// @address 0x290230 ?structure_audibility_is_valid@@YA_NPBUstructure_bsp@@@Z

#include "structures/structure_audibility.h"
#include "structures/structure_bsp_tag.h"
#include "math/integer_math.h"
extern template unsigned long count_bits<unsigned long>(unsigned long);
#include "scenario/scenario_tag.h"
#include "scenario/scenario.h"

extern "C" {
unsigned char structure_get_cluster_ai_deafening(s_structure_cluster_mapping *t, s_cluster_reference a, s_cluster_reference b)
{
    bool result = false;
    const scenario_zone_set_audibility *audibility = t->audibility;
    if (audibility != 0) {
        int first_cluster = t->bsp_clusters[a.bsp_index].values[a.cluster_index];
        int second_cluster = t->bsp_clusters[b.bsp_index].values[b.cluster_index];
        if (first_cluster != second_cluster) {
            const int *bits = static_cast<const int *>(audibility->ai_deafening_pas.address);
            int lo = (first_cluster > second_cluster) ? second_cluster : first_cluster;
            int hi = (first_cluster > second_cluster) ? first_cluster : second_cluster;
            int idx = (audibility->room_count - 1) * lo - ((lo + 1) * lo) / 2 + hi - 1;
            result = ((1 << (idx & 31)) & bits[idx >> 5]) != 0;
        }
    }
    return result;
}
}

static __forceinline float structure_audibility_lookup(s_structure_cluster_mapping *mapping,
    s_cluster_reference first_reference, s_cluster_reference second_reference, const scenario_zone_set_audibility *audibility)
{
    float result = 0.0f;
        int a = mapping->bsp_clusters[first_reference.bsp_index].values[first_reference.cluster_index];
        int b = mapping->bsp_clusters[second_reference.bsp_index].values[second_reference.cluster_index];
        result = a != b ? 255.0f : 0.0f;
        if (a != b) {
            int lower = a > b ? b : a;
            int upper = a > b ? a : b;
            unsigned char value = static_cast<const unsigned char *>(audibility->room_distances.address)
                [(audibility->room_count - 1) * lower - (lower + 1) * lower / 2 + upper - 1];
            if (value == 0) {
                result = 0.0f;
            } else if (value == 255) {
                result = 256.0f;
            } else {
                float minimum = audibility->room_distance_bounds.lower;
                float maximum = audibility->room_distance_bounds.upper;
                float fraction = (float)(value - 1) * 0.0039525693f;
                result = fraction * (maximum - minimum) + minimum;
            }
        }
    return result;
}
float structure_audibility_get_distance(s_structure_cluster_mapping *mapping, unsigned short first_reference, unsigned short second_reference)
{
    const scenario_zone_set_audibility *audibility = mapping->audibility;
    union { unsigned short packed; s_cluster_reference reference; } a = {first_reference}, b = {second_reference};
    float result = 0.0f;
    if (audibility != 0) result = structure_audibility_lookup(mapping, a.reference, b.reference, audibility);
    return result;
}

// @address 0x127b38 structure_audibility_get_cluster_distance
extern "C" float structure_audibility_get_cluster_distance(const structure_bsp *structure, int a, int b)
{
    float result = 0.0f;
    float unset = a != b ? 255.0f : 0.0f;
    if (a == b) return unset;
    if (structure_audibility_is_valid(structure)) {
        const structure_audibility *audibility =
            static_cast<const structure_audibility *>(structure->audibility.address);
        int lower = a > b ? b : a;
        int upper = a > b ? a : b;
        unsigned char value = static_cast<const unsigned char *>(audibility->cluster_distances.address)
            [(structure->clusters.count - 1) * lower - (lower + 1) * lower / 2 + upper - 1];
        if (value != 0) {
            if (value == 255) return 256.0f;
            float minimum = audibility->cluster_distance_bounds.lower;
            float maximum = audibility->cluster_distance_bounds.upper;
            float fraction = (float)(value - 1) * 0.0039525693f;
            return fraction * (maximum - minimum) + minimum;
        }
    }
    return result;
}

// @address 0x7cff08 ?global_scenario@@3PAUscenario@@A
// @address 0x8a1ecc ?g_active_structure_bsp_mask@@3KA

bool structure_audibility_is_valid(const structure_bsp *structure)
{
    return (global_scenario == 0 || global_scenario->structure_bsps.count == 1 ||
               count_bits<unsigned long>(g_active_structure_bsp_mask) == 1) &&
        structure->audibility.count > 0;
}
