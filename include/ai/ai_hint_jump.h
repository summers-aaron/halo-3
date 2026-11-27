#ifndef AI_AI_HINT_JUMP_H
#define AI_AI_HINT_JUMP_H

class c_sector_ref;
union real_point3d;
struct hint_jump_data;
extern "C" unsigned char ai_hint_get_jump_destination_rail(short bsp, hint_jump_data *jump, const real_point3d *point,
    real_point3d *start, real_point3d *end, c_sector_ref *sector, short *hint);

#endif // AI_AI_HINT_JUMP_H
