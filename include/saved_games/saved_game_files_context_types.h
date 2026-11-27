#ifndef SAVED_GAMES_SAVED_GAME_FILES_CONTEXT_TYPES_H
#define SAVED_GAMES_SAVED_GAME_FILES_CONTEXT_TYPES_H

struct s_saved_game_files_context
{
    long user_index;
    const wchar_t *first_string;
    const wchar_t *second_string;
    const wchar_t *third_string;
    long campaign_difficulty;
};

#endif // SAVED_GAMES_SAVED_GAME_FILES_CONTEXT_TYPES_H
