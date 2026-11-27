#ifndef UNITS_MOTOR_REQUEST_H
#define UNITS_MOTOR_REQUEST_H

#include <stddef.h>

#include "math/real_math_types.h"
#include "units/motor_enums.h"

// A motor task, its initiating action, and task-specific parameters.
class c_motor_request
{
public:
    // Task and payload are initialized by the setup operation.
    c_motor_request() : m_valid(false), m_force(false) {}

    bool valid() const { return m_valid; }
    bool force() const { return m_force; }
    void set_force(bool force) { m_force = force; }

    void setup_stun(e_action action_index)
    {
        action = action_index;
        m_task = _motor_task_stun;
        m_valid = true;
    }

    void setup_accelerate(e_action action_index, real_vector3d *acceleration)
    {
        action = action_index;
        m_task = _motor_task_accelerate;
        m_accelerate.acceleration = *acceleration;
        m_valid = true;
    }

    void setup_notify_physics_built()
    {
        action = _action_none;
        m_task = _motor_task_notify_physics_built;
        m_valid = true;
    }

    void setup_handle_simulation_warp(e_action action_index)
    {
        action = action_index;
        m_task = _motor_task_handle_simulation_warp;
        m_valid = true;
    }

    void setup_force_airborne(e_action action_index)
    {
        action = action_index;
        m_task = _motor_task_force_airborne;
        m_valid = true;
    }

    void setup_force_heavy(e_action action_index, bool heavy)
    {
        action = action_index;
        m_task = _motor_task_force_heavy;
        m_force_heavy.heavy = heavy;
        m_valid = true;
    }

    void setup_die(e_action action_index, bool relax)
    {
        action = action_index;
        m_task = _motor_task_die;
        m_die.relax = relax;
        m_valid = true;
    }

    // March: relaxation_type is c_motor_request::e_force_relaxation_type (its values are not attested).
    void setup_force_relaxation_state(e_action action_index, long relaxation_type, bool interrupt)
    {
        action = action_index;
        m_task = _motor_task_force_relaxation_state;
        m_force_relaxation_state.relaxation_type = relaxation_type;
        m_force_relaxation_state.interrupt = interrupt;
        m_valid = true;
    }

    void setup_land(e_action action_index, const real_point3d *point, const real_vector3d *vector, float scale)
    {
        action = action_index;
        m_task = _motor_task_land;
        m_land.scale = scale;
        if (point && vector)
        {
            m_land.point = *point;
            m_land.vector = *vector;
            m_land.has_point = true;
        }
        else
        {
            m_land.has_point = false;
        }
        m_valid = true;
    }

    void setup_open(e_action action_index, bool immediate, bool pause)
    {
        action = action_index;
        m_task = _motor_task_open;
        m_open.immediate = immediate;
        m_open.pause = pause;
        m_valid = true;
    }

    void setup_weapon_stow(e_action action_index)
    {
        action = action_index;
        m_task = _motor_task_weapon_stow;
        m_valid = true;
    }

    void setup_weapon_draw(e_action action_index, bool alternate, bool instant)
    {
        action = action_index;
        m_task = _motor_task_weapon_draw;
        m_weapon_draw.alternate = alternate;
        m_weapon_draw.instant = instant;
        m_valid = true;
    }

    void setup_grenade(e_action action_index)
    {
        action = action_index;
        m_task = _motor_task_grenade;
        m_valid = true;
    }

    void setup_exit(e_action action_index, bool animate, bool exit_to_default, bool seat_exit_flag_1, bool immediate)
    {
        action = action_index;
        m_task = _motor_task_exit;
        m_exit.exit_to_default = exit_to_default;
        m_exit.immediate = immediate;
        m_exit.animate = animate;
        m_exit.seat_exit_flag_1 = seat_exit_flag_1;
        m_valid = true;
    }

    void setup_animation(e_action action_index, long mode, long animation, float interpolation,
        const real_vector3d *facing, bool skip_default_mode)
    {
        action = action_index;
        m_task = _motor_task_animation;
        m_animation.animation = animation;
        m_animation.mode = mode;
        m_animation.interpolation = interpolation;
        m_animation.skip_default_mode = skip_default_mode;
        if (facing)
        {
            m_animation.has_facing = true;
            m_animation.facing = *facing;
        }
        else
        {
            m_animation.has_facing = false;
        }
        m_valid = true;
    }

    // March: config is e_unit_weapon_config (no shared header defines it yet).
    void setup_weapon_animation(e_action action_index, long animation, long config, float interpolation)
    {
        action = action_index;
        m_task = _motor_task_weapon_animation;
        m_weapon_animation.animation = animation;
        m_weapon_animation.config = config;
        m_weapon_animation.interpolation = interpolation;
        m_valid = true;
    }

    // Setup operations owned by actions.cpp.

    void setup_leap_anticipation(e_action action_index, long animation_name, long animation_index,
        float interpolation, const real_vector3d *direction);
    __forceinline void setup_dash(long action_index, long target_object_index, long ticks, bool flag)
    {
        action = (e_action)(action_index);
        m_task = _motor_task_dash;
        dash.target_object_index = target_object_index;
        dash.ticks = ticks;
        dash.flag = flag;
        m_valid = true;
    }
    __forceinline void setup_melee(long action_index, long animation_name, const real_vector2d *direction, bool flag)
    {
        action = (e_action)(action_index);
        m_task = _motor_task_melee;
        melee.animation = animation_name;
        if (direction) { melee.vector = *direction; melee.has_vector = true; }
        else melee.has_vector = false;
        melee.flag = flag;
        m_valid = true;
    }
    __forceinline void setup_resurrect(long a, short p, short q)
    {
        action = (e_action)(a);
        m_task = _motor_task_resurrect;
        resurrect.first=p;
        resurrect.second=q;
        m_valid=true;
    }
    __forceinline void setup_animation(long a, long b, long c, float d, const real_point3d *v, bool e)
    {
        action = (e_action)(a);
        m_task = _motor_task_animation;
        animation.second=c;
        animation.first=b;
        animation.value=d;
        animation.extra=e;
        if(v) { animation.has_vector=true; animation.vector=*v; }
        else animation.has_vector=false;
        m_valid=true;
    }
    __forceinline void setup_posture(long action_, long posture_, const real_point3d *position_,
        const real_vector3d *forward_, bool a_, bool b_)
    {
        action = (e_action)(action_);
        m_task = _motor_task_posture;
        posture.value=posture_;
        posture.position=*position_;
        posture.forward=*forward_;
        posture.a=a_;
        posture.b=b_;
        m_valid=true;
    }
    __forceinline void setup_jump(long action_index, unsigned char flag, const real_point2d *point)
    {
        action = (e_action)(action_index);
        m_task = _motor_task_jump;
        if(point)
        {
        jump.point=*point;
        jump.has_point=true;
        }
        else jump.has_point=false;
        jump.flag=flag;
        m_valid=true;
    }
    __forceinline void setup_ping(long action_index, signed char ping_type, signed char region, short direction, const real_vector3d *vector)
    {
        action = (e_action)(action_index);
        m_task = _motor_task_ping;
        ping.type = (e_motor_task)(ping_type);
        ping.region=region;
        ping.direction=direction;
        if(vector) { ping.has_vector=true; ping.vector=*vector; }
        else ping.has_vector=false;
        m_valid=true;
    }
    __forceinline void setup_surge(long action_index, float value)
    {
        action = (e_action)(action_index);
        m_task = _motor_task_surge;
        surge.value=value;
        m_valid=true;
    }
    __forceinline void setup_board(long action_index)
    {
        m_force=false;
        m_valid=true;
        action = (e_action)(action_index);
        m_task = _motor_task_board;
    }
    __forceinline void setup_transient_posture(long a, long anim, long g, const real_point3d *point, const real_vector3d *vector)
    {
        action = (e_action)(a);
        m_task = _motor_task_transient_posture;
        transient_posture.animation=anim;
        transient_posture.goal=g;
        transient_posture.position=*point;
        transient_posture.alignment=*vector;
        m_valid=true;
    }
    __forceinline void setup_hoist(long a, long height, const real_point3d *point, const real_vector3d *vector)
    {
        action = (e_action)(a);
        m_task = _motor_task_hoist;
        hoist.hoist=height;
        hoist.position=*point;
        hoist.alignment=*vector;
        m_valid=true;
    }
    __forceinline void setup_phase(long a, const real_point3d *point, float t)
    {
        action = (e_action)(a);
        m_task = _motor_task_phase;
        phase.destination=*point;
        phase.duration=t;
        m_valid=true;
    }
    __forceinline void setup_leap_recovery(long a, long b, long c, float t, const real_vector3d *v)
    {
        action = (e_action)(a);
        m_task = _motor_task_leap_recovery;
        leap.second=c;
        leap.first=b;
        leap.value=t;
        if(v) { leap.has_vector=true; leap.vector=*v; }
        else leap.has_vector=false;
        m_valid=true;
    }
    __forceinline void setup_land(long action_index,const real_point3d *point,const real_vector3d *vector,float scale)
    {
        action = (e_action)(action_index);
        m_task = _motor_task_land;
        land.scale=scale;
        if(point && vector)
        {
        land.point=*point;
        land.vector=*vector;
        land.has_point=true;
        }
        else land.has_point=false;
        m_valid=true;
    }
    __forceinline void setup_play_animation(long action_index, long graph, long animation, bool custom, bool loop, long object,
        const real_point3d *point, const real_vector3d *vector, bool interpolate, bool extra_arg)
    {
        action = (e_action)(action_index);
        m_task = _motor_task_custom_animation;
        play_animation.graph=graph;
        play_animation.animation=animation;
        play_animation.custom=custom;
        play_animation.object_index=object;
        play_animation.interpolate=interpolate;
        play_animation.loop=loop;
        if(object==-1 && point && vector) { play_animation.position=*point; play_animation.forward=*vector; play_animation.use_position=true; }
        else play_animation.use_position=false;
        play_animation.extra=extra_arg;
        m_valid=true;
    }

    bool m_valid;
    bool m_force;

    e_motor_task m_task;
    e_action action;
    union
    {
        struct { bool takeoff; bool align; real_vector2d direction; } m_jump;                       // 1
        struct { real_vector3d acceleration; } m_accelerate;                                                        // 3
        struct { long target_unit_index; long ticks; bool flag; } m_dash;                                            // 4
        struct { long relaxation_type; bool interrupt; } m_force_relaxation_state;                                   // 8
        struct { bool heavy; } m_force_heavy;                                                                        // 9
        struct { bool eject; bool finished; } m_ejection;                                                            // 11
        struct { bool takeoff; } m_takeoff;                                                                          // 12
        struct { short perch_type; real_point3d position; real_vector3d direction; } m_perch;        // 13
        struct { bool has_point; real_point3d point; real_vector3d vector; float scale; } m_land;     // 14
        struct { real_point3d point; real_vector2d vector; bool flag; } m_attach;                                    // 15
        struct { long pad_0c; real_vector3d alignment; real_point3d position; } m_detach;                            // 16
        struct { real_point3d position; real_vector3d forward; short phase; } m_bunker;                               // 17, 18
        struct { long animation; real_vector2d direction; bool use_direction; bool skip_reset; } m_melee;             // 19
        struct { short first; short second; } resurrect;                                                             // 20 (March: m_resurrect)
        struct { long target_unit_index; } m_get_infected;                                                           // 21
        struct { long target_object_index; short ticks; } m_infect;                                                 // 22
        struct { bool immediate; bool pause; } m_open;                                                               // 23, 24
        struct { long owner_index; } m_flip;                                                                         // 25
        struct { long vehicle_index; short seat_index; bool animate; } m_unit_enter;                                 // 26
        struct { long animation; long config; float interpolation; } m_weapon_animation;                            // 27
        struct { bool alternate; bool instant; } m_weapon_draw;                                                      // 29
        struct { bool exit_to_default; bool immediate; bool animate; bool seat_exit_flag_1; } m_exit;                // 31
        struct { bool exit_to_default; bool immediate; bool animate; } m_posture_exit;                               // 32
        struct
        {
            signed char type;
            signed char region;
            short direction;
            bool align;

            real_vector3d facing;
        } m_ping;                                                                                                    // 33
        struct { bool relax; } m_die;                                                                                // 34
        struct
        {
            long mode;
            long animation;
            float interpolation;
            bool has_facing;
            bool skip_default_mode;

            union
            {
                real_vector3d facing;               // full facing vector, written by setup_animation
                real_vector2d direction;            // horizontal facing, read by ground animation
            };
        } m_animation;                                                                                               // 35, posture animation
        struct { long posture; real_point3d position; real_vector3d forward; bool immediate; bool exit_to_default; } m_posture; // 36
        struct { long animation; long mode; real_point3d position; real_vector3d alignment; } m_transient_posture;    // 37
        struct { long hoist; real_point3d position; real_vector3d alignment; } m_hoist;                               // 38
        struct
        {
            long graph_tag_index;
            long animation_name;
            bool interpolate;

            long object_index;
            bool goal;
            bool clear_custom;
            bool use_position;

            real_point3d position;
            real_vector3d alignment;
            bool blend_in;
        } m_custom_animation;                                                                                        // 39
        struct { float duration; } m_surge;                                                                          // 44
        struct { real_point3d position; float duration; } m_phase;                                                   // 45

        struct { bool has_point; real_point3d point; real_vector3d vector; float scale; } land;
        struct { unsigned char flag; bool has_point; real_point2d point; } jump;
        struct { long value; short index; char mode; } update;
        struct { unsigned char a, b, c, d; } bytes;
        struct { unsigned short value; real_point3d position; real_vector3d velocity; } motion;
        struct { real_point3d position; real_vector3d velocity; unsigned short value; } vectors;
        struct { long pad; real_point3d a; real_vector3d b; } aligned_vectors;
        struct { real_point3d point; real_vector2d vector; bool flag; } attach;
        struct { long first, second; float value; bool has_vector, extra; real_point3d vector; } animation;
        struct { long first, second; float value; bool has_vector; real_vector3d vector; } leap;
        struct { long animation; real_vector2d vector; bool has_vector, flag; } melee;
        struct { long animation; long effect; float interpolation; unsigned char a, b; } flurry;
        struct { float value; } surge;
        struct { long value; real_point3d position; real_vector3d forward; bool a, b; } posture;
        struct
        {
            long graph, animation;
            bool custom;

            long object_index;
            bool interpolate, loop, use_position;

            real_point3d position;
            real_vector3d forward;
            bool extra;
        } play_animation;
        struct { long animation, goal; real_point3d position; real_vector3d alignment; } transient_posture;
        struct { long hoist; real_point3d position; real_vector3d alignment; } hoist;
        struct { real_point3d destination; float duration; } phase;
        struct { long target_object_index; long ticks; bool flag; } dash;
        struct { signed char type, region; short direction; bool has_vector; real_vector3d vector; } ping;
    };

};

typedef char motor_request_size[(sizeof(c_motor_request) == 0x3c) ? 1 : -1];
typedef char motor_request_detach_position_offset[(offsetof(c_motor_request, m_detach.position) == 0x1c) ? 1 : -1];
typedef char motor_request_animation_direction_offset[(offsetof(c_motor_request, m_animation.direction) == 0x1c) ? 1 : -1];
typedef char motor_request_infect_target_offset[(offsetof(c_motor_request, m_infect.target_object_index) == 0xc) ? 1 : -1];
typedef char motor_request_infect_ticks_offset[(offsetof(c_motor_request, m_infect.ticks) == 0x10) ? 1 : -1];
typedef char motor_request_posture_immediate_offset[(offsetof(c_motor_request, m_posture.immediate) == 0x28) ? 1 : -1];
typedef char motor_request_posture_exit_offset[(offsetof(c_motor_request, m_posture.exit_to_default) == 0x29) ? 1 : -1];

typedef char motor_request_m_task_layout[(offsetof(c_motor_request, m_task) == 0x4) ? 1 : -1];
typedef char motor_request_action_layout[(offsetof(c_motor_request, action) == 0x8) ? 1 : -1];
typedef char motor_request_m_jump_direction_layout[(offsetof(c_motor_request, m_jump.direction) == 0x10) ? 1 : -1];
typedef char motor_request_m_perch_position_layout[(offsetof(c_motor_request, m_perch.position) == 0x10) ? 1 : -1];
typedef char motor_request_m_land_point_layout[(offsetof(c_motor_request, m_land.point) == 0x10) ? 1 : -1];
typedef char motor_request_m_ping_facing_layout[(offsetof(c_motor_request, m_ping.facing) == 0x14) ? 1 : -1];
typedef char motor_request_m_animation_facing_layout[(offsetof(c_motor_request, m_animation.facing) == 0x1c) ? 1 : -1];
typedef char motor_request_m_custom_animation_object_index_layout[(offsetof(c_motor_request, m_custom_animation.object_index) == 0x18) ? 1 : -1];
typedef char motor_request_m_custom_animation_position_layout[(offsetof(c_motor_request, m_custom_animation.position) == 0x20) ? 1 : -1];
typedef char motor_request_m_custom_animation_blend_in_layout[(offsetof(c_motor_request, m_custom_animation.blend_in) == 0x38) ? 1 : -1];
typedef char motor_request_jump_point_layout[(offsetof(c_motor_request, jump.point) == 0x10) ? 1 : -1];
typedef char motor_request_motion_position_layout[(offsetof(c_motor_request, motion.position) == 0x10) ? 1 : -1];
typedef char motor_request_animation_vector_layout[(offsetof(c_motor_request, animation.vector) == 0x1c) ? 1 : -1];
typedef char motor_request_leap_vector_layout[(offsetof(c_motor_request, leap.vector) == 0x1c) ? 1 : -1];
typedef char motor_request_play_animation_position_layout[(offsetof(c_motor_request, play_animation.position) == 0x20) ? 1 : -1];
typedef char motor_request_ping_vector_layout[(offsetof(c_motor_request, ping.vector) == 0x14) ? 1 : -1];

#endif // UNITS_MOTOR_REQUEST_H
