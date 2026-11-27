#ifndef SAVED_GAMES_SAVED_FILM_FILE_NAME_H
#define SAVED_GAMES_SAVED_FILM_FILE_NAME_H

// Saved-film's accepted call supplies a capacity argument. The callee's current
// seven-argument declaration in saved_game_files_operations.h remains disputed.
extern "C" bool saved_game_files_get_film_name(
    long controller_index, unsigned long type, const wchar_t *first_string, const wchar_t *second_string,
    long campaign_difficulty, long unused, wchar_t *output, long capacity);

#endif
