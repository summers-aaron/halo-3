# animation_manager.cpp (GAME-CHANGES F37): g_mixing_board_definition_list (3 x c_mixing_board_definition, 0x12c each)
# is all zero in the image; retail's dynamic initializer 0x82708c00 (not in source) builds it at startup. Its stores,
# run by tmp/m5d/mbinit over the oracle's disassembly, are in mixing_board.words.
patch 0x79d458 port/probe/fill/mixing_board.words
datum ?g_mixing_board_definition_list!!3PAVc_mixing_board_definition!!A 0x79d458 p w p3 w10 p60 p w p3 w10 p60 p w p3 w10 p60
