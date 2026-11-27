// ailibxbox/sector_environment_objects.obj: from the records 0x56a108 0x56a538
// @flags /TP /O2 /Oc
#include "math/real_math_types.h"
#include "objects/object_iterator.h"
#include "objects/object_types.h"
#include "objects/object_datum.h"
#include "memory/data_types.h"
#include "tag_files/tag_groups.h"
#include "ai/sector_environment_objects.h"
// @address 0x56a108 ?find_pathfinding_objects@@YAJPAUscenario@@FPAVc_pathfinding_object@@J_N@Z

#include "scenario/scenario_tag.h"
#include "objects/object_reference_types.h"
#include "ai/scenario_object_reference_types.h"
#include "objects/object_type_definition.h"
#include "scenario/scenario_objects.h"
#include "objects/object_header.h"
#include "scenario/scenario.h"

const float sector_neg_max = -3.4028234663852886e+38f;
const float sector_pos_max = 3.4028234663852886e+38f;






static __forceinline void pathfinding_object_initialize(c_pathfinding_object *object,
    short type, short index, short structure_bsp_index)
{
    float upper = sector_neg_max;
    object->type = type;
    object->index = index;
    object->structure_bsp_index = structure_bsp_index;
    object->object_index = -1;
    object->unknown16 = -1;
    object->unknown20 = -1;
    object->unknown52 = 0;
    object->unknown56 = -1;
    object->flags = 3;
    object->location_valid = 0;
    object->unknown60 = 0;
    object->unknown64 = 0;
    object->unknown24 = 0;
    object->bounds.n[0].lower = sector_pos_max;
    object->bounds.n[0].upper = upper;
    object->bounds.n[1].lower = sector_pos_max;
    object->bounds.n[1].upper = sector_neg_max;
    object->bounds.n[2].lower = sector_pos_max;
    object->bounds.n[2].upper = sector_neg_max;
}


long find_pathfinding_objects(scenario *scenario, short, c_pathfinding_object *pathfinding_object, long, bool)
{
    short total = 0;
    short per_type = 0;
    c_object_iterator<object_datum> iterator;
    long found;

    if (scenario->machines.count > 0) {
        do {
            if (total >= 4000) {
                break;
            }
            pathfinding_object_initialize(&pathfinding_object[total], 2, per_type, 0);
            total = (short)(total + 1);
            per_type = (short)(per_type + 1);
        } while (per_type < scenario->machines.count);
    }

    iterator.begin(128, 0);
    if (iterator.next()) {
        do {
            object_datum *datum = iterator.get_datum();
            found = -1;
            if (datum->identifier.find_scenario_object_from_scenario(global_scenario, &found) != 0) {
                short index = (short)found;
                if (index < total) {
                    c_pathfinding_object *object = &pathfinding_object[index];
                    object->object_index = iterator.get_index();
                    object->location_valid = (datum->location.cluster_reference.bsp_index + 1 != 0);
                }
            }
        } while (iterator.next());
    }

    short base = total;
    per_type = 0;
    if (scenario->crates.count > 0) {
        do {
            if (total >= 4000) {
                break;
            }
            pathfinding_object_initialize(&pathfinding_object[total], 3, per_type, 0);
            total = (short)(total + 1);
            per_type = (short)(per_type + 1);
        } while (per_type < scenario->crates.count);
    }

    iterator.begin(0x400, 0);
    if (iterator.next()) {
        short limit = total;
        do {
            if (limit >= 4000) {
                break;
            }
            object_datum *datum = iterator.get_datum();
            found = -1;
            if (datum->identifier.find_scenario_object_from_scenario(global_scenario, &found) != 0) {
                short index = (short)(base + found);
                if (index < limit) {
                    c_pathfinding_object *object = &pathfinding_object[index];
                    object->object_index = iterator.get_index();
                    object->location_valid = (datum->location.cluster_reference.bsp_index + 1 != 0);
                }
            }
        } while (iterator.next());
    }

    base = total;
    per_type = 0;
    if (scenario->giants.count > 0) {
        do {
            if (total >= 4000) {
                break;
            }
            pathfinding_object_initialize(&pathfinding_object[total], 4, per_type, 0);
            total = (short)(total + 1);
            per_type = (short)(per_type + 1);
        } while (per_type < scenario->giants.count);
    }

    iterator.begin(0x1000, 0);
    if (iterator.next()) {
        short limit = total;
        do {
            if (limit >= 4000) {
                break;
            }
            object_datum *datum = iterator.get_datum();
            found = -1;
            if (datum->identifier.find_scenario_object_from_scenario(global_scenario, &found) != 0) {
                short index = (short)(base + found);
                if (index < limit) {
                    c_pathfinding_object *object = &pathfinding_object[index];
                    object->object_index = iterator.get_index();
                    object->location_valid = (datum->location.cluster_reference.bsp_index + 1 != 0);
                }
            }
        } while (iterator.next());
    }

    return total;
}

static __forceinline s_tag_block *sector_environment_objects_identifier_scenario_datums(long object_type, long *datum_size)
{
    // match: 0x56a538: range assumption keeps scaled-index load in r9 in object_type_definitions[object_type]
    __assume(object_type >= 0 && object_type < 14);
    object_type_definition *const *slot = &object_type_definitions[object_type];
    object_type_definition *definition = *slot;
    long offset = (short)definition->scenario_datums_offset;
    long size = definition->scenario_datum_size;
    if (datum_size) *datum_size = size;
    return (s_tag_block *)((char *)global_scenario + offset);
}

static __forceinline s_scenario_object *sector_environment_objects_find_scenario_object(const c_object_identifier *identifier, long *scenario_datum_index)
{
    s_scenario_object *result = 0;
    long source = identifier->m_source;
    if (source == 1 || source == 0) {
        if (object_type_definitions[identifier->m_type]->scenario_datums_offset != -1) {
            long datum_size = 0;
            s_tag_block *datums = sector_environment_objects_identifier_scenario_datums(identifier->m_type, &datum_size);
            for (long index = 0; index < datums->count; index++) {
                s_scenario_object *scenario_object = (s_scenario_object *)tag_block_get_element_with_size(datums, index, datum_size);
                if (scenario_object->object_identifier.is_equal(identifier)) {
                    result = scenario_object;
                    if (scenario_datum_index)
                        *scenario_datum_index = index;
                    break;
                }
            }
        }
    }
    return result;
}

// @address 0x56a538 ?pathfinding_object_get_object_ref_index@@YAFJF@Z
short pathfinding_object_get_object_ref_index(long object_index, short node_index)
{
    short result = -1;
    object_datum *object = object_get(object_index);
    s_scenario_object *scenario_object = sector_environment_objects_find_scenario_object(&object->identifier, 0);
    if (scenario_object) {
        c_typed_tag_block<s_scenario_object_reference> *block = 0;
        switch (object->identifier.get_type()) {
        case _object_type_scenery:
            block = &((s_scenario_scenery *)scenario_object)->object_references;
            break;
        case _object_type_crate:
            block = &((s_scenario_crate *)scenario_object)->object_references;
            break;
        case _object_type_machine:
            block = &((s_scenario_machine *)scenario_object)->object_references;
            break;
        case _object_type_giant:
            block = &((s_scenario_giant *)scenario_object)->object_references;
            break;
        default:
            break;
        }
        if (block && node_index >= 0 && node_index < block->count()) {
            s_scenario_object_reference *reference = TAG_BLOCK_GET_ELEMENT(block, node_index, s_scenario_object_reference);
            result = reference->index;
        }
    }
    return result;
}
