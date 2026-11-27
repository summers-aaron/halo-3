#include "objects/objects_tracked_object_types.h"
#ifndef OBJECTS_OBJECT_GLOBALS_H
#define OBJECTS_OBJECT_GLOBALS_H

#include "objects/cinematic_light_globals.h"
#include "objects/object_runtime_globals_types.h"

#include "objects/object_memory_pool.h"
#include "objects/object_recycling_storage.h"

#include "objects/object_name_list.h"

#include "objects/light_globals.h"
#include "objects/object_render_state_tls.h"

#include "objects/widgets/widgets.h"

#include "structures/cluster_partition_types.h"
#include "objects/object_cluster_globals.h"

// Runtime storage used by the object system. Definitions retain their unit address tags.
struct s_memory_pool;
struct s_object_globals;
struct s_object_marker_globals;
struct s_data_array;
struct cluster_partition;
struct groups;
struct s_depth_of_field;
struct s_object_render_thread_message;
struct real_orientation;
extern "C++" {
template<class t_message, long k_count> class t_message_queue;
}

extern "C" {
extern const float g_object_world_coordinate_bounds[];
extern unsigned long g_object_types_deleted_object_mask;
extern s_object_marker_globals g_object_marker_globals[];
extern __declspec(thread) t_message_queue<s_object_render_thread_message, 2048> *g_object_message_queue;
extern __declspec(thread) long *g_havok_memory_task_data;
extern real_orientation g_object_node_orientation_scratch[255];
}

// Called with each object index as it is deleted (object_deleted_procs.obj).
extern void (*object_deleted_procs[11])(long);

#endif // OBJECTS_OBJECT_GLOBALS_H
