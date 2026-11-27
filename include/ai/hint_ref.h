#ifndef AI_HINT_REF_H
#define AI_HINT_REF_H

#include <stddef.h>

struct pathfinding_data;
struct pathfinding_hint_data;

// March owns these six methods; none asserts a header path. Kept beside sector_ref.h.
class c_hint_ref
{
public:
    short structure_bsp_index;
    short hint_index;

    // March 82e7be40: each halfword is cleared independently.
    __forceinline void clear()
    {
        structure_bsp_index = -1;
        hint_index = -1;
    }

    // March 82ed0680.
    __forceinline bool operator==(const c_hint_ref &other) const
    {
        return structure_bsp_index == other.structure_bsp_index && hint_index == other.hint_index;
    }

    // March 82f1d160.
    __forceinline struct pathfinding_data *pathfinding_data()
    {
        struct pathfinding_data *pathfinding_data_get(short structure_bsp_index);
        return pathfinding_data_get(structure_bsp_index);
    }

    bool valid() const;

    // March 82f61960: pathfinding_data_get, then tag_block_get_element_with_size on hints.
    // Its body waits for the shared pathfinding_hint_data definition.
    struct pathfinding_hint_data *get_hint();

    // March 82f619c8.
    __forceinline void set(short bsp_index, short index)
    {
        structure_bsp_index = bsp_index;
        hint_index = index;
    }
};

typedef char hint_ref_size[(sizeof(c_hint_ref) == 4) ? 1 : -1];
typedef char hint_ref_alignment[(__alignof(c_hint_ref) == 2) ? 1 : -1];
typedef char hint_ref_hint_offset[((offsetof(c_hint_ref, hint_index)) == 2) ? 1 : -1];

#endif // AI_HINT_REF_H
