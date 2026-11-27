#ifndef SIMULATION_SIMULATION_H
#define SIMULATION_SIMULATION_H

// Functions defined in tu/networkinglibxbox/simulation.cpp.

class c_network_channel;
class c_simulation_view;
struct s_simulation_player_netdebug_data;
struct s_simulation_update_metadata;
struct simulation_update;

void simulation_initialize(void);
void simulation_dispose();
void simulation_initialize_for_new_map(void);
void simulation_initialize_for_saved_game(long flags);
void simulation_dispose_from_old_map();
bool simulation_starting_up(void);
bool simulation_aborted();
bool simulation_in_progress(void);
void simulation_destroy_update(simulation_update *update);
void simulation_reset_world_for_authority(void);
void simulation_player_joined_game(long player_index);
void simulation_stop(void);
c_simulation_view *simulation_get_remote_view_by_channel(c_network_channel *channel);
void simulation_build_player_updates(void);
void simulation_notify_channel_closure(void *channel);
void simulation_remove_view_from_world(c_simulation_view *view);
void simulation_apply_before_game(const struct simulation_update *update);
bool simulation_update_player_netdebug_data(long player_index, s_simulation_player_netdebug_data *netdebug_data);

class c_bitstream;
class c_simulation_world;
struct s_machine_identifier;

extern "C" bool simulation_is_pending_start(void);
extern "C" void simulation_build_player_and_actor_update(c_simulation_world *world, simulation_update *update);
extern "C" char *static_string_append_bounded(char *string, const char *source);
extern "C" c_bitstream *bitstream_initialize(c_bitstream *bitstream, const unsigned char *data, long data_size);
extern "C" void simulation_delete_world_player(long player_index);
extern "C" bool simulation_get_machine_bandwidth_events(const s_machine_identifier *machine);
extern "C" unsigned long simulation_player_left_game(const c_simulation_world *w);
extern "C" unsigned char simulation_film_read_updates(long maximum_updates, long *updates_read);

struct s_simulation_connection_statistics;

void simulation_prepare_to_load_saved_game(long);
bool simulation_film_start_recording();
extern "C" bool simulation_compute_connection_rating(long index, const s_simulation_connection_statistics *statistics, long *result);
bool simulation_film_retrieve_updates(const simulation_update *update, s_simulation_update_metadata *);

extern "C" bool g_simulation_authority_enabled;

struct s_simulation_distributed_view;
extern "C" long network_shared_memory_new_view(c_simulation_view **, s_simulation_distributed_view **);
#endif // SIMULATION_SIMULATION_H
