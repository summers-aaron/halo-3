#ifndef AI_AI_DIALOGUE_QUEUE_H
#define AI_AI_DIALOGUE_QUEUE_H

// Functions defined in tu/ailibxbox/ai_dialogue_queue.cpp that other units call.

struct ai_information_packet;
class c_dialogue_event;
struct team_dialogue_state;

void multi_deletion_remove_marked(team_dialogue_state *team);
float ai_dialogue_queue_unit_vocalization_weight(long unit_index, short vocalization_index);
bool queue_remove_record(struct team_dialogue_state * list, long index);
void record_delete(long index);
void ai_dialogue_queue_update(void);
void ai_dialogue_queue_vocalization_interrupted(long unit_index, long vocalization_index);
void ai_dialogue_queue_handle_object_deleted(long object_index);
long ai_queue_dialogue(c_dialogue_event *event, long unit_index, short vocalization, long sound_index, long delay,
    long response_index, ai_information_packet *information);

#endif // AI_AI_DIALOGUE_QUEUE_H
