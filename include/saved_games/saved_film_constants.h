#ifndef SAVED_GAMES_SAVED_FILM_CONSTANTS_H
#define SAVED_GAMES_SAVED_FILM_CONSTANTS_H
#include "tag_files/files.h"
#include "networking/online/shared_file_content_type.h"
#include "math/real_math_constants.h"
#include "saved_games/saved_film_storage.h"

struct real_decibel;

// Strings and tables defined by saved_film.cpp.
extern "C" {
extern char k_saved_film_upload_xuid_header_format[];
extern char k_saved_film_upload_slot_header_format[];
extern char k_saved_film_upload_url[];
extern const char k_saved_film_snippet_filename[];
extern char k_saved_film_recording_filename[];
extern const char k_saved_film_blf_name[];
extern const char k_saved_film_session_id[];
extern char k_saved_film_source_filename[];
extern const char k_saved_film_sound_class_name[];
extern const real_decibel k_saved_film_sound_gain[];
extern const char k_custom_map_image_filename_format[];
extern const char k_game_build_string[];
}

#endif
