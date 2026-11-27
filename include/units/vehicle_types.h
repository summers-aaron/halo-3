#ifndef UNITS_VEHICLE_TYPES_H
#define UNITS_VEHICLE_TYPES_H

enum e_vehicle_type {
    _vehicle_type_human_tank = 0,
    _vehicle_type_human_jeep = 1,
    _vehicle_type_human_boat = 2,
    _vehicle_type_alien_scout = 3,
    _vehicle_type_alien_fighter = 4,
    _vehicle_type_turret = 5,
    _vehicle_type_mantis = 6,
    _vehicle_type_vtol = 7,
    _vehicle_type_chopper = 8,
    _vehicle_type_guardian = 9,
    k_vehicle_type_count = 10,
    _vehicle_type_none = 11
};

typedef char vehicle_types_size_enum[(sizeof(e_vehicle_type) == 4) ? 1 : -1];

#endif // UNITS_VEHICLE_TYPES_H
