// ailibxbox/cl_engine.obj: from the records 0xb5da8 0xb70f0 0xb75a0 0xb7b98 0xd4e10 0xd5140 0x4fb860 0x4fba58 0x4fbb30 0x4fbc70 0x4fbd30 0x4fbdf0 0x4fbe40 0x4fbff8 0x4fc080 0x4fc188 0x4fc588 0x4fc878 0x4fcd58 0x4fcf80 0x4fcfb0 0x4fd0e8 0x4fd1b0 0x4fd228 0x4fd378 0x4fd380 0x4fd400 0x4fd490 0x4fd538 0x4fd5d8 0x552d30
// @address 0x11df694 command_script_data
#include "hs/hs_thread_deterministic_data.h"
#include "ai/actor_perception.h"
#include "ai/actor_type_flood_types.h"
#include "ai/cl_engine_data.h"
#include "ai/ai_atoms.h"
#include "ai/command_script_data.h"
#include "math/real_math_functions.h"
#include "ai/swarms.h"
#include "memory/tls_data_arrays.h"
#include "memory/tls_globals.h"
#include "multithreading/thread_tls.h"
#include "ai/swarm_creature_iterator_types.h"
#include "creatures/creature_datum.h"
#include "math/real_math_types.h"
#include "tag_files/tag_types.h"
#include "game/game_time_types.h"
#include "ai/ai_point3d.h"
#include "ai/cl_engine.h"
#include "ai/ai_direction.h"
#include "ai/actor_globals.h"
#include "memory/data.h"
#include "ai/actors.h"
#include "cache/restricted_memory_regions.h"
#include "objects/objects.h"
#include "ai/ai.h"
#include "ai/ai_actions.h"
#include "ai/ai_dialogue.h"
#include "ai/ai_reference_frame.h"
#include "ai/ai_script.h"
#include "effects/effects.h"
#include "motor/action_system.h"
#include "ai/unit_dialogue_types.h"
#include "hs/hs_thread_types.h"
#include "hs/hs_script_types.h"
#include "cache/cache_files.h"
#include "hs/hs_runtime.h"
#include "ai/squad_iterator_types.h"
#include "cseries/crt_xdk.h"
#include "scenario/scenario_tag.h"
#include "memory/data_types.h"
#include "cache/restricted_memory.h"
#include "ai/behavior.h"
#include "ai/actor_datum.h"
#include "ai/command_script_types.h"
#include "ai/cs_scenario_definitions.h"
#include "units/unit_types.h"
#include "units/motor_program_state.h"
#include "objects/object_header.h"
#include "scenario/scenario.h"
#include "ai/actor_moving.h"
#include "ai/character_definitions.h"
#include <string.h>
#include "cache/restricted_memory_registration.h"
#include "ai/mission_dialogue_types.h"
#include "ai/character_voice_properties.h"
#include "game/game_time_inline.h"

__declspec(thread) s_data_array *command_script_data;

struct s_role_set {
  long actors[10];
  float weights[10];
  long roles[10];
  short variants[10];
  short count;
};

// @address 0x8974cc global_new_state_data
c_behavior_state *global_new_state_data;
// @address 0x89785c global_new_behavior
short *global_new_behavior;
#if !defined(_XBOX) || defined(__clang__)
// The image holds this table at rva 0x72fc20, indexed by e_ai_atom.
s_atom_definition *g_atom_definitions[29] = {
    &g_atom_pause,
    &g_atom_go_to,
    &g_atom_go_by,
    &g_atom_go_to_and_face,
    &g_atom_go_to_and_posture,
    &g_atom_move_direction,
    &g_atom_approach_object,
    &g_atom_grenade,
    &g_atom_equipment,
    &g_atom_vehicle,
    &g_atom_jump,
    &g_atom_targeted_jump,
    &g_atom_recording,
    &g_atom_action,
    &g_atom_vocalize,
    &g_atom_die,
    &g_atom_behavior,
    &g_atom_fly_to,
    &g_atom_fly_by,
    &g_atom_fly_to_and_face,
    &g_atom_formation,
    &g_atom_approach,
    &g_atom_deploy_turret,
    &g_atom_custom_animation,
    &g_atom_play_sound,
    &g_atom_melee,
    &g_atom_smash,
    &g_atom_swarm_to,
    &g_atom_swarm_from,
};
#endif
// @address 0x72fc98 g_atom_info
// pad0 carries the atom index in the high halfword.
s_atom_info g_atom_info[29] = {
    { 0x00000000, 0, 1 },
    { 0x00010000, 0, 1 },
    { 0x00020000, 0, 0 },
    { 0x00030000, 0, 0 },
    { 0x00040000, 0, 0 },
    { 0x00050000, 0, 1 },
    { 0x00060000, 0, 1 },
    { 0x00070000, 1, 0 },
    { 0x00080000, 1, 0 },
    { 0x00090000, 0, 0 },
    { 0x000a0000, 0, 1 },
    { 0x000b0000, 0, 1 },
    { 0x000c0000, 0, 0 },
    { 0x000d0000, 1, 0 },
    { 0x000e0000, 2, 0 },
    { 0x000f0000, 1, 0 },
    { 0x00100000, 0, 0 },
    { 0x00110000, 0, 0 },
    { 0x00120000, 0, 0 },
    { 0x00130000, 0, 0 },
    { 0x00140000, 0, 0 },
    { 0x00150000, 0, 0 },
    { 0x00160000, 0, 0 },
    { 0x00170000, 0, 0 },
    { 0x00180000, 2, 0 },
    { 0x00190000, 1, 0 },
    { 0x001a0000, 1, 0 },
    { 0x001b0000, 0, 1 },
    { 0x001c0000, 0, 1 },
};

// @flags /TP /O2 /Oc
// @address 0x4fbc70 ?cs_stack_command_script@@YAJJFJ@Z
// @address 0x4fbd30 ?cs_queue_command_script@@YAJJFJ@Z
// @address 0x4fbdf0 ?cs_abort_thread@@YAXJ@Z
// @address 0x4fd538 ?allocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_effect_data_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
// @address 0x4fd5d8 ?deallocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_effect_data_allocator@@YAXPAX@Z@@UAAXPAX@Z
// @address 0x552d30 ?__tls_set_g_command_script_allocator@@YAXPAX@Z
typedef unsigned int uint;

enum e_prop_stimulus;



namespace {
static __forceinline hs_thread *hs_thread_get(long thread_index)
{
    hs_thread *thread = datum_get<hs_thread>((s_data_array *)g_hs_thread_deterministic_data, thread_index);
    return thread;
}
}
static __forceinline void cs_abort_thread_inline(long thread_index)
{
    hs_thread *thread = hs_thread_get(thread_index);
    if ((thread->flags & 0x80) == 0)
    {
        thread->flags |= 0x80;
        thread->flags |= 0x40;
    }
    thread->sleep_until = 0;
}
void cs_abort_thread(long thread_index)
{
    cs_abort_thread_inline(thread_index);
}


namespace {
static __forceinline void cl_engine_actor_path_clear(long actor_index)
{
    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
    actor->path.valid = 0;
    actor->path.obstacle_object_index = -1;
    actor->path.result_value = 0;
    actor->path_result_time = 0;
    actor->path_result_count = 0;
    actor->path_state = 0;
    actor->destination_orders.clear();
}
}

// @address 0x4fbff8 ?actor_allow_vignette@@YA_NJJFPA_N@Z
bool actor_allow_vignette(long actor_index,long script_index,short priority,bool *blocked)
{
    bool result=true;
    long index=datum_get<actor_datum>(tls_actor_data(), actor_index)->command_index;
    while(index!=-1) {
        command_script_datum *s=datum_get<command_script_datum>(tls_command_script_data(), index);
        if(s->thread_index!=script_index && s->priority<priority) {
            if(blocked)
                *blocked=true;
            index=s->next_cs;
            continue;
        }
        result=false;
        break;
    }
    return result;
}

// @address 0x4fcd58 ?cs_helper@@YAFJFP6AFJJPAUcs_command@@PAUs_atom_control@@J@ZJ@Z
short cs_helper(long actor_index,short mask,cl_engine_command_function function,long script_index)
{
    actor_datum *a=datum_get<actor_datum>(tls_actor_data(), actor_index);
    command_script_datum *s=datum_get<command_script_datum>(tls_command_script_data(), script_index);
    short result=1;
    if(a->swarm) {
        if(a->swarm_index!=-1) {
            short success=0,failure=0,total=0;
            result=1;
            swarm_creature_iterator iter;
            iter.next_object_index = datum_get<swarm_datum>(tls_swarm_data(), a->swarm_index)->first_object_index;
            iter.current_object_index = -1;
            cs_command *cmd=&s->command[0];
            creature_datum *c;
            while((c=swarm_creature_iterator_next(&iter))!=0) {
                swarm_creature_state *state=c->creature.link_type==0?(swarm_creature_state *)object_header_block_get(c,
                    &c->creature.creature_reference):0;
                if(state) {
                    state->command_script_control.status=function(actor_index,iter.current_object_index,cmd,&state->command_script_control,script_index);
                    if(state->command_script_control.status==1) success++;
                    else if(state->command_script_control.status==0) failure++;
                    total++;
                }
            }
            if((float)(success+failure)>=(float)total*0.8f) result=success>failure?1:0;
            else result=2;
        }
    } else {
        result=-1;
        for(short i=0;i<3;i++) {
            if(mask&(1<<i)) {
                cs_command *cmd=&s->command[i];
                short current=function(actor_index,a->unit_index,cmd,&s->control[i],script_index);
                if(current>result) result=current;
            }
        }
    }
    return result;
}

struct s_atom_info;
// @address 0x4fcf80 ?cs_control_clear@@YAFJJPAUcs_command@@PAUs_atom_control@@J@Z
short cs_control_clear(long actor_index,long script_index,cs_command *command,s_atom_control *control,long argument)
{
    long *words=(long *)control;
    for(long i=0;i<9;i++)
        words[i]=0;
    control->atom_type=-1;
    control->status=-1;
    return -1;
}

// @address 0x4fcfb0 ?cs_command_start@@YAFJJPAUcs_command@@PAUs_atom_control@@J@Z
short cs_command_start(long actor_index,long script_index,cs_command *command,s_atom_control *control,long argument)
{
    s_atom_definition *definition = g_atom_definitions[command->atom_type];
    short result=1;
    if(!datum_get<actor_datum>(tls_actor_data(), actor_index)->swarm || g_atom_info[command->atom_type].swarm_allowed) {
        if(control->status==2)
            cs_command_end(actor_index,script_index,0,control,argument);
        long *words=(long *)control;
        for(long i=0;i<9;i++)
            words[i]=0;
        control->atom_type=-1;
        control->status=-1;
        control->atom_type=command->atom_type;
        if(definition->start)
            result=definition->start(actor_index,script_index,command,control,argument)?2:1;
        else
            result=2;
        control->status=result;
    }
    return result;
}

// @address 0x4fd1b0 ?cs_command_end@@YAFJJPAUcs_command@@PAUs_atom_control@@J@Z
short cs_command_end(long actor_index, long script_index, cs_command *command, s_atom_control *control, long argument)
{
    short type = control->atom_type;
    if (type >= 0 && type < 29 && g_atom_definitions[type]->handle != 0) {
        g_atom_definitions[type]->handle(actor_index, script_index, control, argument);
    }
    control->status = 0;
    return 0;
}

// @address 0x4fd378 cs_command_get_status
extern "C" short cs_command_get_status(long actor_index,long script_index,cs_command *command,s_atom_control *control,long argument)
{
    return control->status;
}

// Whether the index names an element: its high half a block of the table, its low half one of the
// block's elements.
bool valid_point_ref(long point_reference)
{
    bool result = false;
    if (point_reference != -1) {
        scenario *globals = global_scenario;
        if (globals->script_data.count > 0) {
            int block_index = (point_reference >> 16) & 0xffff;
            if (block_index >= 0 && block_index < cs_scenario_get_script_data(globals)->point_sets.count) {
                short element_index = (short)point_reference;
                result = element_index >= 0 && element_index < cs_get_point_set(block_index)->points.count;
            }
        }
    }
    return result;
}

// match: 0x4fd400: volatile access to the typed script-data address preserves the second reload.

bool valid_point_set(long point_set_reference)
{
    bool result = false;
    if (point_set_reference != -1 && global_scenario->script_data.count > 0) {
        scenario *globals = global_scenario;
        long group = (point_set_reference >> 16) & 0xffff;
        if (group >= 0 && group < ((cs_script_data * volatile &)globals->script_data.address)->point_sets.count) {
            short index = (short)point_set_reference;
            if (index == -1) {
                result = 1;
            } else {
                bool ok = index < 0 ? false
                    : index < ((cs_point_set *)((cs_script_data * volatile &)globals->script_data.address)->point_sets.address)[group].points.count;
                result = ok;
            }
        }
    }
    return result;
}

void __tls_set_g_command_script_allocator(void *object)
{
    data_set_new_base_address(&prop_data, (s_data_array *)object);
}





extern "C" {

}

namespace {
__forceinline command_script_datum *command_script_get(long index)
{
    command_script_datum *script = datum_get<command_script_datum>(tls_command_script_data(), index);
    return script;
}
__forceinline hs_thread *command_script_thread_get(long index)
{
    hs_thread *thread = datum_get<hs_thread>((s_data_array *)g_hs_thread_deterministic_data, index);
    return thread;
}
__forceinline long command_script_thread_begin(short script_index)
{
    return hs_runtime_script_begin(script_index, _hs_script_stub, _hs_thread_type_command_script);
}
__forceinline void command_script_command_clear(cs_command *command)
{
    memset(command, 0, sizeof(*command));
    command->atom_type = -1;
    command->index = -1;
}
__forceinline long command_script_control_clear(long actor_index, long script_index, cs_command *command, s_atom_control *control,
    long command_script_index)
{
    memset(control, 0, sizeof(*control));
    control->atom_type = -1;
    control->status = -1;
    return -1;
}
}

// @address 0x4fb860 ?command_script_new@@YAJJJJ@Z
long command_script_new(long owner_index, long script_index, long thread_index)
{
    long command_script_index = datum_new(tls_command_script_data());
    if (command_script_index != -1) {
        command_script_datum *script = command_script_get(command_script_index);
        script->actor_index = owner_index;
        script->script_index = script_index;
        if (thread_index != -1) {
            script->thread_index = thread_index;
        } else {
            script->thread_index = command_script_thread_begin((short)script_index);
            if (script->thread_index != -1) {
                hs_thread *thread = command_script_thread_get(script->thread_index);
                thread->ai_index = (owner_index & 0x1fffffff) | (3UL << 29);
            }
        }
        if (script->thread_index != -1) {
            script->reserved = true;
            script->reserve_mandatory = false;
            script->role_index = -1;
            script->priority = 0;
            script->abort = false;
            script->abort_on_damage = false;
            script->abort_on_alert = false;
            script->abort_on_combat_status = false;
            script->abort_on_combat_status_level = 0;
            script->abort_on_vehicle_exit = false;
            script->next_cs = -1;
            script->shoot_at_target = false;
            script->shoot_secondary_trigger = false;
            script->look_at_target = false;
            script->face_lower_weapon = false;
            script->aim_at_target = false;
            script->face_at_target = false;
            script->walk = false;
            script->crouch = false;
            script->crouch_speed = 0.0f;
            script->vehicle_speed_valid = false;
            script->turn_urgency_valid = false;
            script->boosting = false;
            script->ignore_obstacles = false;
            script->enable_targeting = false;
            script->enable_looking = false;
            script->enable_moving = false;
            script->enable_dialogue = false;
            script->suppress_activity_termination = false;
            script->suppress_global_dialogue = false;
            script->forced_combat_status = 0;
            script->style_index_valid = false;
            script->action_animation_impulse = -1;
            script->animation_mode = -1;
            script->grenade_throw_started = false;
            script->grenade_throw_trajectory_type = -1;
            script->approach_pending = false;
            script->approach_object_index = -1;
            script->approach_within_range = false;
            script->type = 0;
            for (short index = 0; index < 3; index++) {
                command_script_command_clear(&script->command[index]);
                command_script_control_clear(-1, -1, 0, &script->control[index], command_script_index);
            }
        } else {
            ::datum_delete(tls_command_script_data(), command_script_index);
            command_script_index = -1;
        }
    }
    return command_script_index;
}

// @address 0x4fba58 cs_initialize_command
cs_command *cs_initialize_command(long owner_index, short type, bool active)
{
  cs_command *result = 0;
  if (type >= 0 && type < 29) {
    result = &datum_get<command_script_datum>(tls_command_script_data(), owner_index)->command[g_atom_info[type].slot_index];
    memset(result, 0, sizeof(*result));
    result->index = -1;
    result->atom_type = (char)type;
    result->flags |= 1;
    if (active)
      result->flags |= 2;
    else
      result->flags &= ~2;
  }
  return result;
}

struct ai_script_context;

namespace {
static __forceinline int cl_engine_object_variant_int(object_datum *object)
{
    unit_datum *unit = (unit_datum *)object;
    c_motor_program_state *motor = (c_motor_program_state *)object_header_block_get(object, &unit->unit.motor_program);
    return motor->program;
}
}
// @address 0x4fbe40 cs_remove_command_script
extern "C" void cs_remove_command_script(long owner_index, long member_index)
{
    actor_datum *owner = datum_get<actor_datum>(tls_actor_data(), owner_index);
    command_script_datum *member = command_script_get(member_index);
    if (member->thread_index != -1)
    {
        hs_thread *thread = hs_thread_get(member->thread_index);
        if (ai_index_get_actor(thread->ai_index) == owner_index)
            thread->ai_index = 0;
        if (thread->type == 4 || member->reserve_mandatory)
            cs_abort_thread(member->thread_index);
    }

    if (member->script_initiated_posture != 0 && owner->unit_index != -1 &&
        cl_engine_object_variant_int(object_get(owner->unit_index)) == 9)
    {
        actor_exit_posture(owner_index);
    }
    else if (member->script_initiated_custom_animation != 0)
    {
        long object_index = owner->unit_index;
        if (object_index != -1 && cl_engine_object_variant_int(object_get(object_index)) == 11)
            action_submit_event(object_index, 2);
    }

    for (long *link = &owner->command_index; *link != -1; link = &command_script_get(*link)->next_cs)
    {
        if (*link == member_index)
        {
            *link = member->next_cs;
            break;
        }
    }

    if (owner->command_script_index == member_index)
        owner->command_script_index = -1;
    ::datum_delete(tls_command_script_data(), member_index);
}

// @address 0x4fbb30 ?cs_attach_command_script@@YAJJFJ@Z
long cs_attach_command_script(long actor_index, short script_index, long thread_index)
{
    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
    long new_script_index;
    if (global_script_context.ai_index != 0 && actor_index == ai_index_get_actor(global_script_context.ai_index))
    {
        long index = actor->command_index;
        while (index != -1)
        {
            command_script_datum *script = command_script_get(index);
            script->abort = true;
            index = script->next_cs;
        }
        new_script_index = cs_queue_command_script(actor_index, script_index, thread_index);
    }
    else
    {
        new_script_index = command_script_new(actor_index, script_index, thread_index);
        if (new_script_index != -1)
        {
            while (actor->command_index != -1)
                cs_remove_command_script(actor_index, actor->command_index);
            actor->command_index = new_script_index;
            actor->command_script_index = new_script_index;
            cs_helper(actor_index, 7, cs_control_clear, new_script_index);
            cl_engine_actor_path_clear(actor_index);
        }
    }
    return new_script_index;
}

// 0x4fbc70
long cs_stack_command_script(long actor_index, short script_index, long thread_index)
{
    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
    long new_script_index = command_script_new(actor_index, script_index, thread_index);
    if (new_script_index != -1)
    {
        command_script_datum *script = datum_get<command_script_datum>(tls_command_script_data(), new_script_index);
        script->next_cs = actor->command_index;
        actor->command_index = new_script_index;
        actor->command_script_index = new_script_index;
        cs_helper(actor_index, 7, cs_control_clear, new_script_index);
        cl_engine_actor_path_clear(actor_index);
    }
    return new_script_index;
}

// 0x4fbd30
long cs_queue_command_script(long actor_index, short script_index, long thread_index)
{
    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
    long new_script_index = command_script_new(actor_index, script_index, thread_index);
    if (new_script_index != -1)
    {
        long *p_script = &actor->command_index;
        while (*p_script != -1)
        {
            command_script_datum *script = datum_get<command_script_datum>(tls_command_script_data(), *p_script);
            p_script = &script->next_cs;
        }
        *p_script = new_script_index;
        cl_engine_actor_path_clear(actor_index);
    }
    return new_script_index;
}

namespace {
static __forceinline bool cl_engine_reserve_allowed(long actor_index, long thread_index, short priority)
{
    bool result = true;
    long index = datum_get<actor_datum>(tls_actor_data(), actor_index)->command_index;
    while (index != -1)
    {
        command_script_datum *script = datum_get<command_script_datum>(tls_command_script_data(), index);
        if (script->thread_index != thread_index && script->priority < priority)
        {
            index = script->next_cs;
            continue;
        }
        result = false;
        break;
    }
    return result;
}
}

// @address 0x4fc080 ?cs_reserve@@YAJJJ_NF@Z
long cs_reserve(long actor_index, long thread_index, bool mandatory, short priority)
{
    long script_index = -1;
    if (cl_engine_reserve_allowed(actor_index, thread_index, priority))
    {
        script_index = cs_attach_command_script(actor_index, -1, thread_index);
        if (script_index != -1)
        {
            command_script_datum *script = command_script_get(script_index);
            script->reserved = true;
            script->reserve_mandatory = mandatory;
            script->priority = priority;
            return script_index;
        }
    }
    if (mandatory)
        cs_abort_thread(thread_index);
    return script_index;
}

// @address 0x4fd0e8 ?cs_command_update@@YAFJJPAUcs_command@@PAUs_atom_control@@J@Z
short cs_command_update(long actor_index,long script_index,cs_command *command,s_atom_control *control,long argument)
{
    short result=control->status;
    if(control->status==2) {
        if(command->atom_type>=0 && command->atom_type<29 && g_atom_definitions[command->atom_type]->update) {
            result=g_atom_definitions[command->atom_type]->update(actor_index,script_index,command,control,argument);
            if(result!=2) {
                cs_command_end(actor_index,script_index,command,control,argument);
                control->status=result;
            }
        }
        if(control->timer>0)
            control->timer--;
    }
    return result;
}

// @address 0x4fd228 ?cs_handle_deleted_object@@YAXJJ@Z
void cs_handle_deleted_object(long actor_index, long thread_index)
{
    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
    long index = actor->command_index;
    while (index != -1)
    {
        command_script_datum *script = command_script_get(index);
        if (script->look_at_target && script->look_target.type == 1 && script->look_target.index == thread_index)
        {
            script->look_at_target = false;
            script->look_target.index = -1;
        }
        if (script->aim_at_target && script->aim_target.type == 1 && script->aim_target.index == thread_index)
        {
            script->aim_at_target = false;
            script->aim_target.index = -1;
        }
        short atom_count = actor->swarm ? 1 : 3;
        for (short atom_index = 0; atom_index < atom_count; atom_index++)
        {
            signed char type = script->command[atom_index].atom_type;
            if (type >= 0 && type < 29)
            {
                s_atom_definition *definition = g_atom_definitions[type];
                if (definition->destroy != 0)
                    definition->destroy(actor_index, index, thread_index);
            }
        }
        index = script->next_cs;
    }
}



// @address 0x4fd490 point_try_and_get_frame
extern "C" bool point_try_and_get_frame(long point_reference, real_point3d *position, real_vector3d *forward, real_vector3d *up)
{
    bool result = false;
    if (valid_point_ref(point_reference))
    {
        cs_point *point =
            TAG_BLOCK_GET_ELEMENT(&cs_get_point_set((point_reference >> 16) & 0xffff)->points, point_reference & 0xffff, cs_point);
        ::ai_point_get_position(&point->position, position);
        vector3d_from_euler_angles2d(forward, &point->facing);
        ai_reference_frame_transform_vector(point->position.location[0], forward, forward);
        generate_up_vector3d(forward, up);
        result = true;
    }
    return result;
}



namespace {

}

// Explicit specialization: this allocator is instantiated only in this unit, so the generic member
// (which every instantiating unit spells for itself) is not defined here.
template<>
inline void *
t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_effect_data_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
    m_member_index = restricted_region_register_member(1, name, type, size, alignment_bits, __tls_set_g_effect_data_allocator, 0, 0);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(1, m_member_index);
}


// 0x4fd538
template <>
void *t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_effect_data_allocator>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

// 0x4fd5d8
template <>
void t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_effect_data_allocator>::deallocate(void *memory)
{
    tls_command_script_data_slot() = 0;
    restricted_region_free_member(1, m_member_index);
}



namespace {
// match: 0xb5da8: keep the target update expanded at its call site.
// Inline expansion of actor_perception_set_target (actor_perception.obj keeps the out-of-line copy).
static __forceinline void actor_perception_set_target_inline(long actor_index, long target_index)
{
    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
    long previous = actor->target_prop_index;
    actor->target_prop_index = target_index;
    if (previous != -1)
        ::actor_compute_prop_salience(actor_index, previous, _prop_stimulus_none);
    if (target_index != -1)
        ::actor_compute_prop_salience(actor_index, actor->target_prop_index, _prop_stimulus_none);
}
}

// @address 0xb5da8 ?cs_actor_update@@YAXJ@Z
void cs_actor_update(long actor_index)
{
    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
    actor->command_script_index = -1;
    long index = actor->command_index;
    while (index != -1)
    {
        command_script_datum *script = command_script_get(index);
        long next = script->next_cs;
        bool remove = false;
        if (script->thread_index == -1)
        {
            remove = true;
        }
        else if (script->abort)
        {
            remove = true;
        }
        else
        {
            hs_thread *thread = hs_thread_get(script->thread_index);
            if (thread->sleep_until != -1 && (thread->flags & 0x80) == 0)
            {
                if (actor->active != 0)
                {
                    bool abort = false;
                    if (actor->combat_status >= 2 && ((thread->flags2 & 2) != 0 || script->abort_on_alert))
                    {
                        remove = true;
                        abort = true;
                    }
                    else if (((thread->flags2 & 1) != 0 || script->abort_on_damage) && (actor->body_damage > 0.0f
                        || actor->shield_damage > 0.0f))
                    {
                        remove = true;
                        abort = true;
                    }
                    else if (((thread->flags2 & 4) != 0 && ((signed char)thread->abort_combat_status >= 0
                        && (signed char)thread->abort_combat_status < 10 && actor->combat_status >= (signed char)thread->abort_combat_status))
                        || (script->abort_on_combat_status && (script->abort_on_combat_status_level >= 0
                            && script->abort_on_combat_status_level < 10 && actor->combat_status >= script->abort_on_combat_status_level)))
                    {
                        remove = true;
                        abort = true;
                    }
                    else if (((thread->flags2 & 8) != 0 || script->abort_on_vehicle_exit) && actor->root_vehicle_index == -1)
                    {
                        remove = true;
                        abort = true;
                    }
                    else if (!script->reserved)
                    {
                        remove = !::cs_atom_executing(actor_index, index, 15);
                    }
                    if (abort)
                        cs_abort_thread(script->thread_index);
                }
            }
            else
            {
                remove = true;
            }
        }
        if (remove)
            cs_remove_command_script(actor_index, index);
        index = next;
    }
    if (actor->command_index != -1)
    {
        command_script_datum *first = command_script_get(actor->command_index);
        if (actor->active != 0 && !first->enable_targeting)
        {
            actor_perception_set_target_inline(actor_index, -1);
        }
        if (!first->enable_moving || ::cs_atom_executing(actor_index, actor->command_index, 7))
            actor->command_script_index = actor->command_index;
    }
}


// @address 0xb70f0 ?cs_actor_execute_command_script@@YAXJJPAFPAVc_behavior_state@@@Z
void cs_actor_execute_command_script(long actor_index, long script_index, short *result, c_behavior_state *behavior_state)
{
    command_script_datum *script = datum_get<command_script_datum>(tls_command_script_data(), script_index);
    short command_count = datum_get<actor_datum>(tls_actor_data(), actor_index)->swarm_index == -1 ? 3 : 1;
    global_new_behavior = result;
    *result = 2;
    global_new_state_data = behavior_state;
    for (short command_index = 0; command_index < command_count; command_index = (short)(command_index + 1))
    {
        cs_command *command = &script->command[command_index];
        if ((command->flags & 1) != 0)
        {
            cs_helper(actor_index, (short)(1 << command_index), cs_command_start, script_index);
            command->flags = command->flags & ~1;
        }
        cs_helper(actor_index, (short)(1 << command_index), cs_command_update, script_index);
    }
}

// @address 0xb7b98 ?cs_atom_executing@@YA_NJJF@Z
bool cs_atom_executing(long actor_handle, long command_script_handle, short atom_mask)
{
    bool result = false;
    if (cs_helper(actor_handle, atom_mask, cs_command_get_status, command_script_handle) == 2) {
        result = true;
    } else {
        actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_handle);
        command_script_datum *command_script = datum_get<command_script_datum>(tls_command_script_data(), command_script_handle);
        short atom_count = actor->swarm ? 1 : 3;
        if (atom_count > 0) {
            short atom_index = 0;
            do {
                if (((1 << atom_index) & atom_mask) != 0 && (command_script->command[atom_index].flags & 1) != 0) {
                    result = true;
                    break;
                }
                atom_index = (short)(atom_index + 1);
            } while (atom_index < atom_count);
        }
    }
    return result;
}

struct squad_group_datum;

// @address 0xd4e10 ?cs_blocked@@YA_NJ@Z
bool cs_blocked(long thread_index)
{
    hs_thread *thread = hs_thread_get(thread_index);
    bool result = false;
    if (thread->sleep_until == -3 && thread->ai_index != 0)
    {
        ai_index_actor_iterator iterator;
        ai_index_actor_iterator_new(thread->ai_index, &iterator);
        while (ai_index_actor_iterator_next(&iterator) != 0)
        {
            long script_index = cs_index_from_thread(iterator.actor_index, thread_index, false);
            if (script_index != -1)
            {
                command_script_datum *script = datum_get<command_script_datum>(tls_command_script_data(), script_index);
                actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), iterator.actor_index);
                short atom_count = actor->swarm ? 1 : 3;
                for (short atom_index = 0; atom_index < atom_count; atom_index++)
                {
                    cs_command *command = &script->command[atom_index];
                    if (((signed char)command->flags & 2) != 0 &&
                        ::cs_atom_executing(iterator.actor_index, script_index, (short)(1 << atom_index)))
                    {
                        result = true;
                        break;
                    }
                }
            }
            if (result)
                break;
        }
    }
    return result;
}

// @address 0xd5140 ?cs_index_from_thread@@YAJJJ_N@Z
long cs_index_from_thread(long actor_index, long thread_index, bool create)
{
    long script_index = -1;
    long index = datum_get<actor_datum>(tls_actor_data(), actor_index)->command_index;
    while (index != -1)
    {
        command_script_datum *script = datum_get<command_script_datum>(tls_command_script_data(), index);
        if (script->thread_index == thread_index)
        {
            script_index = index;
            break;
        }
        index = script->next_cs;
    }
    if (script_index == -1 && create)
    {
        script_index = cs_stack_command_script(actor_index, -1, thread_index);
        if (script_index != -1)
        {
            command_script_get(script_index)->reserved = false;
        }
    }
    return script_index;
}




static __forceinline character_voice_properties *cl_engine_character_voice_properties_get(long tag_index)
{
  character_voice_properties *result = 0;
  while (tag_index != -1) {
    character_definition *definition = TAG_GET('char', character_definition, tag_index);
    if (definition->voice_properties.count > 0) {
      result = (character_voice_properties *)definition->voice_properties.address;
      break;
    }
    tag_index = definition->parent_character.index;
  }
  return result;
}

static __forceinline character_general_properties *cl_engine_character_general_properties_get(long tag_index)
{
  character_general_properties *result = 0;
  while (tag_index != -1) {
    character_definition *definition = TAG_GET('char', character_definition, tag_index);
    if (definition->general_properties.count > 0) {
      result = (character_general_properties *)definition->general_properties.address;
      break;
    }
    tag_index = definition->parent_character.index;
  }
  return result;
}

// @address 0x4fc188 ?actor_can_fill_role@@YAMJJFPBUmission_dialogue_line@@PAUs_role_set@@FPAJPAFPA_N@Z
float actor_can_fill_role(long actor_index, long script_index, short priority, const mission_dialogue_line *line, s_role_set *role_set,
    short role_count, long *out_id, short *out_index, bool *out_allowed)
{
  bool can_fill = true;
  bool found = false;
  bool designated = false;
  float result = 0.0f;
  bool vignette_blocked = false;
  if (out_id)
    *out_id = 0;
  if (out_index)
    *out_index = -1;
  if (out_allowed)
    *out_allowed = false;
  for (short index = 0; index < role_count; index++) {
    if (role_set->actors[index] == actor_index) {
      can_fill = false;
      break;
    }
  }
  if (can_fill)
    can_fill = actor_allow_vignette(actor_index, script_index, priority, &vignette_blocked);
  if (!can_fill)
    return result;
  actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
  if (out_allowed)
    *out_allowed = true;
  if (actor->unit_index == -1)
    return result;
  if (line && line->variants.count() > 0) {
    unit_datum *object = (unit_datum *)object_get(actor->unit_index);
    s_unit_dialogue *dialogue = (s_unit_dialogue *)((char *)object + object->unit.dialogue_offset);
    if (dialogue->dialogue_index != -1) {
      bool role_found = false;
      for (short index = 0; index < role_count; index++) {
        if (role_set->roles[index] == dialogue->designator) {
          role_found = true;
          break;
        }
      }
      if (!role_found) {
        for (short index = 0; index < line->variants.count(); index++) {
          if (line->variants.begin()[index].id == dialogue->designator) {
            *out_id = dialogue->designator;
            found = true;
            designated = true;
            break;
          }
        }
      }
      if (!found) {
        character_voice_properties *voice = cl_engine_character_voice_properties_get(actor->character_index);
        if (voice && voice->voices.count() > 0) {
          for (short line_index = 0; line_index < line->variants.count(); line_index++) {
            const s_ai_mission_dialogue_variant *entry = &line->variants.begin()[line_index];
            bool covered = false;
            for (short role_index = 0; role_index < role_count; role_index++) {
              if (role_set->roles[role_index] == entry->id) {
                covered = true;
                break;
              }
            }
            if (!covered) {
              for (short variant_index = 0; variant_index < voice->voices.count(); variant_index++) {
                s_character_voice *variant = &voice->voices.begin()[variant_index];
                if (variant->designator == entry->id) {
                  *out_id = variant->designator;
                  *out_index = variant_index;
                  found = true;
                  designated = false;
                  break;
                }
              }
            }
            if (found)
              break;
          }
        }
      }
    }
  }
  else {
    designated = true;
    found = true;
  }
  if (found) {
    result = dialogue_player_weighting(actor->unit_index) + 0.1f;
    short value = 0;
    character_general_properties *general = cl_engine_character_general_properties_get(datum_get<actor_datum>(tls_actor_data(),
        actor_index)->character_index);
    if (general)
      value = general->rank;
    if (value > 0 && designated)
      result += 0.2f;
  }
  if (vignette_blocked)
    result *= 0.5f;
  if (designated)
    result += 1.0f;
  return result;
}

// @address 0x4fc588 ?cs_cast_internal@@YA_NJ_NFPAPBUmission_dialogue_line@@FFPAUs_role_set@@PAFFJPAJ3@Z
bool cs_cast_internal(long script_index, bool partial, short priority, const mission_dialogue_line **lines, short role_count,
    short role_index, s_role_set *sets, short *set_count, short max_sets, long ai_index, long *filled_roles, short *actor_count)
{
    s_role_set *set = &sets[*set_count];
    bool last_role = role_index + 1 >= role_count;
    bool result = false;
    short available_count = 0;
    if (actor_count && ai_index != -1) {
        ai_index_actor_iterator iterator;
        ai_index_actor_iterator_new(ai_index, &iterator);
        while (ai_index_actor_iterator_next(&iterator)) {
            bool allowed = false;
            actor_can_fill_role(iterator.actor_index, script_index, priority, 0, 0, -1, 0, 0, &allowed);
            if (allowed)
                available_count++;
        }
    }
    if (ai_index != -1) {
        ai_index_actor_iterator iterator;
        ai_index_actor_iterator_new(ai_index, &iterator);
        while (ai_index_actor_iterator_next(&iterator)) {
            if (*set_count >= max_sets)
                break;
            long designator;
            short variant;
            float weight = actor_can_fill_role(iterator.actor_index, script_index, priority, lines[role_index], set, role_index,
                &designator, &variant, 0);
            if (weight > 0.0f) {
                bool keep = false;
                filled_roles[role_index >> 5] |= 1 << (role_index & 31);
                set->actors[role_index] = iterator.actor_index;
                set->weights[role_index] = weight;
                set->roles[role_index] = designator;
                set->variants[role_index] = variant;
                result = true;
                if (last_role) {
                    keep = true;
                } else {
                    bool success = cs_cast_internal(script_index, partial, priority, lines, role_count, (short)(role_index + 1), sets,
                        set_count, max_sets, ai_index, filled_roles, 0);
                    if (success)
                        set = &sets[*set_count];
                    else
                        keep = partial;
                }
                if (keep) {
                    set->count = (short)(role_index + 1);
                    *set_count = (short)(*set_count + 1);
                    if (*set_count >= max_sets)
                        break;
                    set = &sets[*set_count];
                    memcpy(set, &sets[*set_count - 1], sizeof(*set));
                }
            }
        }
    }
    if (actor_count)
        *actor_count = available_count;
    return result;
}



static __forceinline const mission_dialogue_line *mission_dialogue_get_line_by_name(ai_mission_dialogue *dialogue, long name)
{
    const mission_dialogue_line *result = 0;
    for (short index = 0; index < dialogue->lines.count(); index++) {
        const mission_dialogue_line *line = &dialogue->lines.begin()[index];
        if (line->name == name) {
            result = line;
            break;
        }
    }
    return result;
}

static __forceinline void cs_reset_thread_order(long thread_index)
{
    c_data_iterator<command_script_datum> iterator;
    iterator.m_iterator.data = tls_command_script_data();
    iterator.m_iterator.absolute_index = -1;
    iterator.m_iterator.index = -1;
    while (iterator.next()) {
        command_script_datum *script = iterator.get_datum();
        if (script->thread_index == thread_index)
            script->role_index = -1;
    }
}

static __forceinline void cl_engine_unit_dialogue_set(long unit_index, long dialogue_index, long designator)
{
    unit_datum *unit = (unit_datum *)object_get(unit_index);
    s_unit_dialogue *dialogue = (s_unit_dialogue *)((char *)unit + unit->unit.dialogue_offset);
    dialogue->dialogue_index = dialogue_index;
    dialogue->designator = designator;
}

// @address 0x4fc878 ?cs_cast@@YA_NJJ_NFPAJF@Z
bool cs_cast(long thread_index, long ai_index, bool mandatory, short priority, long *role_names, short role_count)
{
    bool success = false;
    struct scenario *scenario = global_scenario;
    cs_reset_thread_order(thread_index);
    if (scenario->ai_mission_dialogue.count > 0) {
        s_tag_reference *reference = (s_tag_reference *)scenario->ai_mission_dialogue.address;
        if (reference->index != -1) {
            ai_mission_dialogue *dialogue = TAG_GET('mdlg', ai_mission_dialogue, reference->index);
            const mission_dialogue_line *lines[10];
            bool valid = true;
            for (short role_index = 0; role_index < role_count; role_index++) {
                if (role_names[role_index]) {
                    const mission_dialogue_line *line = mission_dialogue_get_line_by_name(dialogue, role_names[role_index]);
                    if (line) {
                        lines[role_index] = line;
                    } else {
                        valid = false;
                        break;
                    }
                } else {
                    lines[role_index] = 0;
                }
            }
            if (valid) {
                s_role_set sets[50];
                long filled_roles[1];
                short set_count = 0;
                short actor_count = 0;
                memset(filled_roles, 0, sizeof(filled_roles));
                for (short set_index = 0; set_index < 50; set_index++)
                    sets[set_index].count = 0;
                cs_cast_internal(thread_index, false, priority, lines, role_count, 0, sets, &set_count, 50, ai_index, filled_roles,
                    &actor_count);
                if (set_count == 0 && !mandatory)
                    cs_cast_internal(thread_index, !mandatory, priority, lines, role_count, 0, sets, &set_count, 50, ai_index, filled_roles, 0);
                if (set_count > 0) {
                    float best_weight = 0.0f;
                    short best_set = -1;
                    for (short set_index = 0; set_index < set_count; set_index++) {
                        s_role_set *set = &sets[set_index];
                        float weight = 0.0f;
                        for (short role_index = 0; role_index < set->count; role_index++)
                            weight += set->weights[role_index];
                        if (weight > best_weight) {
                            best_weight = weight;
                            best_set = set_index;
                        }
                    }
                    if (best_set != -1) {
                        s_role_set *set = &sets[best_set];
                        for (short role_index = 0; role_index < set->count; role_index++) {
                            actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), set->actors[role_index]);
                            unit_datum *unit = (unit_datum *)object_get(actor->unit_index);
                            s_unit_dialogue *unit_dialogue = (s_unit_dialogue *)((char *)unit + unit->unit.dialogue_offset);
                            short variant = set->variants[role_index];
                            if (unit_dialogue->designator != set->roles[role_index] && set->roles[role_index] != 0 && variant != -1) {
                                character_voice_properties *voice = cl_engine_character_voice_properties_get(actor->character_index);
                                if (variant >= 0 && variant < voice->voices.count()) {
                                    s_character_voice *element = &voice->voices.begin()[variant];
                                    cl_engine_unit_dialogue_set(actor->unit_index, element->dialogue.index, set->roles[role_index]);
                                }
                            }
                        }
                        success = true;
                        for (short role_index = 0; role_index < set->count; role_index++) {
                            long script_index = cs_reserve(set->actors[role_index], thread_index, mandatory, priority);
                            if (script_index != -1) {
                                command_script_get(script_index)->role_index = (short)(role_index + 1);
                            } else {
                                success = false;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    if (!success) {
        if (mandatory)
            cs_abort_thread_inline(thread_index);
    }
    return success;
}



static __forceinline void ai_globals_dialogue_suppress(float seconds)
{
    short ticks = (short)game_seconds_to_ticks_round(seconds);
    if (ticks > tls_actor_globals()->dialogue_disabled)
        tls_actor_globals()->dialogue_disabled = ticks;
}


// @address 0xb75a0 ?cs_apply_persistent_state@@YAXJ@Z
void cs_apply_persistent_state(long actor_index)
{
    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
    if (actor->command_index != -1) {
        command_script_datum *script = command_script_get(actor->command_index);
        if (script->shoot_at_target || script->enable_targeting) {
            actor->lower_weapon_blocked_b = true;
            actor->lower_weapon_blocked_c = true;
            actor->direction_dirty = false;
        } else if (script->aim_at_target) {
            if (script->face_lower_weapon) {
                actor->lower_weapon_blocked_c = false;
                actor->direction_dirty = true;
            } else {
                actor->lower_weapon_blocked_c = true;
                actor->direction_dirty = false;
            }
        } else {
            actor->lower_weapon_blocked_c = false;
            actor->direction_dirty = true;
        }
        if (script->shoot_secondary_trigger)
            actor->bypass = true;
        bool aim_locked = false;
        bool look_locked = false;
        if (actor->command_index == actor->command_script_index) {
            aim_locked = actor->aim_mode >= 2;
            look_locked = actor->look_mode >= 2;
        }
        if (!aim_locked) {
            if (script->look_at_target) {
                bool found = false;
                switch ((short)script->look_target.type) {
                case 1:
                    actor->aim_mode = 2;
                    actor->aim_direction.to_object(script->look_target.index);
                    found = true;
                    break;
                case 2:
                    if (valid_point_ref(script->look_target.index)) {
                        cs_point *point = TAG_BLOCK_GET_ELEMENT(&cs_get_point_set((script->look_target.index >> 16) & 0xffff)->points,
                            script->look_target.index & 0xffff, cs_point);
                        real_point3d position;
                        ai_point_get_position(&point->position, &position);
                        actor->aim_mode = 2;
                        actor->aim_direction.to_point(&position);
                        found = true;
                    }
                    break;
                }
                if (!found) {
                    actor->aim_mode = 2;
                    actor->aim_direction.to_vector(&actor->facing_vector);
                }
            }
        }
        if (!look_locked) {
            if (!script->aim_at_target && (!script->face_at_target || actor->root_vehicle_index == -1)) {
                if (script->face_at_target) {
                    bool found = false;
                    switch ((short)script->aim_target.type) {
                    case 1:
                        if (object_try_and_get_and_verify_type(script->aim_target.index, (unsigned long)-1)) {
                            actor->look_mode = 3;
                            actor->look_direction.to_object(script->aim_target.index);
                            found = true;
                        }
                        break;
                    case 2:
                        if (valid_point_ref(script->aim_target.index)) {
                            cs_point *point = TAG_BLOCK_GET_ELEMENT(&cs_get_point_set((script->aim_target.index >> 16) & 0xffff)->points,
                                script->aim_target.index & 0xffff, cs_point);
                            real_point3d position;
                            ai_point_get_position(&point->position, &position);
                            actor->look_mode = 3;
                            actor->look_direction.to_point(&position);
                            found = true;
                        }
                        break;
                    }
                    if (found)
                        actor->path_aim = script->face_exactly;
                } else if (!script->enable_looking) {
                    actor->look_mode = 2;
                    actor->look_direction.to_movement();
                } else if (script->enable_targeting) {
                    actor->look_mode = 4;
                    actor->look_direction.to_target();
                } else {
                    actor->look_mode = 2;
                    actor->look_direction.to_danger();
                }
            } else {
                bool found = false;
                c_ai_point3d destination;
                switch ((short)script->aim_target.type) {
                case 1:
                    if (object_try_and_get_and_verify_type(script->aim_target.index, (unsigned long)-1)) {
                        actor->look_mode = 4;
                        actor->look_direction.to_object(script->aim_target.index);
                        real_point3d center_of_mass;
                        ai_get_center_of_mass(script->aim_target.index, &center_of_mass);
                        destination.set(&center_of_mass);
                        found = true;
                    }
                    break;
                case 2:
                    if (valid_point_ref(script->aim_target.index)) {
                        cs_point *point = TAG_BLOCK_GET_ELEMENT(&cs_get_point_set((script->aim_target.index >> 16) & 0xffff)->points,
                            script->aim_target.index & 0xffff, cs_point);
                        real_point3d position;
                        ai_point_get_position(&point->position, &position);
                        actor->look_mode = 4;
                        actor->look_direction.to_point(&position);
                        destination = point->position;
                        found = true;
                    }
                    break;
                }
                if (found) {
                    if (script->shoot_at_target) {
                        actor->move_target_type = 2;
                        actor->move_target_point = destination;
                        actor->allow_unseen_target_fire = true;
                        actor->ready = true;
                    }
                } else {
                    actor->look_mode = 2;
                    actor->look_direction.to_movement();
                }
                actor->path_aim = script->face_exactly;
            }
        }
        actor->ignore_obstacles = script->ignore_obstacles;
        if (script->boosting)
            actor_unit_control_boost(actor_index, true);
        if (script->suppress_global_dialogue)
            ai_globals_dialogue_suppress(2.0f);
        if (script->turn_urgency_valid)
            actor->urgency_override = script->turn_urgency;
        else
            actor->urgency_override = 0.0f;
        if (script->vehicle_speed_valid)
            actor->destination_orders.f1c = script->vehicle_speed;
        else
            actor->destination_orders.f1c = 0.0f;
        if (script->animation_mode != -1)
            actor->control_type = script->animation_mode;
        actor->crouch = script->crouch;
        actor->moving_crouch = script->crouch;
        actor->throttle_close |= script->walk;
        if (!cs_atom_executing(actor_index, actor->command_index, 3))
            actor->pose_allowed = true;
    }
}

t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_effect_data_allocator> g_ai_command_script_allocator;
