#ifndef AI_PATH_FLYING_H
#define AI_PATH_FLYING_H

// The flying-path areas already evaluated for one firing-position search (ai\path_flying.cpp: March 2007 debug
// flight_evaluation_cache_new and flight_evaluation_cache_add_area; add_area compares the count at +240 with 30).
struct s_flight_evaluation_entry { unsigned short bsp; short area,point,state; };
struct flight_evaluation_cache { s_flight_evaluation_entry entries[30]; short count; };

typedef char flight_evaluation_cache_size_must_be_242[(sizeof(flight_evaluation_cache) == 242) ? 1 : -1];


// Functions defined in tu/ailibxbox/path_flying.cpp.

union real_point3d;
class c_ai_point3d;
struct firing_position_ref;

void flight_evaluation_cache_add_area(flight_evaluation_cache *cache,short zone_index,short area_index);
bool flight_test_points(const real_point3d *start,const real_point3d *end,long first_ignore_object_index,long second_ignore_object_index);
short test_cache_entry_internal(const real_point3d *point,long first_ignore_object_index,short entry_index,flight_evaluation_cache *cache);
bool flight_hint_any_point_visible(long hint_index,const real_point3d *point,long unit_index,flight_evaluation_cache *cache);
bool firing_position_path_3d_available(const real_point3d *point,long unit_index,firing_position_ref reference,flight_evaluation_cache *cache);
bool path_3d_available(const real_point3d *point,const c_ai_point3d *ai_point,long unit_index,firing_position_ref reference,flight_evaluation_cache *cache);

class c_destination_orders;
class c_path_destination;
struct path_result;
bool path_3d_build_path(long actor_index, c_destination_orders *destination_orders, const real_point3d *point,
    const c_path_destination *path_destination, firing_position_ref reference, flight_evaluation_cache *cache,
    long second_ignore_object_index, path_result *result);

#endif
