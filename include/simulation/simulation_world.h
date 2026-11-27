#ifndef SIMULATION_SIMULATION_WORLD_H
#define SIMULATION_SIMULATION_WORLD_H

// Functions defined in tu/networkinglibxbox/simulation_world.cpp.

class c_simulation_world;
class c_simulation_player;
struct s_machine_identifier;

extern "C" c_simulation_player *simulation_world_find_player(c_simulation_world *world, const s_machine_identifier *identifier, long user_index);

extern "C" void simulation_world_update_views_with_message(c_simulation_world *world, long message_a, long message_b, long message_c);
extern "C" void simulation_world_set_out_of_sync(c_simulation_world *world, bool determinism_failure);
extern "C" int simulation_world_handle_synchronous_update_ack(c_simulation_world *world, unsigned long mode, long received_time, long update_number);
extern "C" int simulation_world_compute_frame_rate_tier(void);
extern "C" long simulation_world_get_available_updates(c_simulation_world *world, bool *match_remote_time, long *queued_updates);

class c_simulation_view;
struct s_simulation_update_metadata;
struct simulation_update;

extern "C" void simulation_world_update_synchronous_views(c_simulation_world *world, const simulation_update *update,
    const s_simulation_update_metadata *metadata);
extern "C" void simulation_world_update_disconnected(c_simulation_world *world);
extern "C" void simulation_world_update_view_establishment(c_simulation_world *world, c_simulation_view *view);
extern "C" void simulation_world_update_all_view_establishments(c_simulation_world *world);
extern "C" void simulation_world_update_active(c_simulation_world *world);
extern "C" bool simulation_world_handle_synchronous_update(c_simulation_world *world, const simulation_update *update,
    const s_simulation_update_metadata *metadata);

void simulation_world_build_network_status(long *values);

extern "C" bool simulation_world_test_all_client_views_active(c_simulation_world *world);

#endif // SIMULATION_SIMULATION_WORLD_H
