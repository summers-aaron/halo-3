#ifndef EFFECTS_EFFECT_COUNTS_H
#define EFFECTS_EFFECT_COUNTS_H

struct s_effect_counts
{
    long effect_count;
    long event_count;
    long location_count;
};

extern "C" { extern __declspec(thread) s_effect_counts *g_effect_counts; }

#endif // EFFECTS_EFFECT_COUNTS_H
