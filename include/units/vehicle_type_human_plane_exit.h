#ifndef UNITS_VEHICLE_TYPE_HUMAN_PLANE_EXIT_H
#define UNITS_VEHICLE_TYPE_HUMAN_PLANE_EXIT_H

union real_point3d;
extern "C" void vehicle_type_human_plane_find_exit_point(long vehicle_index, float distance, float height, float radius,
    short seat_type, real_point3d *point, bool unused);

#endif // UNITS_VEHICLE_TYPE_HUMAN_PLANE_EXIT_H
