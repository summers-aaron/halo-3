#ifndef SAVED_GAMES_SAVED_FILM_PLAYBACK_H
#define SAVED_GAMES_SAVED_FILM_PLAYBACK_H

#include "shell/shell.h"
#include "networking/tools/http_stream.h"
#include "networking/logic/storage/network_http_buffer_downloader.h"
#include "networking/logic/storage/network_http_buffer_url.h"
#include "tag_files/file_reference.h"
#include <stddef.h>

// Layout from saved_film.cpp, shared by the start-menu playback controls.
struct s_saved_film_playback
{
    c_http_post_source post_source;
    c_http_stored_buffer_downloader<1024> downloader;
    e_controller_index controller_index;
    long state;
    unsigned long load_screen_start_time;
    s_network_http_buffer_url url;
    long content_item_index;
    s_file_reference file;
    unsigned char pending_screen_update;
};
typedef char saved_film_playback_size[(sizeof(void *) != 4 || ((sizeof(s_saved_film_playback) == 0xdfc))) ? 1 : -1];
typedef char saved_film_playback_state_offset[(sizeof(void *) != 4 || ((offsetof(s_saved_film_playback, state) == 0xbcc))) ? 1 : -1];
extern "C" s_saved_film_playback *g_saved_film_playback;

#endif
