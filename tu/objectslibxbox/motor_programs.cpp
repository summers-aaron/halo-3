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
typedef bool (*object_handler)(long object_index);
c_motor_program_definition *g_program_definition[] = {
    (c_motor_program_definition *)0x8279dd18,
    (c_motor_program_definition *)0x8279dd68,
    (c_motor_program_definition *)0x8279ddc0,
    (c_motor_program_definition *)0x8279de08,
    (c_motor_program_definition *)0x8279de50,
    (c_motor_program_definition *)0x8279dea8,
    (c_motor_program_definition *)0x8279df00,
    (c_motor_program_definition *)0x8279df48,
    (c_motor_program_definition *)0x8279dfa0,
    (c_motor_program_definition *)0x8279dff8,
    (c_motor_program_definition *)0x8279e040,
    (c_motor_program_definition *)0x8279e098,
    (c_motor_program_definition *)0x8279e0e0,
    (c_motor_program_definition *)0x8279e128,
    (c_motor_program_definition *)0x8279e170,
    (c_motor_program_definition *)0x8279e1b8
};

// The per-action callback table indexed by the inlined action_get_definition
// (84 entries; index 26, _action_token, has no definition). Entries point at the
// s_action_definition structs packed at 0x8273c654..0x8273cac8.
// @address 0x73cc38 g_action_definitions
s_action_definition *g_action_definitions[k_action_count] = {
    (s_action_definition *)0x8273c730,
    (s_action_definition *)0x8273c744,
    (s_action_definition *)0x8273c758,
    (s_action_definition *)0x8273c76c,
    (s_action_definition *)0x8273c780,
    (s_action_definition *)0x8273c794,
    (s_action_definition *)0x8273c730,
    (s_action_definition *)0x8273c730,
    (s_action_definition *)0x8273c780,
    (s_action_definition *)0x8273c7a8,
    (s_action_definition *)0x8273c7bc,
    (s_action_definition *)0x8273c7d0,
    (s_action_definition *)0x8273c7e4,
    (s_action_definition *)0x8273c7f8,
    (s_action_definition *)0x8273c80c,
    (s_action_definition *)0x8273c820,
    (s_action_definition *)0x8273c834,
    (s_action_definition *)0x8273c848,
    (s_action_definition *)0x8273c85c,
    (s_action_definition *)0x8273c870,
    (s_action_definition *)0x8273c884,
    (s_action_definition *)0x8273c898,
    (s_action_definition *)0x8273c8ac,
    (s_action_definition *)0x8273c8c0,
    (s_action_definition *)0x8273c8d4,
    (s_action_definition *)0x8273c8e8,
    0,
    (s_action_definition *)0x8273c8fc,
    (s_action_definition *)0x8273c910,
    (s_action_definition *)0x8273c924,
    (s_action_definition *)0x8273c938,
    (s_action_definition *)0x8273c94c,
    (s_action_definition *)0x8273c960,
    (s_action_definition *)0x8273c974,
    (s_action_definition *)0x8273c988,
    (s_action_definition *)0x8273c99c,
    (s_action_definition *)0x8273c9b0,
    (s_action_definition *)0x8273c9c4,
    (s_action_definition *)0x8273c9d8,
    (s_action_definition *)0x8273c9ec,
    (s_action_definition *)0x8273ca00,
    (s_action_definition *)0x8273ca14,
    (s_action_definition *)0x8273ca28,
    (s_action_definition *)0x8273ca3c,
    (s_action_definition *)0x8273c654,
    (s_action_definition *)0x8273c668,
    (s_action_definition *)0x8273c67c,
    (s_action_definition *)0x8273c67c,
    (s_action_definition *)0x8273ca50,
    (s_action_definition *)0x8273ca64,
    (s_action_definition *)0x8273c690,
    (s_action_definition *)0x8273c6a4,
    (s_action_definition *)0x8273ca78,
    (s_action_definition *)0x8273c6b8,
    (s_action_definition *)0x8273c6cc,
    (s_action_definition *)0x8273c6cc,
    (s_action_definition *)0x8273c6e0,
    (s_action_definition *)0x8273c6f4,
    (s_action_definition *)0x8273ca8c,
    (s_action_definition *)0x8273caa0,
    (s_action_definition *)0x8273cab4,
    (s_action_definition *)0x8273c708,
    (s_action_definition *)0x8273c708,
    (s_action_definition *)0x8273c708,
    (s_action_definition *)0x8273c708,
    (s_action_definition *)0x8273c730,
    (s_action_definition *)0x8273c71c,
    (s_action_definition *)0x8273c71c,
    (s_action_definition *)0x8273c71c,
    (s_action_definition *)0x8273c71c,
    (s_action_definition *)0x8273c71c,
    (s_action_definition *)0x8273c71c,
    (s_action_definition *)0x8273c71c,
    (s_action_definition *)0x8273c71c,
    (s_action_definition *)0x8273c71c,
    (s_action_definition *)0x8273c71c,
    (s_action_definition *)0x8273c71c,
    (s_action_definition *)0x8273c71c,
    (s_action_definition *)0x8273c71c,
    (s_action_definition *)0x8273c71c,
    (s_action_definition *)0x8273c71c,
    (s_action_definition *)0x8273c71c,
    (s_action_definition *)0x8273c71c,
    (s_action_definition *)0x8273c71c,
};

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
