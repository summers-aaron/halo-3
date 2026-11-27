#ifndef INTERFACE_CHUD_CHUD_DIRECTIONAL_DAMAGE_H
#define INTERFACE_CHUD_CHUD_DIRECTIONAL_DAMAGE_H

#include "interface/chud/chud_point_tracker_types.h"
#include "game/game_time_inline.h"

struct s_directional_damage
{
    c_chud_point_tracker tracker;
    real_vector3d direction;
    long end_time, start_time;
    short type;

    float intensity;
    float transition;
    float first, second;
    real_argb_color color;
    float alpha;
    float scale;
    float rotation;
};
class c_chud_directional_damage {
public:
    s_directional_damage entries[4];
    void update(long user_index, float dt);
    void start(const real_vector3d *direction, float duration, short type, float intensity,
        float first, float second, real_argb_color *color);
    __forceinline long get_oldest_index() {
        long result=-1;
        long oldest=game_time_get();
        for(long i=0;i<4;++i) {
            s_directional_damage *damage=&entries[i];
            if(damage->start_time<=oldest || result==-1) { result=i; oldest=damage->start_time; }
        }
        return result;
    }
};

#endif
