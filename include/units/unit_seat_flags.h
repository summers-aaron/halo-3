#ifndef UNITS_UNIT_SEAT_FLAGS_H
#define UNITS_UNIT_SEAT_FLAGS_H

enum e_unit_seat_flags
{
    _unit_seat_driver_bit = 2,
    _unit_seat_gunner_bit = 3,
    _unit_seat_boarding_seat_bit = 11,
    _unit_seat_boarding_must_open_target_seat_bit = 13,
    _unit_seat_leader_bit = 21,
    _unit_seat_hide_inventory_bit = 30
};

#endif // UNITS_UNIT_SEAT_FLAGS_H
