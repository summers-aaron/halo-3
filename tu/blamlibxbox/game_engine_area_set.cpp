// blamlibxbox/game_engine_area_set.obj: from the records 0x2fe928 0x2fe9b8 0x2fea30
// @flags /TP /O2 /Oc
#include "cseries/cseries_wrapped.h"
// @address 0x2fe928 ?set_selected@c_area@@QAAX_N@Z
// @address 0x2fe9b8 ?is_selected@c_area@@QBA_NXZ
#include <stdlib.h>

#include "objects/objects.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "game/game_engine_area_set.h"
#include "objects/object_datum.h"
#include "objects/object_header.h"
#include "game/game_engine_area_types.h"
#include "objects/object_multiplayer_access.h"

void c_area::set_selected(bool selected)
{
    if (m_object_index != NONE)
    {
        s_multiplayer_object_properties *multiplayer = object_try_and_get_multiplayer(m_object_index);
        if (multiplayer)
        {
            multiplayer->flags.set(_multiplayer_object_data_flag_selected, selected);
            simulation_action_object_update(m_object_index, 4096);
        }
    }
}

bool c_area::is_selected() const
{
    bool selected = false;
    if (m_object_index != NONE)
    {
        s_multiplayer_object_properties *multiplayer = object_try_and_get_multiplayer(m_object_index);
        selected = multiplayer != 0 && (_rotl((unsigned short)multiplayer->flags.get(), 24) & 1) != 0;
    }
    return selected;
}


static __forceinline signed char game_engine_area_set_key_byte(const object_datum *object)
{
    const s_multiplayer_object_properties *multiplayer = (const s_multiplayer_object_properties *)
        object_header_block_get(object, &object->multiplayer_properties);
    return multiplayer->spawn_sequence;
}

// @address 0x2fea30 ?c_area_set_compare_by_order_index@@YAHPBX0@Z
int c_area_set_compare_by_order_index(void const * _sym_p0, void const * _sym_p1)
{
    object_datum *a = (object_datum *)object_get_and_verify_type(*((const long *)_sym_p0), NONE);
    object_datum *b = (object_datum *)object_get_and_verify_type(*((const long *)_sym_p1), NONE);
    int result = game_engine_area_set_key_byte(a) - game_engine_area_set_key_byte(b);
    if (result != 0)
        return result;
    result = a->scenario_datum_index - b->scenario_datum_index;
    if (result != 0)
        return result;
    return a->map_variant_index - b->map_variant_index;
}
