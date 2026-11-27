#ifndef GAME_GAME_ENGINE_GLOBALS_MESSAGES_H
#define GAME_GAME_ENGINE_GLOBALS_MESSAGES_H

// Messages queued for process_game_engine_globals_messages; the one type sets or clears a
// user's bit in the game engine globals' pending flags.
enum e_game_engine_globals_message_type
{
    _game_engine_globals_message_set_pending_flag = 0,
};

void post_game_engine_globals_message(e_game_engine_globals_message_type type, char bit, short value);

#endif // GAME_GAME_ENGINE_GLOBALS_MESSAGES_H
