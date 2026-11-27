#ifndef NETWORKING_LOGIC_LIFE_CYCLE_HANDLER_PRE_GAME_SERVICES_H
#define NETWORKING_LOGIC_LIFE_CYCLE_HANDLER_PRE_GAME_SERVICES_H

#include "main/levels.h"

extern "C++" {
class c_network_session_parameter_base;

enum e_network_game_start_mode;

enum e_network_game_start_mode network_squad_session_get_start_mode(void);
extern "C" bool render_is_loading_complete(long controller_count);
}

#endif // NETWORKING_LOGIC_LIFE_CYCLE_HANDLER_PRE_GAME_SERVICES_H
