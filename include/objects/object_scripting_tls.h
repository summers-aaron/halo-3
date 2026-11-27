#ifndef OBJECTS_OBJECT_SCRIPTING_TLS_H
#define OBJECTS_OBJECT_SCRIPTING_TLS_H

struct s_object_scripting_state;

extern "C" __declspec(thread) s_object_scripting_state *g_object_scripting_state;

#endif // OBJECTS_OBJECT_SCRIPTING_TLS_H
