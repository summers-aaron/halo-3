#ifndef UNITS_UNIT_SEAT_HELPERS_H
#define UNITS_UNIT_SEAT_HELPERS_H

#include "units/units.h"
#include "units/unit_seat_source.h"

namespace {
static __forceinline short unit_get_all_seats(long unit_index, unit_seat_source *seats, short maximum_count)
{
    short count = 0;
    ::unit_get_all_seats_internal(unit_index, seats, &count, maximum_count, 0);
    return count;
}
}

#endif // UNITS_UNIT_SEAT_HELPERS_H
