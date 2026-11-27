// objectslibxbox/motor_programs.obj: from the records 0xdc038 0x110ca0 0x36d448 0x36d508 0x36d598
// @flags /TP /O2 /Oc
#include "tag_files/tag_types.h"
#include "objects/object_datum.h"
#include "objects/object_header.h"
#include "tag_files/tag_groups.h"
#include "units/motor_enums.h"
#include "units/motor_request.h"
#include "units/motor_program_state.h"
#include "motor/motor_programs.h"
#include "objects/mover_datum.h"
#include "motor/motor_programs.h"
#include "motor/action_system.h"
// @address 0x110ca0 ?motor_program_update@@YAXJ@Z
// @address 0x36d448 ?generic_motor_program_submit@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
// @address 0x36d508 ?set_program@c_motor_program_state@@QAA_NJW4e_motor_program@@@Z

#include "motor/motor_program_definition.h"
#include "units/biped_definitions.h"

// Program updates notify the object without a deleted-object argument.
// tmp/probe: the retail objects these tables point at, filled from the retail image (tmp/probe/fill/motor.spec; GAME-CHANGES F6).
extern "C" char retail_79dd18[], retail_79dd68[], retail_79ddc0[], retail_79de08[], retail_79de50[], retail_79dea8[], retail_79df00[], retail_79df48[], retail_79dfa0[], retail_79dff8[], retail_79e040[], retail_79e098[], retail_79e0e0[], retail_79e128[], retail_79e170[], retail_79e1b8[], retail_73bb68[];
typedef bool (*object_handler)(long object_index);
c_motor_program_definition *g_program_definition[] = {
    (c_motor_program_definition *)retail_79dd18,
    (c_motor_program_definition *)retail_79dd68,
    (c_motor_program_definition *)retail_79ddc0,
    (c_motor_program_definition *)retail_79de08,
    (c_motor_program_definition *)retail_79de50,
    (c_motor_program_definition *)retail_79dea8,
    (c_motor_program_definition *)retail_79df00,
    (c_motor_program_definition *)retail_79df48,
    (c_motor_program_definition *)retail_79dfa0,
    (c_motor_program_definition *)retail_79dff8,
    (c_motor_program_definition *)retail_79e040,
    (c_motor_program_definition *)retail_79e098,
    (c_motor_program_definition *)retail_79e0e0,
    (c_motor_program_definition *)retail_79e128,
    (c_motor_program_definition *)retail_79e170,
    (c_motor_program_definition *)retail_79e1b8
};

// The per-action callback table indexed by the inlined action_get_definition
// (84 entries; index 26, _action_token, has no definition). Entries point at the
// s_action_definition structs packed at 0x8273c654..0x8273cac8.
// @address 0x73cc38 g_action_definitions
#ifdef HALO3_64BIT
extern "C" s_action_definition *g_action_definitions[k_action_count];
#else
extern "C" s_action_definition *g_action_definitions[k_action_count] = {
    (s_action_definition *)(retail_73bb68 + (0x73c730 - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c744 - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c758 - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c76c - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c780 - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c794 - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c730 - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c730 - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c780 - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c7a8 - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c7bc - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c7d0 - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c7e4 - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c7f8 - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c80c - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c820 - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c834 - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c848 - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c85c - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c870 - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c884 - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c898 - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c8ac - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c8c0 - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c8d4 - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c8e8 - 0x73bb68)),
    0,
    (s_action_definition *)(retail_73bb68 + (0x73c8fc - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c910 - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c924 - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c938 - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c94c - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c960 - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c974 - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c988 - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c99c - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c9b0 - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c9c4 - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c9d8 - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c9ec - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73ca00 - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73ca14 - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73ca28 - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73ca3c - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c654 - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c668 - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c67c - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c67c - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73ca50 - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73ca64 - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c690 - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c6a4 - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73ca78 - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c6b8 - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c6cc - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c6cc - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c6e0 - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c6f4 - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73ca8c - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73caa0 - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73cab4 - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c708 - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c708 - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c708 - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c708 - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c730 - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c71c - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c71c - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c71c - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c71c - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c71c - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c71c - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c71c - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c71c - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c71c - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c71c - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c71c - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c71c - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c71c - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c71c - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c71c - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c71c - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c71c - 0x73bb68)),
    (s_action_definition *)(retail_73bb68 + (0x73c71c - 0x73bb68)),
};
#endif

c_motor_program_definition *c_motor_program_state::definition_get()
{
    if (program != _motor_program_none)
        return g_program_definition[program];
    else
        return 0;
}

// @address 0xdc038 ?find_task_definition@c_motor_program_definition@@QAAPAUs_motor_task_definition@@W4e_motor_task@@@Z
s_motor_task_definition *c_motor_program_definition::find_task_definition(e_motor_task task)
{
    s_motor_task_definition *result = 0;
    for (short task_index = 0; task_index < task_count; task_index++) {
        s_motor_task_definition *definition = tasks[task_index];
        if (definition->task == task) {
            result = definition;
            break;
        }
    }
    return result;
}



// 0x110ca0
void motor_program_update(long object_index)
{
    mover_datum *object = (mover_datum *)object_get_and_verify_type(object_index, ~0UL);
    c_motor_program_state *state = (c_motor_program_state *)object_header_block_get(object, &object->motor_program);
    c_motor_program_definition *definition = state->definition_get();
    if (definition && definition->handle_deleted_object)
        reinterpret_cast<object_handler>(definition->handle_deleted_object)(object_index);
}

// 0x36d448
e_motor_result generic_motor_program_submit(long object_index, const c_motor_request *request)
{
    mover_datum *object = (mover_datum *)object_get_and_verify_type(object_index, 0x3003);
    e_motor_result result = _motor_result_none;
    c_motor_program_state *state = (c_motor_program_state *)object_header_block_get(object, &object->motor_program);
    c_motor_program_definition *definition = state->definition_get();
    s_motor_task_definition *task_definition = definition->find_task_definition(request->m_task);
    if (task_definition && task_definition->submit)
        result = task_definition->submit(object_index, request);
    return result;
}

bool c_motor_program_state::set_program(long object_index, e_motor_program new_program)
{
    bool result = true;
#ifdef HALO3_64BIT
    const unsigned long k_program_count = sizeof(g_program_definition) / sizeof(g_program_definition[0]);
    if (program != _motor_program_none && (unsigned long)program < k_program_count) {
        c_motor_program_definition *definition = g_program_definition[program];
        if (definition && definition->deactivate)
            definition->deactivate(object_index);
    }
    // deactivate() may relocate or delete the object in g_object_memory_pool; re-fetch state.
    mover_datum *object = (mover_datum *)object_try_and_get_and_verify_type(object_index, ~0UL);
    if (object == 0 || object->motor_program.size <= 0)
        return false;
    c_motor_program_state *target = (c_motor_program_state *)object_header_block_get(object, &object->motor_program);
    target->program = new_program;
    if (new_program != _motor_program_none && (unsigned long)new_program < k_program_count) {
        c_motor_program_definition *definition = g_program_definition[new_program];
        if (definition && definition->activate)
            result = definition->activate(object_index);
    }
#else
    if (program != _motor_program_none) {
        c_motor_program_definition *definition = g_program_definition[program];
        if (definition->deactivate)
            definition->deactivate(object_index);
    }
    program = new_program;
    if (new_program != _motor_program_none) {
        c_motor_program_definition *definition = g_program_definition[new_program];
        if (definition->activate)
            result = definition->activate(object_index);
    }
#endif
    return result;
}

// @address 0x36d598 ?mover_default_program@@YA?AW4e_motor_program@@J@Z
e_motor_program mover_default_program(long object_index)
{
    mover_datum *object = (mover_datum *)object_get_and_verify_type(object_index, 0x3003);
    e_motor_program program = _motor_program_none;
    switch (object->object.identifier.get_type()) {
    case 0: {
        biped_definition *definition = (biped_definition *)tag_get('bipd', object->object.definition_index);
        if ((definition->physics.flags & (1UL << 4)) != 0)
            program = _motor_program_flying;
        else
            program = _motor_program_ground;
        break;
    }
    case 1:
        program = _motor_program_vehicle;
        break;
    case _object_type_effect_scenery:
        program = _motor_program_effect_scenery;
        break;
    default:
        // match: 0x36d598: unreachable hint omits dead branch check
        __assume(0);
    }
    return program;
}
