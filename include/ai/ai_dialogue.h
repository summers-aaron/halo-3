#ifndef AI_AI_DIALOGUE_H
#define AI_AI_DIALOGUE_H

// Functions defined in tu/ailibxbox/ai_dialogue.cpp.

struct ai_dialogue_definitions;
class c_dialogue_event;
class c_dialogue_option;
struct team_dialogue_state;
struct unit_speech_item;

ai_dialogue_definitions *ai_dialogue_globals_get();
short ai_dialogue_get_involuntary_vocalization_index(short involuntary_index);
void ai_dialogue_event_aggregate_options(c_dialogue_event *event, c_dialogue_option *options, short *option_out, short);
short ai_dialogue_event(c_dialogue_option *options, short option_count);
bool ai_dialogue_event(c_dialogue_event *event);
void dialogue_emotion(long actor_index, short emotion, short emotion_type);
team_dialogue_state *team_dialogue_state_get(short team);
void ai_dialogue_started(long unit_index, unit_speech_item *item);
float dialogue_player_weighting(long unit_index);
extern "C" bool ai_dialogue_trigger_actor_scream(long actor_index, short type);
extern "C" bool ai_dialogue_event_has_valid_pattern(c_dialogue_event *event);

struct vocalization_pattern;

void __tls_set_g_team_dialogue_state_allocator(void *allocator);
void __tls_set_g_vocalization_record_allocator(void *allocator);
bool pattern_match_ai_type_name(long object_index, long name, float * weight);
bool ai_dialogue_event_internal(ai_dialogue_definitions *definitions, c_dialogue_event *event, c_dialogue_option *options,
    short maximum_options, short *option_out);
bool dialogue_speak(ai_dialogue_definitions *dialogue_globals, long parameter, c_dialogue_option *option, long *queue_index);
short dialogue_choose_option(c_dialogue_option *options, short option_count);
bool ai_dialogue_event_evaluate_pattern(ai_dialogue_definitions *definitions, c_dialogue_event *event, vocalization_pattern *pattern,
    c_dialogue_option *options, short maximum_options, short *option_out);
extern "C" bool pattern_match_spatial_relation(long object_index, long other_index, short range_type);
short dialogue_unit_can_vocalize(ai_dialogue_definitions *definitions, c_dialogue_event *event, vocalization_pattern *pattern,
    long unit_index, long *sound_definition_index_reference, float *vocalization_factor_reference);

#endif // AI_AI_DIALOGUE_H
