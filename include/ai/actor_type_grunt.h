#ifndef AI_ACTOR_TYPE_GRUNT_H
#define AI_ACTOR_TYPE_GRUNT_H

// Functions defined in tu/ailibxbox/actor_type_grunt.cpp.

class c_behavior_state;
struct joint_state_datum;

void behavior_scared_by_leader_deactivate(long actor_index, c_behavior_state *behavior_state, joint_state_datum *joint_state);
void behavior_scared_by_leader_control(long actor_index, c_behavior_state *behavior_state, joint_state_datum *joint_state);
void behavior_scared_by_leader_flush_actor(long actor_index, c_behavior_state *behavior_state, long flushed_actor_index);

#endif // AI_ACTOR_TYPE_GRUNT_H
