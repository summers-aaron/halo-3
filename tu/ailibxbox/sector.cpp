// ailibxbox/sector.obj: from the records 0x9e680 0x11f788 0x11f818 0x50b318
// @flags /TP /O2 /Oc
#include "tag_files/tag_types.h"
#include "structures/structure_runtime_resources.h"
// @address 0x9e680 ?pathfinding_data_get@@YAPAUpathfinding_data@@F@Z
// @address 0x11f788 ?runtime_cw_link@@YAGPBUpathfinding_data@@JJ@Z
// @address 0x50b318 ?sector_broken@@YA_NFPAUpathfinding_data@@PAUpathfinding_sector@@@Z
// @address 0x11f818 ?runtime_link_iterator_next@@YAPAUsector_link@@PAUruntime_link_iterator@@@Z

#include "ai/sector_ref.h"
#include "ai/sector.h"
#include "physics/breakable_surfaces.h"
#include "scenario/scenario_structure_bsp_reference.h"
#include "scenario/scenario_tag.h"
#include "structures/structure_bsp_tag.h"
extern "C"
{
    struct scenario;
}
#include "scenario/scenario.h"
#include "tag_files/tag_groups.h"
#include "scenario/scenario_access.h"
#include "ai/pathfinding_geometry.h"
#include "ai/pathfinding_data.h"
#include "ai/pathfinding_sector_types.h"

struct runtime_link_iterator
{
    unsigned short current;
    unsigned short start;
    unsigned short next;
    long sector;
    long link_index;
    unsigned char flag;
    pathfinding_data *data;
    short count;
};

c_sector_ref g_sector_none = {-1, 0xffff};

namespace {
static __forceinline void runtime_link_iterator_new(runtime_link_iterator *iterator, const pathfinding_data *data, long sector_index, long link_index)
{
    iterator->sector = sector_index;
    iterator->current = -1;
    iterator->link_index = -1;
    iterator->next = (unsigned short)link_index;
    iterator->start = (unsigned short)link_index;
    iterator->data = (pathfinding_data *)data;
    iterator->count = 0;
}
}

pathfinding_data *pathfinding_data_get(short structure_bsp_index)
{
    pathfinding_data *result = 0;
    if (structure_bsp_is_active(structure_bsp_index))
    {
        structure_bsp *bsp = global_structure_bsp_get(structure_bsp_index);
        if (bsp->pathfinding_data.count > 0)
            result = (pathfinding_data *)bsp->pathfinding_data.address;
    }
    return result;
}

unsigned short runtime_cw_link(const pathfinding_data *data, long link_index, long sector_index)
{
    long result = -1;
    runtime_link_iterator iterator;
    runtime_link_iterator_new(&iterator, data, sector_index, link_index);
    runtime_link_iterator_next(&iterator);
    while (runtime_link_iterator_next(&iterator)) {
        if (iterator.current == link_index)
            break;
        result = iterator.current;
    }
    return (unsigned short)result;
}

// match: 0x11f818: __restrict hoists the iterator->sector load above the iterator->current store; without it the load sinks below the store
sector_link *runtime_link_iterator_next(runtime_link_iterator *__restrict iterator)
{
    sector_link *link = 0;
    unsigned short next_link_index = iterator->next;
    short count = iterator->count + 1;
    unsigned short current_link_index = next_link_index;
    iterator->count = count;
    if (current_link_index != -1)
    {
        if (current_link_index != 0xffff)
        {
            iterator->current = next_link_index;
            link = &((sector_link *)iterator->data->links.address)[current_link_index];
            unsigned short following_link_index;
            if (link->right_sector == iterator->sector)
            {
                following_link_index = link->reverse_link;
                iterator->link_index = link->start_vertex;
                iterator->flag = 1;
                if (following_link_index == iterator->start)
                    following_link_index = -1;
            }
            else
            {
                following_link_index = link->forward_link;
                iterator->link_index = link->end_vertex;
                iterator->flag = 0;
                if (following_link_index == iterator->start)
                    following_link_index = -1;
            }
            iterator->next = following_link_index;
        }
    }
    if (count > 2000)
        link = 0;
    return link;
}

bool sector_broken(short structure_bsp_index, pathfinding_data *data, pathfinding_sector *sector)
{
    bool result = false;
    short node_index = sector->first_hint_index;
    while (node_index != -1)
    {
        pathfinding_hint *node = &((pathfinding_hint *)data->hints.address)[node_index];
        if (node->type == 7)
        {
            s_structure_bsp_resource *resource = &global_structure_bsp_get(structure_bsp_index)->resource;
            s_structure_bsp_resources *resources;
            if (resource->use_resource_items)
                resources = resource->collision_resource_interface.get_resources();
            else
                resources = (s_structure_bsp_resources *)resource->raw_resources.address;
            collision_bsp *bsp = resources->collision_bsp;
            collision_surface *surface = &((collision_surface *)bsp->surfaces.address)[node->surface_index];
            if (surface->flags & 8)
                result = !breakable_surface_extant(structure_bsp_index, node->variant, surface->breakable_surface, surface->breakable_surface_bit);
            break;
        }
        node_index = node->next_hint_index;
    }
    return result;
}
