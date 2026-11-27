#ifndef AI_AI_DIALOGUE_QUEUE_DATA_H
#define AI_AI_DIALOGUE_QUEUE_DATA_H

// One row of global_dialogue_priorities (ai_dialogue.obj), 24 bytes. The queue reads only the leading
// halfwords; the March 2007 builds name neither the four floats nor the trailing short, so they stay unnamed.
struct s_priority_definition
{
    short speech_type;
    short priority;
    float unnamed_floats[4];
    short unnamed_short;
};
typedef char dialogue_priority_definition_size[(sizeof(s_priority_definition) == 24) ? 1 : -1];
extern const s_priority_definition global_dialogue_priorities[16];
extern "C" void ai_dialogue_notify_vocalization(long unit_index, short vocalization_index, long record_index, char *information);

#endif // AI_AI_DIALOGUE_QUEUE_DATA_H
