// port/probe/extra: the move callback of the motor programs that never change program on their own (retail
// 0x79e040 and 0x79e1b8 in g_program_definition, slot +0x14). Retail points both at 0x821c1258, `li r3, -1; blr`:
// the Xbox linker folded every function with that body into one, and retail-names.tsv names it after members of the
// fold (c_game_engine_base_variant::get_game_engine_name_string_id, phys_shape::compute_dma_size). On PC those are
// __thiscall and pop their argument (ret 4), while mover_move calls the slot as a cdecl
// e_motor_program (*)(long, c_motor_request *): each vehicle move then returned with esp one word high, and the
// caller's epilogue jumped to the object handle (0xef670005-style crashes entering a game with vehicles).
// port/probe/fill/motor.spec names 0x1c1258 after this function, so only the motor program tables bind it.
#include "units/motor_enums.h"

class c_motor_request;

e_motor_program probe_motor_program_move_none(long object_index, c_motor_request *request)
{
    (void)object_index;
    (void)request;
    return _motor_program_none;
}
