// blamlibxbox/cheats.obj: from the records 0xc3f98
// @flags /TP /O2 /Oc
#include "game/game_globals_storage.h"
#include "memory/tls_globals.h"
#include "game/cheats.h"
#include "game/game_skulls.h"
// @address 0xc3f98 ice_cream_flavor_available

extern "C" {
unsigned char ice_cream_flavor_available(int skull)
{
    int skulls = tls_game_globals()->options.game_mode == 1 ? tls_game_globals()->options.primary_skulls : 0;
    bool result = false;
    if (!skull_out_of_range(skull, 9))
        result = (((1 << skull) & skulls) != 0);
    return result;
}
}
