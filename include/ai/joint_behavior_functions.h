#ifndef AI_JOINT_BEHAVIOR_FUNCTIONS_H
#define AI_JOINT_BEHAVIOR_FUNCTIONS_H

class c_behavior_state;
class c_joint_behavior_data;

// Functions defined in tu/ailibxbox/joint_behavior.cpp.

struct invitation_data;
struct joint_invitation_iterator;

void actor_invitations_update(long actor_index);
bool actor_invited(long actor_index, short invitation_type);
invitation_data *joint_invitation_iterator_next(joint_invitation_iterator *iterator);
bool invite_actor(long inviter_index, long actor_index, long joint_index, short invitation_type, float weight);
void joint_state_update(long i);
long joint_new(long actor_index, short behavior_index, c_joint_behavior_data *joint_behavior);
long joint_accept(long actor_index, invitation_data *invitation, c_joint_behavior_data *joint_behavior);
bool joint_decline(invitation_data *invitation);
extern "C" void joint_withdraw(long actor_index, c_joint_behavior_data *data);
void __tls_set_g_joint_data_allocator(void *allocator);
extern "C" void joint_reject(long index, short slot_index);
void choose_participants(long joint_index, short minimum_participants, short maximum_participants);
void joint_primitive_behavior_deactivate(long a, c_behavior_state *state);
bool joint_mutex_group_activate(long a, c_behavior_state *state);
void joint_mutex_group_deactivate(long a, c_behavior_state *state);
void joint_mutex_group_debug(long actor_index, c_behavior_state *state);
bool joint_behavior_get_participant_limits(short behavior_index, short *minimum_participants, short *maximum_participants);
short joint_mutex_group_decide(long index, short layer, c_behavior_state *state);

#endif // AI_JOINT_BEHAVIOR_FUNCTIONS_H
