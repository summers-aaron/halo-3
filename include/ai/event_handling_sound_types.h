#ifndef AI_EVENT_HANDLING_SOUND_TYPES_H
#define AI_EVENT_HANDLING_SOUND_TYPES_H
#include "ai/ai_information_data.h"
// Sound event packet: a two-short header followed by six words of type-specific information.
struct ai_sound_data { short type; short volume; union { char information[24]; ai_information_data information_data; }; };
// actor_perception_initialize_audibility_table writes count then values for each BSP.
struct actor_audibility_bsp { long count; short *values; };
struct audibility_table { void *audibility; actor_audibility_bsp bsps[16]; };
typedef char ai_sound_data_size[(sizeof(ai_sound_data) == 0x1c) ? 1 : -1];
typedef char actor_audibility_bsp_size[(sizeof(void *) != 4 || sizeof(actor_audibility_bsp) == 8) ? 1 : -1];
typedef char audibility_table_size[(sizeof(void *) != 4 || sizeof(audibility_table) == 0x84) ? 1 : -1];
#endif
