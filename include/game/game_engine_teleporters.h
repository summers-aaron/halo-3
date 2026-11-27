#ifndef GAME_GAME_ENGINE_TELEPORTERS_H
#define GAME_GAME_ENGINE_TELEPORTERS_H

// Functions defined in tu/blamlibxbox/game_engine_teleporters.cpp.

class c_teleporter_area;
union real_point3d;

void teleporter_update(class c_teleporter_area * _sym_p0, long player_index, long mode);
void game_engine_teleporters_update();
extern "C" void teleporter_send_blocked_event(unsigned int blocked_player_index);
long get_teleporter_recharge_time_ticks(void);
c_teleporter_area *teleporter_try_and_get_from_object_index(long object_index);
extern "C" bool teleporter_are_points_linked(const real_point3d *sender_point, const real_point3d *receiver_point);
extern "C" float teleporter_is_channel_linked(long object_index);
extern "C" unsigned char teleporter_area_is_blocked(c_teleporter_area *area, long unused);
extern "C" bool game_engine_sandbox_exit_edit_mode(long player_index);
extern "C" void teleporter_update_players_in_area(c_teleporter_area *area);
void teleporter_update_projectiles(c_teleporter_area *area);
void teleporter_update_vehicles(c_teleporter_area *area);
class c_teleporter_area * teleporter_try_and_get_random_available_receiver(class c_teleporter_area * sender, long object_index, long * blocked_ticks);
extern "C" bool teleporter_has_receiver_on_channel(c_teleporter_area *area);
bool teleporter_can_pass_object(c_teleporter_area *area, long object_index);
extern "C" void teleporter_compute_destination(c_teleporter_area *sender, c_teleporter_area *receiver, long object_index, real_point3d *destination);
extern "C" bool teleporter_is_teleport_blocked(c_teleporter_area *sender, c_teleporter_area *area, long object_index);
void teleporter_flag_object_as_having_teleported_recursive(long object_index);
extern "C" unsigned char teleporter_teleport_object(c_teleporter_area *area, c_teleporter_area *receiver, long object_index);

extern "C" bool teleporter_is_sender(const c_teleporter_area *area);

#endif // GAME_GAME_ENGINE_TELEPORTERS_H
