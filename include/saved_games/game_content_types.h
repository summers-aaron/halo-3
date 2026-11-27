#ifndef SAVED_GAMES_GAME_CONTENT_TYPES_H
#define SAVED_GAMES_GAME_CONTENT_TYPES_H

enum e_game_content_type
{
    _game_content_type_none = -1,
    _game_content_type_savegame = 0,
    _game_content_type_map = 1,
    _game_content_type_personal = 2,
    _game_content_type_ctf = 3,
    _game_content_type_slayer = 4,
    _game_content_type_oddball = 5,
    _game_content_type_koth = 6,
    _game_content_type_jugg = 7,
    _game_content_type_terries = 8,
    _game_content_type_assault = 9,
    _game_content_type_vip = 10,
    _game_content_type_zombiez = 11,
    _game_content_type_film = 12,
    _game_content_type_clip = 13,
    _game_content_type_shot = 14,
    k_game_content_type_count = 15
};

enum e_saved_game_file_type;

e_game_content_type saved_game_file_type_to_game_content_type(e_saved_game_file_type type);

#endif
