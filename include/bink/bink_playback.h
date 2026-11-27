#ifndef BINK_BINK_PLAYBACK_H
#define BINK_BINK_PLAYBACK_H

// Functions defined in tu/blamlibxbox/bink_playback.cpp.

bool netdebug_initialize(const char *project_name, const char *build_version, const char *system_name);

union rectangle2d;

extern "C" bool bink_playback_is_playing(void);
extern "C" void bink_playback_render_first_frame(void);
extern "C" void bink_playback_stop_and_cleanup(void);
extern "C" void bink_playback_play_by_name(long index);
extern "C" void bink_playback_play_by_tag_index(long index);
extern "C" bool predict_bink_movie_fullpath(const char *path, long index, unsigned long flags);
extern "C" void bink_prediction_update(void);
extern "C" bool bink_is_idle_and_drive_empty(void);
extern "C" void bink_playback_start_movie(unsigned int flags);
extern "C" void bink_playback_play_idle_movie(void);
extern "C" void bink_playback_render_rect(const rectangle2d *rect);
extern "C" void bink_playback_update(void);

enum e_map_memory_configuration;
struct _bink_globals;
struct s_bink_prediction_message;
struct s_bink_runtime;

extern "C" void bink_playback_cancel_prediction(void);
extern "C" void bink_playback_set_recording_enabled(e_map_memory_configuration configuration);
extern "C" void bink_playback_stop_movie(void);
void bink_free(void *memory_block);
void *bink_alloc(unsigned int size);
bool bink_thread_get_prediction_message(s_bink_runtime *runtime, s_bink_prediction_message *message);
extern "C" unsigned long bink_xmp_query_drive_empty(int *empty);
extern "C" unsigned long bink_xmp_send_message(unsigned long message, void *parameter0, void *parameter1);
extern "C" void bink_playback_build_log_filename(const char *);
bool prepare_bink_movie_for_playback_async(s_bink_runtime *runtime, const s_bink_prediction_message *message);
extern "C" void bink_playback_render_centered_frame(void);
extern "C" void bink_playback_update_frame(long unused);
extern "C" void bink_playback_bind_frame_textures(long index);
extern "C" void bink_playback_abort_movie(void);

// Data defined in tu/blamlibxbox/bink_playback.cpp that other units use.
extern _bink_globals bink_globals;

void bink_playback_initialize();
void bink_playback_dispose(void);
extern "C" void bink_playback_clear_playing(void);
void bink_playback_dispose_from_old_map();
extern "C" void bink_playback_handle_after_load(unsigned long, unsigned long load_flags);

#endif // BINK_BINK_PLAYBACK_H
