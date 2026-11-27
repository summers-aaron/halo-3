#ifndef OBJECTS_OBJECT_LIFECYCLE_H
#define OBJECTS_OBJECT_LIFECYCLE_H

struct s_scenario_object;
struct s_tag_block;

// Functions defined in tu/objectslibxbox/objects.cpp.
// Object creation from a scenario placement takes eight arguments.
extern "C" long object_new_from_scenario_internal(long object_type, short scenario_index,
    s_scenario_object *scenario_object, s_tag_block *palette, long definition_override,
    bool query_simulation, bool force, bool cinematic);
extern "C" void objects_purge_deleted_object(long object_index);

#endif // OBJECTS_OBJECT_LIFECYCLE_H
