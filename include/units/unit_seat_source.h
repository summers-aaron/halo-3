#ifndef UNITS_UNIT_SEAT_SOURCE_H
#define UNITS_UNIT_SEAT_SOURCE_H

#include <stddef.h>

struct unit_seat;

// A seat on a unit: the unit index, the seat index, and the seat definition.
// pointer at +0, +4 and +8; March hs_get_unit_seats indexes 12-byte records.
struct unit_seat_source
{
    long unit_index;
    short seat_index;
    const unit_seat *definition;
};

// March hs_get_unit_seats applies seat-role filtering for 0..3; 4 accepts
// every role. Only the latter value is named by the current script callers.
enum e_vehicle_seat_desire
{
    _vehicle_seat_desire_any = 4
};

enum e_unit_seat_occupancy_type
{
    _unit_seat_occupancy_any = 0,
    _unit_seat_occupancy_occupied = 1,
    _unit_seat_occupancy_empty = 2
};

typedef char unit_seat_source_size[(sizeof(void *) != 4 || sizeof(unit_seat_source) == 12) ? 1 : -1];
typedef char unit_seat_source_index_offset[(offsetof(unit_seat_source, seat_index) == 4) ? 1 : -1];
typedef char unit_seat_source_definition_offset[(sizeof(void *) != 4 || (offsetof(unit_seat_source, definition) == 8)) ? 1 : -1];

#endif // UNITS_UNIT_SEAT_SOURCE_H
