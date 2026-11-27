#ifndef SAVED_GAMES_GAME_STATE_H
#define SAVED_GAMES_GAME_STATE_H

#include "shell/shell.h"
#include "saved_games/game_state_lock.h"
#include "saved_games/game_state_procs_types.h"

// Functions defined in tu/blamlibxbox/game_state.cpp.

class c_restricted_memory;

// Game-state proc listener of the async save buffer (slot 3, vtable 0x3cb88).
class c_game_state_save_listener : public c_game_state_proc_listener
{
public:
    virtual void set_mode(long mode, s_game_state_buffer_description *description);
    virtual void update(void *);
    virtual void dispose(void *);
};

// Game-state proc listener of the slot load buffer (slot 4, vtable 0x3cb94).
class c_game_state_load_listener : public c_game_state_proc_listener
{
public:
    virtual void set_mode(long mode, s_game_state_buffer_description *description);
    virtual void update(void *);
    virtual void dispose(void *);
};

// Game-state proc listener of the compressed game-state buffer (slot 5, vtable 0x3ae74).
class c_game_state_compressor_listener : public c_game_state_proc_listener
{
public:
    virtual void set_mode(long mode, s_game_state_buffer_description *description);
    virtual void update(void *);
    virtual void dispose(void *);
};

class c_game_state_compressor
{
public:
    bool game_state_decompress_buffer_and_load(unsigned long buffer_size, void *destination, unsigned long *destination_size,
        long capacity, long scratch_size, void *scratch_buffer);
};
struct s_game_state_header;

void game_state_save();
void game_state_prepare_for_revert();
void game_state_set_revert_time(long revert_time);
bool game_state_header_matches(const s_game_state_header *current_header, s_game_state_header *file_header);
void game_state_shell_dispose(void);
void game_state_revert(unsigned long flags);
void game_state_shell_initialize(void);
extern "C" void game_state_write_scratch_file(const char *name);
extern "C" bool game_state_verify_hash(s_game_state_header *external_header, bool allow_sentinel);
extern "C" void game_state_prepare_for_load(int arg);
extern "C" unsigned char game_state_prepare_game_from_header(s_game_state_header *header, bool keep_session, bool change_game, bool *game_changed);

struct game_state_header;

extern "C" void game_state_allocate_buffer(void);
extern "C" bool game_state_saved_game_header_valid(const game_state_header *header);
extern "C" void game_state_update_hash(unsigned char clear_header, void *address);
extern "C" bool game_state_saved_game_matches_session(const game_state_header *header);
extern "C" int game_state_compute_hash_async(void);
extern "C" void game_state_reset_async_state(void);
extern "C" void game_state_handle_null_callback(void);
extern "C" void game_state_update_async_save(void *);
extern "C" void game_state_wait_for_async_save(void *buffer);
extern "C" void game_state_save_async(int p0);
extern "C" bool game_state_load_from_slot(long index, unsigned long flags);
extern "C" void game_state_handle_coop_load(void);

struct game_options;

bool game_state_read_game_options_from_persistent_storage_blocking(e_controller_index controller_index, game_options *options);
void game_state_try_and_load_from_persistent_storage(e_controller_index controller_index);


// Shared declarations used by game_state.cpp.
struct s_game_state_memory_mapping;
class c_restricted_memory_callbacks;
extern "C" s_game_state_memory_mapping g_game_state_async_file_state;
extern c_game_state_save_listener g_game_state_save_listener;
extern c_game_state_load_listener g_game_state_load_listener;
struct s_game_state_shared_lock;

extern "C" s_game_state_shared_lock g_game_state_shared_lock;
extern "C" void (*g_game_state_load_callbacks[5])(long);
extern "C" c_game_state_compressor_listener g_game_state_compressor_listener;

class c_synchronized_long;
class c_restricted_section;
extern c_restricted_section g_restricted_section[];

void game_state_initialize();
void game_state_dispose();
void game_state_initialize_for_new_map();
void game_state_dispose_from_old_map();
void game_state_initialize_for_new_structure_bsp(unsigned long bsp_index);
void game_state_dispose_from_old_structure_bsp(unsigned long structure_bsp_mask);
struct s_game_non_bsp_zone_set;
void game_state_initialize_for_new_designer_zone_set(const s_game_non_bsp_zone_set *new_zone_set);
void game_state_dispose_from_old_designer_zone_set(const s_game_non_bsp_zone_set *old_zone_set);

#endif // SAVED_GAMES_GAME_STATE_H
