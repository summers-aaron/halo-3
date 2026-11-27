// blamlibxbox/hs_unit_seats.obj: from the records 0x577d68 0x577e80
// @flags /TP /O2 /Oc
#include "memory/static_arrays.h"
// @address 0x577d68 ?hs_get_unit_seats@@YAJJJW4e_vehicle_seat_desire@@W4e_unit_seat_occupancy_type@@PAUunit_seat_source@@J@Z
// @address 0x577e80 ?hs_match_unit_seat_internal@@YA_NJJJ@Z
#include <ppcintrinsics.h>
#include <math.h>
#include <stdlib.h>
#include "units/units.h"
#include "units/unit_seat_source.h"
#include "units/unit_seat_helpers.h"
#include "hs/hs_unit_seats.h"
#include "scenario/scenario_tag.h"
#include "scenario/scenario.h"
#include "objects/object_header.h"
#include "tag_files/tag_groups.h"

bool hs_match_unit_seat_internal(long unit_index, long seat_index, long seat_set)
{
    bool result = false;
    object_datum *unit = (object_datum *)object_get_and_verify_type(unit_index, 8195);
    long unit_definition_index = unit->definition;
    if (seat_set == NONE)
    {
        result = true;
    }
    else
    {
        long mapping_start_index = seat_set & 0xffff;
        long mapping_count = (unsigned long)seat_set >> 16;
        for (long mapping_offset = 0; mapping_offset < mapping_count; mapping_offset++)
        {
            const s_hs_unit_seat_mapping *mapping = TAG_BLOCK_GET_ELEMENT(&global_scenario->seat_mappings,
                mapping_start_index + mapping_offset, const s_hs_unit_seat_mapping);
            if (mapping->unit_definition_tag_index == unit_definition_index)
            {
                result = mapping->unit_seats.test(seat_index);
                break;
            }
        }
    }
    return result;
}

long hs_get_unit_seats(long unit_index, long seat_set, e_vehicle_seat_desire seat_desire,
    e_unit_seat_occupancy_type occupancy_type, unit_seat_source *sources, long maximum_count)
{
    long source_count = 0;
    const long maximum_seat_count = 64;
    unit_seat_source seats[maximum_seat_count];
    short seat_count = unit_get_all_seats(unit_index, seats, maximum_seat_count);
    for (long seat_index = 0; seat_index < seat_count && source_count < maximum_seat_count; seat_index++)
    {
        const unit_seat_source *seat = &seats[seat_index];
        if (hs_match_unit_seat_internal(seat->unit_index, seat->seat_index, seat_set))
        {
            bool add_seat;
            switch (occupancy_type)
            {
            case _unit_seat_occupancy_occupied:
                add_seat = unit_seat_get_occupant(seat->unit_index, seat->seat_index) != NONE;
                break;
            case _unit_seat_occupancy_empty:
                add_seat = unit_seat_get_occupant(seat->unit_index, seat->seat_index) == NONE;
                break;
            default:
                add_seat = true;
                break;
            }
            if (add_seat)
                sources[source_count++] = *seat;
        }
    }
    return source_count;
}
