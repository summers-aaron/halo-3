#ifndef AI_SECTOR_REF_H
#define AI_SECTOR_REF_H

#include <stddef.h>

// Four-byte sector reference.
// valid() at RVA 0xb36b8 sign-extends the BSP halfword at +0 and zero-extends
// the sector halfword at +2. g_sector_none stores 0xffff in both halves.
//
// Methods are the March 2007 debug build's (`marchbody class c_sector_ref`: 8 methods, none with an
// assert, so the header path and method order are the tree's). Field names are not in the debug map.
class c_sector_ref
{
public:
    short structure_bsp_index;
    unsigned short sector_index;

    // March 0x8277fe10: stores -1 to both halfwords.
    __forceinline void clear() {
        structure_bsp_index = -1;
        sector_index = 0xffff;
    }

    // March 0x82eb6358: the sector argument is tested against -1 and against 0xffff, then clear().
    __forceinline void set(short bsp_index, unsigned short index) {
        if (index == -1 || index == 0xffff) {
            clear();
        } else {
            structure_bsp_index = bsp_index;
            sector_index = index;
        }
    }

    // March 0x82e7bdb8.
    __forceinline bool operator==(const c_sector_ref &other) const {
        return structure_bsp_index == other.structure_bsp_index && sector_index == other.sector_index;
    }

    // March 0x82eb63c8: its own expression, not !operator==.
    __forceinline bool operator!=(const c_sector_ref &other) const {
        return structure_bsp_index != other.structure_bsp_index || sector_index != other.sector_index;
    }

    // March 0x827e4958: the long packs the BSP index in the high half (srawi 16) and the sector in the
    // low half (clrlwi 16); callers pass NONE.
    __forceinline bool operator==(long value) const {
        return structure_bsp_index == (value >> 16) && sector_index == (value & 0xffff);
    }

    // March 0x82e9a5a0.
    __forceinline bool operator!=(long value) const {
        return structure_bsp_index != (value >> 16) || sector_index != (value & 0xffff);
    }

    // Returns the pathfinding data for this reference's BSP.
    __forceinline struct pathfinding_data *pathfinding_data() {
        struct pathfinding_data *pathfinding_data_get(short structure_bsp_index);
        return pathfinding_data_get(structure_bsp_index);
    }

    // Defined out of line.
    bool valid() const;
};

typedef char sector_ref_size_must_be_4[(sizeof(c_sector_ref) == 4) ? 1 : -1];
typedef char sector_ref_alignment_must_be_2[(__alignof(c_sector_ref) == 2) ? 1 : -1];
typedef char sector_ref_bsp_offset_must_be_0[((offsetof(c_sector_ref, structure_bsp_index)) == 0) ? 1 : -1];
typedef char sector_ref_sector_offset_must_be_2[((offsetof(c_sector_ref, sector_index)) == 2) ? 1 : -1];

#endif // AI_SECTOR_REF_H
