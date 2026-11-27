#ifndef AI_ACTIVITIES_H
#define AI_ACTIVITIES_H

// Functions defined in tu/ailibxbox/activities.cpp that other units call.

struct behavior_posture_info;
behavior_posture_info *activities_try_and_get_posture(int key);

struct s_activity_info {
    long name;
    long posture;
    long data[2];
};
extern const s_activity_info g_activity_info[];
extern short g_activity_exit_combat_status[];

struct s_tag_block;

extern "C" unsigned char activity_index_get_by_name(int activity_name, short *found_posture, short *found_variant);
bool actor_activity_set(long actor_datum_index, short new_activity_index, short activity_variant,
    short new_activity_parameter, bool new_activity_flag);
s_tag_block *actor_get_patrol_block(long actor_index, short *patrol_point_count);

#endif // AI_ACTIVITIES_H
