// interfacelibxbox/chud_definitions.obj: from the records 0x136268 0x4a3998 0x4a3a30
// @flags /TP /O2 /J /Oc
#include "tag_files/tag_groups.h"
#include "interface/chud/chud_definitions.h"

long s_chud_widget_animation_data::compute_type_duration_msec(long animation_index, bool looping)
{
    int duration_msec = 0;
    int handle = animations[animation_index].animation.index;
    if (handle != -1) {
        s_chud_animation_definition *definition = (s_chud_animation_definition *)tag_get('chad', handle);
        duration_msec = definition->period_msec;
        if (looping && !(definition->flags & 2) && !(definition->flags & 4))
            duration_msec = -1;
        if (!looping && ((definition->flags & 2) || (definition->flags & 4)))
            duration_msec = -1;
    }
    return duration_msec;
}

static __forceinline long chud_animation_duration(s_chud_widget_animation_data *anim, long index, bool unused)
{
    if (anim) return anim->compute_type_duration_msec(index, unused);
    return 0;
}
// @address 0x136268 ?compute_state_duration_msec@s_chud_widget_base@@QAAJJ_N@Z
long s_chud_widget_base::compute_state_duration_msec(long state, bool unused)
{
    s_chud_widget_animation_data *anim = animations.count ? (s_chud_widget_animation_data *)animations.address : 0;
    long result = 0;
    switch (state) {
    case 0: result = -1; break;
    case 1: result = chud_animation_duration(anim, 0, unused); break;
    case 2: result = anim ? anim->compute_type_duration_msec(1, unused) : -1; break;
    case 3: result = chud_animation_duration(anim, 2, unused); break;
    case 4: result = chud_animation_duration(anim, 3, unused); break;
    case 5: result = chud_animation_duration(anim, 4, unused); break;
    case 6: result = chud_animation_duration(anim, 5, unused); break;
    }
    return result;
}

// @address 0x4a3a30 chud_animation_interpolate_reals
void chud_animation_interpolate_reals(const float *a, const float *b, float t, float *out, long count)
{
    for (long i = 0; i < count; i++)
        out[i] = a[i] + (b[i] - a[i]) * t;
}
