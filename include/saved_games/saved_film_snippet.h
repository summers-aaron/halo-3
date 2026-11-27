#ifndef SAVED_GAMES_SAVED_FILM_SNIPPET_H
#define SAVED_GAMES_SAVED_FILM_SNIPPET_H

// Functions defined in tu/blamlibxbox/saved_film_snippet.cpp that other units call.

void saved_film_snippet_finished_seek(void *finished_flag);

struct s_saved_film_snippet_marker;

extern "C" unsigned char saved_film_set_snippet_start_tick(void);

extern "C" void saved_film_snippet_handle_message(long message_a, long message_b, const void *message_c, long message_d);
extern "C" long saved_film_snippet_find_last_recorded_marker(s_saved_film_snippet_marker *markers);
extern "C" long saved_film_snippet_find_best_marker_for_tick(s_saved_film_snippet_marker *markers, long tick, bool seekable_only);

#endif // SAVED_GAMES_SAVED_FILM_SNIPPET_H
