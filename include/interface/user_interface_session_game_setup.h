#ifndef INTERFACE_USER_INTERFACE_SESSION_GAME_SETUP_H
#define INTERFACE_USER_INTERFACE_SESSION_GAME_SETUP_H

class c_game_variant;

// Variant verification owned by user_interface_session.cpp.
extern "C" bool user_interface_squad_set_game_variant_with_verify(const c_game_variant *variant);

#endif // INTERFACE_USER_INTERFACE_SESSION_GAME_SETUP_H
