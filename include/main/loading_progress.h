#ifndef MAIN_LOADING_PROGRESS_H
#define MAIN_LOADING_PROGRESS_H

struct loading_progress_globals
{
    const char *scenario_path;
    // March main\loading.cpp:515 ASSERT("!loading_globals.tag_load_in_progress"); byte at base+4.
    bool tag_load_in_progress;
    // Genuine gap: March has no field between the bool and the timestamp; aligns +8.
    char alignment[3];
    // March main_tag_load_begin stores system_milliseconds() here; main_tag_load_end reads it for elapsed time.
    unsigned long tag_load_start_time;
    bool visible;
    long stage;
    float maximum[3];
    float current[3];
    float progress;
    char pad30;
    bool active;
};

extern "C" loading_progress_globals g_loading_progress;

#endif // MAIN_LOADING_PROGRESS_H
