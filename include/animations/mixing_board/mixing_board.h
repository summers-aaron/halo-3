#ifndef ANIMATIONS_MIXING_BOARD_MIXING_BOARD_H
#define ANIMATIONS_MIXING_BOARD_MIXING_BOARD_H

// Functions defined in tu/blamlibxbox/mixing_board.cpp that other units call.

class c_animation_manager;
class c_mixing_board_definition;

#if defined(__clang__)
void reset_mixing_board(c_animation_manager *manager, unsigned char *starting_address, const c_mixing_board_definition *definition);
#else
void reset_mixing_board(c_animation_manager *manager,long starting_address,const c_mixing_board_definition *definition);
#endif

#endif // ANIMATIONS_MIXING_BOARD_MIXING_BOARD_H
