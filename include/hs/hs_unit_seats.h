#ifndef HS_HS_UNIT_SEATS_H
#define HS_HS_UNIT_SEATS_H

#include "memory/static_arrays.h"

// Functions defined in tu/blamlibxbox/hs_unit_seats.cpp that other units call.

struct unit_seat_source;
enum e_vehicle_seat_desire;
enum e_unit_seat_occupancy_type;

// Element of scenario::seat_mappings (12 bytes): hs_match_unit_seat_internal reads the unit definition at +0 and
// tests the 64 seat bits at +4. March 2007 names the type (hs_compile.obj s_hs_unit_seat_mapping::s_hs_unit_seat_mapping).
struct s_hs_unit_seat_mapping
{
    long unit_definition_tag_index;
    c_static_flags_no_init<64> unit_seats;
};
typedef char hs_unit_seats_size_unit_seat_mapping[(sizeof(s_hs_unit_seat_mapping) == 12) ? 1 : -1];

bool hs_match_unit_seat_internal(long unit_index,long seat_index,long seat_set);

long hs_get_unit_seats(long unit_index, long seat_set, e_vehicle_seat_desire seat_desire,
    e_unit_seat_occupancy_type occupancy_type, unit_seat_source *sources, long maximum_count);

#endif // HS_HS_UNIT_SEATS_H
