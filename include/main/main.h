#ifndef MAIN_MAIN_H
#define MAIN_MAIN_H

// Functions defined in tu/blamlibxbox/main.cpp.

void publish_waiting_gamestate();
#include "main/main_render_resources.h"
void process_published_game_state(bool value);
void main_switch_zone_set(long zone_set_index);
void main_reset_zone_requests(long zone_set_index);
unsigned char main_kick_startup_masking_sequence(unsigned char use_alpha_texture);
bool main_startup_sequence(void);
void __tls_set_g_main_gamestate_timing_data_allocator(void *allocator);
void __tls_set_g_main_render_timing_data_allocator(void *allocator);
void main_loop_body();
void main_loop();
void main_halt_and_display_errors();

struct s_main_block_writer;

extern "C" unsigned char main_has_pending_state_changes(void);
extern "C" void main_set_status_value(const char *status_type, const char *format, ...);
#include "main/main_idle.h"
extern "C" void main_set_designer_zone_active(int bit_index);
void main_set_cinematic_zone_inactive(int bit_index);
extern "C" void main_set_cinematic_zone_active(int bit_index);
void main_set_active_zone_set(int actor_index);
extern "C" void main_crash_render_message(const char *text);
extern "C" int main_write_memory_list_stream(s_main_block_writer *writer);
extern "C" int main_write_minidump_memory_list(s_main_block_writer *writer);
extern "C" int main_write_minidump_memory64_list(s_main_block_writer *writer);
extern "C" int main_write_minidump_memory_list_excluding_sections(s_main_block_writer *writer);
extern "C" void main_update_idle_tasks();
extern "C" void main_halt_and_spin_forever(void);
extern "C" long main_exception_handle_crash(void);

struct s_main_stream_block;

extern "C" void main_set_render_timing_data(void *address);
void main_thread_combine_timing_data(void *address);
extern "C" const char *main_get_exception_name(unsigned long status);
extern "C" void main_crash_handle_minidump_upload(bool minidump_success, void *context);
extern "C" int main_crash_update_user_input_file();
extern "C" void main_crash_set_user_input(const char *source, long size);
extern "C" bool main_test_range_excluded(s_main_block_writer *writer, unsigned long start, long length);
extern "C" void main_encode_stream_block(s_main_stream_block *data);
extern "C" void main_encode_pair_entry(unsigned __int64 *data);
extern "C" void main_update_profile_rank_grade(void);
extern "C" void main_update_campaign_achievements(void);
extern "C" bool main_encode_block_range(s_main_block_writer *writer, const char *memory, long length);
void main_loop_exit();
void main_loop_enter();
extern "C" void main_render_loading_frame();
void main_loop_process_global_state_changes();
extern "C" bool main_write_minidump_streams(s_main_block_writer *writer);
struct s_thread_exception_record;
extern "C" long main_write_crash_minidump(unsigned long flags, const char *path, long exception_code, s_thread_exception_record *exception_record);
extern "C" void main_demand_update();

// Data defined in tu/blamlibxbox/main.cpp that other units use.

class c_interlocked_long;

extern c_interlocked_long g_render_thread_enabled;

extern "C" void main_reset_saved_film_state(void);
#endif // MAIN_MAIN_H
