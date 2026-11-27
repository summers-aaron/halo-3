#ifndef AI_CL_ENGINE_DATA_H
#define AI_CL_ENGINE_DATA_H

struct cs_command;
struct s_atom_control;

typedef short (*cl_engine_command_function)(long,long,cs_command*,s_atom_control*,long);
typedef void (*cl_engine_message_handler)(long actor_index,long script_index,s_atom_control *control,long argument);
typedef bool (*cl_engine_atom_start)(long,long,cs_command*,s_atom_control*,long);
typedef void (*cl_engine_atom_destroy)(long actor_index,long script_index,long thread_index);

// Per-atom-type vtable: start/update/handle/destroy selected by cs_command::atom_type.
struct s_atom_definition
{
    cl_engine_atom_start start;
    cl_engine_command_function update;
    cl_engine_message_handler handle;
    cl_engine_atom_destroy destroy;
};

// Per-atom-type side table parallel to g_atom_definitions.
struct s_atom_info
{
    long pad0; // never read by cl_engine.obj; March 360cache_debug has no name for it
    long slot_index;
    unsigned char swarm_allowed;
};

struct ai_script_context;
extern s_atom_definition *g_atom_definitions[];
extern s_atom_info g_atom_info[];
extern ai_script_context global_script_context;

#endif // AI_CL_ENGINE_DATA_H
