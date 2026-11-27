#ifndef AI_SECTOR_H
#define AI_SECTOR_H

#include "ai/pathfinding_data.h"
#include "ai/pathfinding_geometry.h"
#include "ai/pathfinding_sector_types.h"
#include "ai/sector_ref.h"
#include "math/real_math_globals.h"
#include "tag_files/tag_groups.h"

// Functions defined in tu/ailibxbox/sector.cpp.

struct pathfinding_data;

pathfinding_data *pathfinding_data_get(short structure_bsp_index);
unsigned short runtime_cw_link(const pathfinding_data *data, long link_index, long sector_index);
bool sector_broken(short structure_bsp_index, pathfinding_data *data, pathfinding_sector *sector);

// Data defined in tu/ailibxbox/sector.cpp that other units use.

class c_sector_ref;
union real_point3d;
union real_vector3d;

void get_sector_flags_normal(long flags, real_vector3d *normal);

bool sector_from_point_and_sector(const real_point3d *point, c_sector_ref sector,
    const real_point3d *other_point, c_sector_ref *other_sector);

extern c_sector_ref g_sector_none;

struct runtime_link_iterator;
struct sector_link;

sector_link *runtime_link_iterator_next(runtime_link_iterator *iterator);

static __forceinline pathfinding_sector *pathfinding_get_sector(const pathfinding_data *data, short sector_index)
{
    return &((pathfinding_sector *)data->sectors.address)[sector_index];
}

static __forceinline void get_sector_normal(const pathfinding_sector *sector, real_vector3d *normal, bool use_up)
{
    if (use_up && (sector->flags & 0x1000))
        *normal = *global_up3d;
    else
        get_sector_flags_normal(sector->flags, normal);
}

static __forceinline pathfinding_sector *pathfinding_get_sector(c_sector_ref sector_ref)
{
    return pathfinding_get_sector(pathfinding_data_get(sector_ref.structure_bsp_index), sector_ref.sector_index);
}

static __forceinline unsigned short runtime_ccw_link(const pathfinding_data *data, long link_index, long sector_index)
{
    const sector_link *link = TAG_BLOCK_GET_ELEMENT(&data->links, link_index, sector_link);
    return link->links[link->sectors[1] == sector_index];
}

#endif // AI_SECTOR_H
