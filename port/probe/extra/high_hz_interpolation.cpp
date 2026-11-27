// tmp/probe (milestone 5P-B, GAME-CHANGES M2): render-side interpolation of the 30 Hz game state for frame rates above
// 30 (HALO3_FPS, port/sdk/d3d). PC-only game code, compiled like a game unit; the console build has no such unit.
//
// The game simulates 30 ticks a second. Its main loop runs once per vertical blank and measures the frame's time
// (main_time_update), game_time_update turns that into whole ticks plus a fraction (game_time_globals->leftover_ticks),
// and every frame publishes the game state (restricted region 3: objects, first-person weapons, observer, game time)
// into a mirror the render thread draws from. Between two ticks the mirror holds the same object poses, so at 60 Hz
// and above every other frame would repeat the last. The camera's orientation already follows the player's look
// every frame (player_control_update, observer_update run per frame); its position follows the unit, a tick behind.
//
// render_interpolation_apply runs on the render thread on each published mirror, before main_render
// (process_published_game_state), and rewrites the mirror's tick-driven poses as the blend of the previous tick's and
// the current tick's: display time = time - 1 + leftover_ticks, so the frame shows the state that far between the
// two ticks (one tick of latency for the tick-driven poses, none for the camera's per-frame look). The mirror is the
// render thread's own copy, rebuilt at the next publish: the simulation never sees a blended value. The previous
// tick's poses are the render thread's own record of the last mirror it saw at that tick; nothing is shared with the
// main thread.
//
// Blended: object node matrices (rotation as quaternions, normalized lerp; translation and scale lerp) and the
// object's position and bounding / light sphere centres; first-person weapon and hands node matrices (camera
// relative); the camera position, and the camera orientation and field of view when they move only on ticks
// (scripted and cinematic cameras; a camera whose orientation changed between two frames of one tick is
// look-driven and keeps its current orientation, and moves by the blend of its tick-driven anchor only:
// camera_anchor). Not blended (shown at the current tick): an object or weapon that is new, changed its node count,
// moved more than k_teleport_distance in one tick, or a camera cut; and every frame while game time stands still
// (pause, halt).
//
// The blend assumes a pose moves only on ticks. What moves between two ticks (every main-loop pass) is drawn as it
// is now, or blending it from the record of a tick's first frame would draw it a tick late, catch up by the tick's end
// and drop back at the next: a 30 Hz sawtooth. An object placed between two ticks (a user interface model widget's
// Spartan turning) keeps its placement and has its pose blended in its own frame (blend_objects); a camera's
// per-pass focus offset is taken out of its anchor (camera_anchor), and a camera whose anchor still moves between
// two frames of a tick is drawn as it is (record_and_blend_cameras). The look itself moves every pass, but the game
// clamps it to a vehicle seat's pitch range in the vehicle's frame at the current tick; a look held at that limit is
// drawn clamped to the vehicle as drawn (vehicle_pitch_range_turn).
//
// Region 3 also holds world-space state the main thread derives from those poses and the render thread draws as it
// is. Unblended, a point taken from a moving object leads the drawn (blended) object by up to a tick and steps once a
// tick: a 30 Hz sawtooth against it at every frame between two ticks. Each is shown at the display time like the
// poses it comes from:
// - the HUD's world points (chud persistent user data), which the HUD projects with the drawn camera
//   (chud_draw_navpoints, chud_point_tracker_update_screen_position): navpoint positions and scales
//   (c_chud_navpoint_manager: teammate, companion, objective and scripted navpoints; kept per slot, blended while the
//   slot holds the same id and type) and the tracked-target point of a weapon locking on (point_tracker_position),
//   blended; the directional damage indicators' points, which the chud update places one unit from the published
//   camera, moved with the drawn camera;
// - antennas (antenna_datum: base on an object's marker, spring tip, in world space), blended;
// - the first-person camera estimate (fpw_first_person::camera_matrix), through which effects attached to a
//   first-person weapon resolve its nodes: estimated again from the drawn camera.
// Effects, particles, contrails, beams, light volumes and lens flares need nothing: the render thread resolves them
// from the blended object nodes as it advances and draws them.
#include "cseries/cseries.h"
#include "math/real_math.h"
#include "memory/data.h"
#include "objects/object_datum.h"
#include "objects/object_header_data.h"
#include "objects/light_datum.h"
#include "objects/light_globals.h"
#include "camera/director_globals_types.h"
#include "camera/observer_globals_types.h"
#include "../include/camera/camera_effect_interpolation.h"
#include "objects/objects.h"
#include "interface/first_person_weapon_tls.h"
#include "interface/chud/chud_user_data_types.h"
#include "interface/first_person_weapons.h"
#include "objects/widgets/antenna.h"
#include "game/game_time_types.h"
#include "main/game_tick_time_samples.h"
#include "scenario/scenario.h"
#include <math.h>
#include <string.h>

// port/sdk/d3d/null_device.cpp: whether frames between ticks are interpolated (HALO3_FPS above 30, HALO3_INTERPOLATE).
extern "C" unsigned long port_d3d_frame_interpolation(void);
// tmp/probe/tu/blamlibxbox/following_camera.cpp (GAME-CHANGES "High-Hz camera"): the offset the following camera of a
// user's director moves every main-loop pass (its acceleration offset); false for another camera.
bool following_camera_get_user_acceleration_offset(long user_index, real_vector3d *offset);
// The same unit (GAME-CHANGES "High-Hz camera"): the vehicle whose frame player_control_modify_desired_angles clamps a
// user's look pitch in, and the range; false when the look is not clamped so.
bool player_look_get_user_vehicle_pitch_range(long user_index, long *vehicle_index, real_bounds *range);
// Diagnostics: HALO3_INTERPOLATION_TRACE=1 logs every interpolated frame (port_write_error, stderr).
extern "C" char *getenv(const char *name);
extern "C" void port_write_error(const char *text);

namespace {

const long k_object_capacity = 2048;           // the object data array's size (objects.cpp data_new "object")
const long k_light_capacity = 400;             // the light data array's size (lights.cpp data_new "lights")
const long k_node_capacity = 32768;            // node matrices kept per tick (more than the 0x180000-byte pool holds)
const long k_fp_user_count = 4;
const long k_fp_slot_count = 2;
const long k_fp_node_capacity = 64;            // fpw_weapon::node_matrices
const float k_teleport_distance = 10.0f;       // world units an object moves in one tick (300 / s): a jump, not motion
const float k_camera_cut_distance = 4.0f;      // world units the camera moves in one tick: a cut
const float k_camera_cut_cosine = 0.8660254f;  // a tick-driven camera turning more than 30 degrees in one tick: a cut
const long k_look_driven_ticks = 15;           // a camera stays look-driven this long after its last in-tick turn
const long k_pass_driven_ticks = 2;            // an object or camera anchor stays pass-driven this long after it last
                                               // moved between two frames of one tick
const long k_chud_user_count = 4;              // c_chud_manager::x_persistent_user_data[4]
const long k_navpoint_count = 20;              // c_chud_navpoint_manager::navpoints
const long k_directional_damage_count = 4;     // c_chud_directional_damage::entries
const long k_antenna_capacity = 12;            // the antenna data array's size (antenna.cpp data_new "antenna")

// Display time is one tick behind simulation time, plus the fraction of the current tick.
float tick_blend_fraction(long previous_tick, long tick, float leftover)
{
    const float span = static_cast<float>(tick - previous_tick);
    return (span - 1.0f + PIN(leftover, 0.0f, 1.0f)) / span;
}

struct s_object_record {
    short identifier;                          // the object header's salt; 0: none
    short node_count;
    long first_node;
    real_point3d position, bounding_center, bounding_sphere_center, dynamic_light_sphere_center;
    real_matrix4x3 root;                       // the placement of its attachment tree (object_root_transform)
    bool root_valid;
};

struct s_light_record {
    short identifier;                          // light_datum::identifier; 0: none
    real_point3d position, endpoint, valid_point;
    real_point3d bounding_sphere_center, lod_bounding_sphere_center, collision_sphere_center;
    real_vector3d forward, up;
};

struct s_fp_record {
    long unit_index;
    long objects[3];                           // fpw_weapon::object_indices[2] weapon, [4] weapon model, [6] hands
    long node_count;                           // 0: none
    real_matrix4x3 camera_adjustment;
    real_matrix4x3 nodes[k_fp_node_capacity];
};

struct s_camera_record {
    bool valid;
    real_point3d anchor;                       // camera_anchor: the tick-driven part of the observer's focus
    real_point3d position;
    real_vector3d forward, up;
    real_vector3d last_forward, last_up;
    float horizontal_field_of_view, vertical_field_of_view, last_fov;
};

struct s_navpoint_record {
    bool valid;                                // s_managed_navpoint::valid
    long id;                                   // s_managed_navpoint::id: the submitter's player, object, area or flag
    long type;                                 // s_managed_navpoint::arrow_index (s_chud_navpoint::type)
    real_point3d position;
    float scale;                               // s_managed_navpoint::scale: from the distance to the camera
};

struct s_antenna_record {
    long index;                                // the antenna's datum index; -1: none
    long object_index;
    real_point3d position;                     // antenna_datum::position: the attachment marker
    real_point3d vertices[2];                  // antenna_datum::vertices[].position: base (on the marker) and tip
    real_vector3d acceleration;                // antenna_datum::acceleration: the marker's forward, the base tangent
};

// One user's HUD world points.
struct s_chud_record {
    s_navpoint_record navpoints[k_navpoint_count];
    bool tracking;                             // s_chud_persistent_display_state::point_tracker_enabled
    real_point3d tracked_position;             // s_chud_persistent_display_state::point_tracker_position
};

// The poses of one game tick, as the render thread last saw them.
struct s_tick_record {
    long time;                                 // game time; -1: nothing recorded
    long node_count;
    s_object_record objects[k_object_capacity];
    s_light_record lights[k_light_capacity];
    real_matrix4x3 nodes[k_node_capacity];
    s_fp_record first_person[k_fp_user_count][k_fp_slot_count];
    s_camera_record cameras[4];
    s_antenna_record antennas[k_antenna_capacity];
    s_chud_record chud[k_chud_user_count];
};

const long k_cached_node_capacity = 8192;

struct s_quaternion { float x, y, z, w; };

struct s_cached_node_blend {
    short node_index;
    s_quaternion q_prev;
    s_quaternion q_curr;                       // sign-aligned so dot(q_prev, q_curr) >= 0
    float prev_scale;
    float curr_scale;
    real_point3d prev_pos;
    real_point3d curr_pos;
};

struct s_cached_object_blend {
    short absolute_index;
    short identifier;
    short node_count;
    short cached_node_count;
    long first_cached_node;
    unsigned char pos_mask;                    // bits 0..3: position, bounding, bounding_sphere, dynamic_light_sphere
};

struct s_cached_root_check {
    short absolute_index;
    short identifier;
    float scale;
    real_point3d position;
    real_vector3d forward;
    real_vector3d up;
};

struct s_interpolation_globals {
    bool started;                              // false until the first frame: both records empty
    s_tick_record ticks[2];
    long last;                                 // ticks[last]: the latest tick seen; ticks[last ^ 1]: the tick before it
    long look_driven_until[4];                 // per user: game time until which the camera orientation is look-driven
    long anchor_pass_driven_until[4];          // per user: game time until which the camera anchor is pass-driven
    struct {
        short identifier;                      // the object header's salt when it last moved within a tick
        long until;                            // game time until which the object is pass-driven
    } pass_driven_objects[k_object_capacity];
    real_vector3d camera_offset[4];            // per user, this frame: drawn camera position minus the published one
    long trace;                                // HALO3_INTERPOLATION_TRACE=1: 1
    // Per-tick cache built on the first blended frame of a tick and reused across all subframes of that tick.
    bool object_cache_valid;
    long object_cache_tick;
    long object_cache_prev_tick;
    long object_cache_header_count;
    long object_cache_total_objects;
    long object_cache_moving_objects;
    long cached_object_count;
    long cached_node_count;
    long cached_root_count;
    s_cached_object_blend cached_objects[k_object_capacity];
    s_cached_node_blend cached_nodes[k_cached_node_capacity];
    s_cached_root_check cached_roots[k_object_capacity];
};

s_interpolation_globals g_interpolation; // zero (.bss); render_interpolation_apply starts it

// ---- rotation blending (a matrix's forward, left, up columns as a quaternion) ----

// sqrtf as one sqrtss. The game build's sqrtf is the double sqrt of the C library (math.h: (float)sqrt((double)x)), a
// call per use, several per node of every interpolated object each frame; the correctly rounded float square root is
// the same value (rounding the correctly rounded double root to float never differs: 53 >= 2 * 24 + 2 bits).
inline float square_root(float x) { return __builtin_elementwise_sqrt(x); }

void quaternion_from_axes(const real_vector3d *f, const real_vector3d *l, const real_vector3d *u, s_quaternion *q)
{
    // M = [f l u] (columns): m00 = f.i, m10 = f.j, m20 = f.k, m01 = l.i, ...
    const float m00 = f->i, m10 = f->j, m20 = f->k;
    const float m01 = l->i, m11 = l->j, m21 = l->k;
    const float m02 = u->i, m12 = u->j, m22 = u->k;
    const float trace = m00 + m11 + m22;
    if (trace > 0.0f) {
        const float s = 2.0f * square_root(trace + 1.0f);
        q->w = 0.25f * s;
        q->x = (m21 - m12) / s;
        q->y = (m02 - m20) / s;
        q->z = (m10 - m01) / s;
    } else if (m00 > m11 && m00 > m22) {
        const float s = 2.0f * square_root(1.0f + m00 - m11 - m22);
        q->w = (m21 - m12) / s;
        q->x = 0.25f * s;
        q->y = (m01 + m10) / s;
        q->z = (m02 + m20) / s;
    } else if (m11 > m22) {
        const float s = 2.0f * square_root(1.0f + m11 - m00 - m22);
        q->w = (m02 - m20) / s;
        q->x = (m01 + m10) / s;
        q->y = 0.25f * s;
        q->z = (m12 + m21) / s;
    } else {
        const float s = 2.0f * square_root(1.0f + m22 - m00 - m11);
        q->w = (m10 - m01) / s;
        q->x = (m02 + m20) / s;
        q->y = (m12 + m21) / s;
        q->z = 0.25f * s;
    }
}

void axes_from_quaternion(const s_quaternion *q, real_vector3d *f, real_vector3d *l, real_vector3d *u)
{
    const float x = q->x, y = q->y, z = q->z, w = q->w;
    f->i = 1.0f - 2.0f * (y * y + z * z);
    f->j = 2.0f * (x * y + z * w);
    f->k = 2.0f * (x * z - y * w);
    l->i = 2.0f * (x * y - z * w);
    l->j = 1.0f - 2.0f * (x * x + z * z);
    l->k = 2.0f * (y * z + x * w);
    u->i = 2.0f * (x * z + y * w);
    u->j = 2.0f * (y * z - x * w);
    u->k = 1.0f - 2.0f * (x * x + y * y);
}

// Normalized lerp along the shorter arc; false when the blend is degenerate (the caller keeps the current value).
bool quaternion_blend(const s_quaternion *a, const s_quaternion *b, float t, s_quaternion *out)
{
    const float sign = a->x * b->x + a->y * b->y + a->z * b->z + a->w * b->w < 0.0f ? -1.0f : 1.0f;
    s_quaternion q;
    q.x = a->x + (sign * b->x - a->x) * t;
    q.y = a->y + (sign * b->y - a->y) * t;
    q.z = a->z + (sign * b->z - a->z) * t;
    q.w = a->w + (sign * b->w - a->w) * t;
    const float length_squared = q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
    if (!(length_squared > 1e-12f))
        return false;
    const float inverse = 1.0f / square_root(length_squared);
    out->x = q.x * inverse;
    out->y = q.y * inverse;
    out->z = q.z * inverse;
    out->w = q.w * inverse;
    return true;
}

float lerp(float a, float b, float t) { return a + (b - a) * t; }

void point_lerp(const real_point3d *a, const real_point3d *b, float t, real_point3d *out)
{
    out->x = lerp(a->x, b->x, t);
    out->y = lerp(a->y, b->y, t);
    out->z = lerp(a->z, b->z, t);
}

float distance_squared(const real_point3d *a, const real_point3d *b)
{
    const float x = a->x - b->x, y = a->y - b->y, z = a->z - b->z;
    return x * x + y * y + z * z;
}

bool finite_matrix(const real_matrix4x3 *m)
{
    const float *f = &m->scale;
    for (int i = 0; i < 13; i++)
        if (!(f[i] - f[i] == 0.0f)) // NaN or infinity
            return false;
    return true;
}

// Whether two matrices hold the same bits: memcmp == 0, which this unit's game build flags (-fno-builtin) make a C
// library call, for every node of every interpolated object on every frame.
bool same_bits(const real_matrix4x3 *a, const real_matrix4x3 *b)
{
    static_assert(sizeof(real_matrix4x3) % 4 == 0, "compared as 32-bit words");
    const char *x = (const char *)a, *y = (const char *)b;
    for (unsigned int i = 0; i < sizeof(real_matrix4x3); i += 4) {
        unsigned int u, v;
        __builtin_memcpy(&u, x + i, 4);
        __builtin_memcpy(&v, y + i, 4);
        if (u != v)
            return false;
    }
    return true;
}

// current = blend(previous, current, t), in place; false when kept as it is.
bool matrix_blend(const real_matrix4x3 *previous, real_matrix4x3 *current, float t)
{
    if (same_bits(previous, current))
        return false; // unchanged (most nodes of most objects): kept bit for bit
    if (!finite_matrix(previous) || !finite_matrix(current))
        return false;
    s_quaternion a, b, q;
    quaternion_from_axes(&previous->forward, &previous->left, &previous->up, &a);
    quaternion_from_axes(&current->forward, &current->left, &current->up, &b);
    if (!quaternion_blend(&a, &b, t, &q))
        return false;
    axes_from_quaternion(&q, &current->forward, &current->left, &current->up);
    current->scale = lerp(previous->scale, current->scale, t);
    point_lerp(&previous->position, &current->position, t, &current->position);
    return true;
}

// The vector3d blend of two unit vectors (normalized lerp); false when degenerate.
bool unit_vector_blend(const real_vector3d *a, const real_vector3d *b, float t, real_vector3d *out)
{
    real_vector3d v;
    v.i = lerp(a->i, b->i, t);
    v.j = lerp(a->j, b->j, t);
    v.k = lerp(a->k, b->k, t);
    const float length_squared = v.i * v.i + v.j * v.j + v.k * v.k;
    if (!(length_squared > 1e-12f))
        return false;
    const float inverse = 1.0f / square_root(length_squared);
    out->i = v.i * inverse;
    out->j = v.j * inverse;
    out->k = v.k * inverse;
    return true;
}

// Whether a state set to last until game time `until` (set at most `span` ticks ahead) still holds at `time`. Bounded
// by the span, so a value set before game time went back (a revert, a new map: time starts again at 0) lapses.
bool holds_until(long until, long time, long span)
{
    return time < until && until - time <= span;
}

// ---- placements (a real_matrix4x3 as a similarity transform: uniform scale, rotation columns, translation) ----

real_vector3d rotate_vector(const real_matrix4x3 *m, const real_vector3d *v)
{
    real_vector3d r;
    r.i = m->forward.i * v->i + m->left.i * v->j + m->up.i * v->k;
    r.j = m->forward.j * v->i + m->left.j * v->j + m->up.j * v->k;
    r.k = m->forward.k * v->i + m->left.k * v->j + m->up.k * v->k;
    return r;
}

// out = a * b: b's placement carried by a
void transform_multiply(const real_matrix4x3 *a, const real_matrix4x3 *b, real_matrix4x3 *out)
{
    real_matrix4x3 r;
    r.scale = a->scale * b->scale;
    r.forward = rotate_vector(a, &b->forward);
    r.left = rotate_vector(a, &b->left);
    r.up = rotate_vector(a, &b->up);
    real_vector3d p;
    p.i = b->position.x;
    p.j = b->position.y;
    p.k = b->position.z;
    const real_vector3d moved = rotate_vector(a, &p);
    r.position.x = a->position.x + a->scale * moved.i;
    r.position.y = a->position.y + a->scale * moved.j;
    r.position.z = a->position.z + a->scale * moved.k;
    *out = r;
}

// out = the inverse of a (orthonormal rotation, non-zero scale)
void transform_inverse(const real_matrix4x3 *a, real_matrix4x3 *out)
{
    real_matrix4x3 r;
    r.scale = 1.0f / a->scale;
    r.forward.i = a->forward.i; r.forward.j = a->left.i; r.forward.k = a->up.i;
    r.left.i = a->forward.j;    r.left.j = a->left.j;    r.left.k = a->up.j;
    r.up.i = a->forward.k;      r.up.j = a->left.k;      r.up.k = a->up.k;
    real_vector3d p;
    p.i = -a->position.x;
    p.j = -a->position.y;
    p.k = -a->position.z;
    const real_vector3d moved = rotate_vector(&r, &p);
    r.position.x = r.scale * moved.i;
    r.position.y = r.scale * moved.j;
    r.position.z = r.scale * moved.k;
    *out = r;
}

// The placement of an object's attachment tree: the position, forward, up and scale of its root object (an object
// without a parent, whose are world space). Every node of every object of the tree is placed by it
// (object_compute_node_matrices). False when the tree cannot be followed or the placement is degenerate.
bool object_root_transform(const object_datum *object, real_matrix4x3 *transform)
{
    for (long depth = 0; object->parent_object_index != -1; depth++) {
        if (depth >= 16)
            return false;
        object = (const object_datum *)object_try_and_get_and_verify_type(object->parent_object_index, ~0ul);
        if (object == 0)
            return false;
    }
    if (!(object->scale > 1e-6f))
        return false;
    transform->scale = object->scale;
    transform->forward = object->forward;
    transform->up = object->up;
    transform->left.i = object->up.j * object->forward.k - object->up.k * object->forward.j;
    transform->left.j = object->up.k * object->forward.i - object->up.i * object->forward.k;
    transform->left.k = object->up.i * object->forward.j - object->up.j * object->forward.i;
    transform->position = object->position;
    return finite_matrix(transform);
}

// ---- recording a tick ----

void record_objects(s_tick_record *record)
{
    record->node_count = 0;
    for (long i = 0; i < k_object_capacity; i++)
        record->objects[i].identifier = 0;
    c_data_iterator<s_object_header> iterator;
    iterator.begin(object_header_data);
    while (iterator.next()) {
        const s_object_header *header = iterator.get_datum();
        const object_datum *object = (const object_datum *)header->datum;
        const long absolute_index = iterator.get_absolute_index();
        if (object == 0 || absolute_index < 0 || absolute_index >= k_object_capacity)
            continue;
        const long node_count = object->node_matrices.size / (long)sizeof(real_matrix4x3);
        if (node_count < 0 || record->node_count + node_count > k_node_capacity)
            continue;
        s_object_record *entry = &record->objects[absolute_index];
        entry->identifier = header->identifier;
        entry->node_count = (short)node_count;
        entry->first_node = record->node_count;
        entry->position = object->position;
        entry->bounding_center = object->bounding_center;
        entry->bounding_sphere_center = object->bounding_sphere_center;
        entry->dynamic_light_sphere_center = object->dynamic_light_sphere_center;
        entry->root_valid = object_root_transform(object, &entry->root);
        memcpy(&record->nodes[record->node_count], (const char *)object + object->node_matrices.offset,
               node_count * sizeof(real_matrix4x3));
        record->node_count += node_count;
    }
}

void record_first_person(s_tick_record *record)
{
    const fpw_first_person *first_person = g_first_person_weapon_data;
    for (long user = 0; user < k_fp_user_count; user++) {
        for (long slot = 0; slot < k_fp_slot_count; slot++) {
            s_fp_record *entry = &record->first_person[user][slot];
            entry->node_count = 0;
            if (first_person == 0)
                continue;
            const fpw_weapon *weapon = &first_person[user].weapons[slot];
            const long count = weapon->node_matrix_count;
            if ((weapon->flags & 2) == 0 || count <= 0 || count > k_fp_node_capacity)
                continue;
            entry->unit_index = first_person[user].unit_index;
            entry->objects[0] = weapon->object_indices[2];
            entry->objects[1] = weapon->object_indices[4];
            entry->objects[2] = weapon->object_indices[6];
            entry->node_count = count;
            entry->camera_adjustment = weapon->camera_adjustment;
            memcpy(entry->nodes, weapon->node_matrices, count * sizeof(real_matrix4x3));
        }
    }
}

bool camera_result_valid(const s_player_block *block)
{
    return block->result_valid && block->camera_result.location.cluster_reference.bsp_index != -1;
}

real_point3d observer_world_focus_point(const s_player_block *block)
{
    real_point3d p = block->focus_point;
    if (block->using_matrix) {
        const real_matrix4x3 *m = &block->matrix;
        const float x = p.x * m->scale;
        const float y = p.y * m->scale;
        const float z = p.z * m->scale;
        p.x = m->position.x + x * m->forward.i + y * m->left.i + z * m->up.i;
        p.y = m->position.y + x * m->forward.j + y * m->left.j + z * m->up.j;
        p.z = m->position.z + x * m->forward.k + y * m->left.k + z * m->up.k;
    }
    return p;
}

// ---- blending the mirror ----

// What a frame blended (HALO3_INTERPOLATION_TRACE).
struct s_blend_counts {
    long objects;                              // objects with a previous tick to blend from
    long moving_objects;                       // of those, objects with a node that moved
    long first_person;                         // first-person weapon slots with a node that moved
    long chud;                                 // HUD navpoints and tracked targets that moved
    long pass_driven;                          // objects placed between two ticks, blended in their own frame
};

// Blends every object between the previous tick's record and the mirror (current: this tick's record, taken at its
// first frame; same_tick: this frame is not that first frame).
//
// An object placed between two ticks is pass-driven: the user interface's model widgets (c_gui_model_widget, the
// Spartan of Settings > Appearance and of the lobbies) turn their object every main-loop pass while the user holds the
// stick (assemble_render_data, object_set_position_direct, which computes its node matrices again), and its animation
// still moves only on ticks. Blending its world-space nodes from the previous tick's record, taken at that tick's first
// frame, to the mirror draws the turn a tick late, catching up by the tick's end and dropping back at every tick: a
// 30 Hz sawtooth (5 degrees back at 240 Hz). Its pose is blended in its tree's own frame instead (the placement of the
// tree's root, object_root_transform) and placed as the mirror places it now: the animation at the display time, the
// turn as it is.
inline bool same_point3d(const real_point3d *a, const real_point3d *b)
{
    unsigned int xa[3], xb[3];
    __builtin_memcpy(xa, a, 12);
    __builtin_memcpy(xb, b, 12);
    return ((xa[0] ^ xb[0]) | (xa[1] ^ xb[1]) | (xa[2] ^ xb[2])) == 0u;
}

inline bool same_vector3d(const real_vector3d *a, const real_vector3d *b)
{
    unsigned int xa[3], xb[3];
    __builtin_memcpy(xa, a, 12);
    __builtin_memcpy(xb, b, 12);
    return ((xa[0] ^ xb[0]) | (xa[1] ^ xb[1]) | (xa[2] ^ xb[2])) == 0u;
}

inline bool same_float_bits(float a, float b)
{
    unsigned int xa, xb;
    __builtin_memcpy(&xa, &a, 4);
    __builtin_memcpy(&xb, &b, 4);
    return xa == xb;
}

// Builds the per-tick compact moving-object + precomputed sign-aligned quaternion cache on the first
// blended frame of a new tick. Returns false if any active object is pass-driven or the moving node
// count exceeds k_cached_node_capacity, in which case the caller runs the exact general pass.
bool build_object_blend_cache(const s_tick_record *previous, const s_tick_record *current, long time)
{
    s_interpolation_globals *g = &g_interpolation;
    g->object_cache_valid = false;
    const s_data_array *array = object_header_data;
    if (array == 0 || !array->valid || array->data == 0)
        return false;

    long total_objects = 0;
    long moving_objects = 0;
    long cached_obj_count = 0;
    long cached_node_count = 0;
    long cached_root_count = 0;
    const float max_d2 = k_teleport_distance * k_teleport_distance;

    c_data_iterator<s_object_header> iterator;
    iterator.begin(object_header_data);
    while (iterator.next()) {
        const s_object_header *header = iterator.get_datum();
        const object_datum *object = (const object_datum *)header->datum;
        const long absolute_index = iterator.get_absolute_index();
        if (object == 0 || absolute_index < 0 || absolute_index >= k_object_capacity)
            continue;
        if (g->pass_driven_objects[absolute_index].identifier == header->identifier &&
            holds_until(g->pass_driven_objects[absolute_index].until, time, k_pass_driven_ticks))
            return false; // active pass-driven UI widget: use exact general path
        if (object->parent_object_index == -1 && current->objects[absolute_index].identifier == header->identifier &&
            current->objects[absolute_index].root_valid) {
            s_cached_root_check *rc = &g->cached_roots[cached_root_count++];
            rc->absolute_index = (short)absolute_index;
            rc->identifier = header->identifier;
            rc->scale = object->scale;
            rc->position = object->position;
            rc->forward = object->forward;
            rc->up = object->up;
        }
        const s_object_record *entry = &previous->objects[absolute_index];
        const long node_count = object->node_matrices.size / (long)sizeof(real_matrix4x3);
        if (entry->identifier != header->identifier || entry->node_count != node_count)
            continue;
        if (distance_squared(&entry->position, &object->position) > max_d2)
            continue;
        total_objects++;

        const real_matrix4x3 *nodes = (const real_matrix4x3 *)((const char *)object + object->node_matrices.offset);
        const long first_cached = cached_node_count;
        for (long node = 0; node < node_count; node++) {
            const real_matrix4x3 *prev_m = &previous->nodes[entry->first_node + node];
            const real_matrix4x3 *curr_m = &nodes[node];
            if (same_bits(prev_m, curr_m))
                continue;
            if (!finite_matrix(prev_m) || !finite_matrix(curr_m))
                continue;
            if (cached_node_count >= k_cached_node_capacity)
                return false;
            s_cached_node_blend *cn = &g->cached_nodes[cached_node_count++];
            cn->node_index = (short)node;
            quaternion_from_axes(&prev_m->forward, &prev_m->left, &prev_m->up, &cn->q_prev);
            quaternion_from_axes(&curr_m->forward, &curr_m->left, &curr_m->up, &cn->q_curr);
            const float dot = cn->q_prev.x * cn->q_curr.x + cn->q_prev.y * cn->q_curr.y +
                              cn->q_prev.z * cn->q_curr.z + cn->q_prev.w * cn->q_curr.w;
            if (dot < 0.0f) {
                cn->q_curr.x = -cn->q_curr.x;
                cn->q_curr.y = -cn->q_curr.y;
                cn->q_curr.z = -cn->q_curr.z;
                cn->q_curr.w = -cn->q_curr.w;
            }
            cn->prev_scale = prev_m->scale;
            cn->curr_scale = curr_m->scale;
            cn->prev_pos = prev_m->position;
            cn->curr_pos = curr_m->position;
        }
        const long obj_nodes = cached_node_count - first_cached;
        if (obj_nodes > 0)
            moving_objects++;

        unsigned char pos_mask = 0;
        if (!same_point3d(&entry->position, &object->position))
            pos_mask |= 1u;
        if (!same_point3d(&entry->bounding_center, &object->bounding_center))
            pos_mask |= 2u;
        if (!same_point3d(&entry->bounding_sphere_center, &object->bounding_sphere_center))
            pos_mask |= 4u;
        if (!same_point3d(&entry->dynamic_light_sphere_center, &object->dynamic_light_sphere_center))
            pos_mask |= 8u;

        if (obj_nodes > 0 || pos_mask != 0) {
            s_cached_object_blend *co = &g->cached_objects[cached_obj_count++];
            co->absolute_index = (short)absolute_index;
            co->identifier = header->identifier;
            co->node_count = (short)node_count;
            co->cached_node_count = (short)obj_nodes;
            co->first_cached_node = first_cached;
            co->pos_mask = pos_mask;
        }
    }

    g->object_cache_tick = time;
    g->object_cache_prev_tick = previous->time;
    g->object_cache_header_count = array->actual_count;
    g->object_cache_total_objects = total_objects;
    g->object_cache_moving_objects = moving_objects;
    g->cached_object_count = cached_obj_count;
    g->cached_node_count = cached_node_count;
    g->cached_root_count = cached_root_count;
    g->object_cache_valid = true;
    return true;
}

// Fast mid-tick check: verifies no root object changed placement between frames of the same tick.
bool cached_roots_unchanged()
{
    const s_interpolation_globals *g = &g_interpolation;
    const s_data_array *array = object_header_data;
    if (array == 0 || !array->valid || array->data == 0 || array->actual_count != g->object_cache_header_count)
        return false;
    const char *base = (const char *)array->data;
    const long stride = array->size;
    for (long i = 0; i < g->cached_root_count; i++) {
        const s_cached_root_check *rc = &g->cached_roots[i];
        const s_object_header *header = (const s_object_header *)(base + rc->absolute_index * stride);
        if (header->identifier != rc->identifier)
            return false;
        const object_datum *object = (const object_datum *)header->datum;
        if (object == 0 || object->parent_object_index != -1)
            return false;
        if (!same_float_bits(object->scale, rc->scale) ||
            !same_point3d(&object->position, &rc->position) ||
            !same_vector3d(&object->forward, &rc->forward) ||
            !same_vector3d(&object->up, &rc->up))
            return false;
    }
    return true;
}

void blend_objects_cached(const s_tick_record *previous, float t, s_blend_counts *counts)
{
    const s_interpolation_globals *g = &g_interpolation;
    const s_data_array *array = object_header_data;
    const char *base = (const char *)array->data;
    const long stride = array->size;
    long moved_objs = 0;

    for (long i = 0; i < g->cached_object_count; i++) {
        const s_cached_object_blend *co = &g->cached_objects[i];
        const s_object_header *header = (const s_object_header *)(base + co->absolute_index * stride);
        if (header->identifier != co->identifier)
            continue;
        object_datum *object = (object_datum *)header->datum;
        if (object == 0)
            continue;
        const long node_count = object->node_matrices.size / (long)sizeof(real_matrix4x3);
        if (node_count != co->node_count)
            continue;
        if (co->cached_node_count > 0) {
            real_matrix4x3 *nodes = (real_matrix4x3 *)((char *)object + object->node_matrices.offset);
            bool any_moved = false;
            const s_cached_node_blend *cn = &g->cached_nodes[co->first_cached_node];
            for (long k = 0; k < co->cached_node_count; k++, cn++) {
                s_quaternion q;
                q.x = cn->q_prev.x + (cn->q_curr.x - cn->q_prev.x) * t;
                q.y = cn->q_prev.y + (cn->q_curr.y - cn->q_prev.y) * t;
                q.z = cn->q_prev.z + (cn->q_curr.z - cn->q_prev.z) * t;
                q.w = cn->q_prev.w + (cn->q_curr.w - cn->q_prev.w) * t;
                const float len2 = q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
                if (!(len2 > 1e-12f))
                    continue;
                const float inv = 1.0f / square_root(len2);
                q.x *= inv;
                q.y *= inv;
                q.z *= inv;
                q.w *= inv;
                real_matrix4x3 *dst = &nodes[cn->node_index];
                axes_from_quaternion(&q, &dst->forward, &dst->left, &dst->up);
                dst->scale = lerp(cn->prev_scale, cn->curr_scale, t);
                point_lerp(&cn->prev_pos, &cn->curr_pos, t, &dst->position);
                any_moved = true;
            }
            moved_objs += any_moved;
        }
        if (co->pos_mask != 0) {
            const s_object_record *entry = &previous->objects[co->absolute_index];
            if (co->pos_mask & 1u)
                point_lerp(&entry->position, &object->position, t, &object->position);
            if (co->pos_mask & 2u)
                point_lerp(&entry->bounding_center, &object->bounding_center, t, &object->bounding_center);
            if (co->pos_mask & 4u)
                point_lerp(&entry->bounding_sphere_center, &object->bounding_sphere_center, t, &object->bounding_sphere_center);
            if (co->pos_mask & 8u)
                point_lerp(&entry->dynamic_light_sphere_center, &object->dynamic_light_sphere_center, t,
                           &object->dynamic_light_sphere_center);
        }
    }
    counts->objects = g->object_cache_total_objects;
    counts->moving_objects = moved_objs;
}

void blend_objects(const s_tick_record *previous, const s_tick_record *current, bool same_tick, long time, float t,
                   s_blend_counts *counts)
{
    if (t < 1.0f) {
        if (!same_tick) {
            if (build_object_blend_cache(previous, current, time)) {
                blend_objects_cached(previous, t, counts);
                return;
            }
        } else if (g_interpolation.object_cache_valid && g_interpolation.object_cache_tick == time &&
                   g_interpolation.object_cache_prev_tick == previous->time) {
            if (cached_roots_unchanged()) {
                blend_objects_cached(previous, t, counts);
                return;
            }
            g_interpolation.object_cache_valid = false;
        }
    }

    // Pass 1: detect pass-driven roots and blend node matrices while every object's position is still unblended.
    // object_root_transform walks parent_object_index to the root and reads root->position; mutating parent->position
    // in place during this loop caused any child visited after its moving parent (e.g. a Warthog turret) to see the
    // parent's blended position, trip !same_bits(&now->root, &root), and falsely enter pass-driven blending.
    c_data_iterator<s_object_header> iterator;
    iterator.begin(object_header_data);
    while (iterator.next()) {
        const s_object_header *header = iterator.get_datum();
        object_datum *object = (object_datum *)header->datum;
        const long absolute_index = iterator.get_absolute_index();
        if (object == 0 || absolute_index < 0 || absolute_index >= k_object_capacity)
            continue;
        real_matrix4x3 root;
        const bool root_valid = object_root_transform(object, &root);
        const s_object_record *now = &current->objects[absolute_index];
        if (same_tick && root_valid && now->identifier == header->identifier && now->root_valid &&
            !same_bits(&now->root, &root)) {
            g_interpolation.pass_driven_objects[absolute_index].identifier = header->identifier;
            g_interpolation.pass_driven_objects[absolute_index].until = time + k_pass_driven_ticks;
        }
        if (t >= 1.0f)
            continue;
        const s_object_record *entry = &previous->objects[absolute_index];
        const long node_count = object->node_matrices.size / (long)sizeof(real_matrix4x3);
        if (entry->identifier != header->identifier || entry->node_count != node_count)
            continue; // a new object in this slot, or a new model: shown as it is
        if (distance_squared(&entry->position, &object->position) > k_teleport_distance * k_teleport_distance)
            continue;
        counts->objects++;
        real_matrix4x3 *nodes = (real_matrix4x3 *)((char *)object + object->node_matrices.offset);
        bool moved = false;
        if (g_interpolation.pass_driven_objects[absolute_index].identifier == header->identifier &&
            holds_until(g_interpolation.pass_driven_objects[absolute_index].until, time, k_pass_driven_ticks) &&
            root_valid && entry->root_valid) {
            real_matrix4x3 to_previous_frame, to_current_frame;
            transform_inverse(&entry->root, &to_previous_frame);
            transform_inverse(&root, &to_current_frame);
            for (long node = 0; node < node_count; node++) {
                real_matrix4x3 before, after;
                transform_multiply(&to_previous_frame, &previous->nodes[entry->first_node + node], &before);
                transform_multiply(&to_current_frame, &nodes[node], &after);
                if (matrix_blend(&before, &after, t)) {
                    transform_multiply(&root, &after, &nodes[node]);
                    moved = true;
                }
            }
            counts->moving_objects += moved;
            counts->pass_driven++;
            continue; // its position and bounding spheres: as placed now
        }
        for (long node = 0; node < node_count; node++)
            moved |= matrix_blend(&previous->nodes[entry->first_node + node], &nodes[node], t);
        counts->moving_objects += moved;
    }
    if (t >= 1.0f)
        return;

    // Pass 2: blend positions and bounding/light sphere centres now that all object_root_transform reads are complete.
    iterator.begin(object_header_data);
    while (iterator.next()) {
        const s_object_header *header = iterator.get_datum();
        object_datum *object = (object_datum *)header->datum;
        const long absolute_index = iterator.get_absolute_index();
        if (object == 0 || absolute_index < 0 || absolute_index >= k_object_capacity)
            continue;
        const s_object_record *entry = &previous->objects[absolute_index];
        const long node_count = object->node_matrices.size / (long)sizeof(real_matrix4x3);
        if (entry->identifier != header->identifier || entry->node_count != node_count)
            continue;
        if (distance_squared(&entry->position, &object->position) > k_teleport_distance * k_teleport_distance)
            continue;
        if (g_interpolation.pass_driven_objects[absolute_index].identifier == header->identifier &&
            holds_until(g_interpolation.pass_driven_objects[absolute_index].until, time, k_pass_driven_ticks) &&
            entry->root_valid)
            continue;
        point_lerp(&entry->position, &object->position, t, &object->position);
        point_lerp(&entry->bounding_center, &object->bounding_center, t, &object->bounding_center);
        point_lerp(&entry->bounding_sphere_center, &object->bounding_sphere_center, t, &object->bounding_sphere_center);
        point_lerp(&entry->dynamic_light_sphere_center, &object->dynamic_light_sphere_center, t,
                   &object->dynamic_light_sphere_center);
    }
}

void blend_first_person(const s_tick_record *previous, float t, s_blend_counts *counts)
{
    fpw_first_person *first_person = g_first_person_weapon_data;
    if (first_person == 0)
        return;
    for (long user = 0; user < k_fp_user_count; user++) {
        for (long slot = 0; slot < k_fp_slot_count; slot++) {
            const s_fp_record *entry = &previous->first_person[user][slot];
            fpw_weapon *weapon = &first_person[user].weapons[slot];
            if (entry->node_count == 0 || (weapon->flags & 2) == 0 || weapon->node_matrix_count != entry->node_count ||
                first_person[user].unit_index != entry->unit_index || weapon->object_indices[2] != entry->objects[0] ||
                weapon->object_indices[4] != entry->objects[1] || weapon->object_indices[6] != entry->objects[2])
                continue; // another weapon, model or unit: shown as it is
            bool moved = matrix_blend(&entry->camera_adjustment, &weapon->camera_adjustment, t);
            for (long node = 0; node < entry->node_count; node++)
                moved |= matrix_blend(&entry->nodes[node], &weapon->node_matrices[node], t);
            counts->first_person += moved;
        }
    }
}

// The tick-driven part of a camera's focus (the observer's focus point, in world space): the anchor its position
// follows. The first-person camera puts the focus on the unit's camera position (unit_get_camera_position) and the
// following camera on the unit's or vehicle's (unit_get_camera_info), both from tick poses, so they move only on
// ticks; but the following camera also adds an offset that lags the vehicle's velocity (camera_acceleration,
// following_camera_update_acceleration), which it moves every main-loop pass toward a target that changes on ticks
// (1.5-3 mm a pass in a Banshee or Chopper). That offset is not part of the anchor: it is drawn as it is now, as are
// the look-driven orbit arm and the camera's collision.
real_point3d camera_anchor(long user, const s_player_block *block)
{
    real_point3d anchor = observer_world_focus_point(block);
    real_vector3d offset;
    if (following_camera_get_user_acceleration_offset(user, &offset)) {
        anchor.x -= offset.i;
        anchor.y -= offset.j;
        anchor.z -= offset.k;
    }
    return anchor;
}

// Whether two anchors differ by more than the rounding of taking the offset out again (the focus is the anchor plus
// the offset in floats): a few ulps of the coordinates.
bool anchor_moved(const real_point3d *a, const real_point3d *b)
{
    const float x = fabsf(a->x), y = fabsf(a->y), z = fabsf(a->z);
    const float size = x > y ? (x > z ? x : z) : (y > z ? y : z);
    const float tolerance = 1e-4f + size * 1e-6f;
    return distance_squared(a, b) > tolerance * tolerance;
}

// v turned about the unit axis by the angle whose cosine and sine are given (Rodrigues).
real_vector3d turn_vector(const real_vector3d *v, const real_vector3d *axis, float cosine, float sine)
{
    const float along = (axis->i * v->i + axis->j * v->j + axis->k * v->k) * (1.0f - cosine);
    real_vector3d r;
    r.i = v->i * cosine + (axis->j * v->k - axis->k * v->j) * sine + axis->i * along;
    r.j = v->j * cosine + (axis->k * v->i - axis->i * v->k) * sine + axis->j * along;
    r.k = v->k * cosine + (axis->i * v->j - axis->j * v->i) * sine + axis->k * along;
    return r;
}

// The pitch (elevation from the frame's up) of a unit direction in a frame, and its components there.
float frame_pitch(const real_matrix4x3 *frame, const real_vector3d *v, real_vector3d *local)
{
    local->i = v->i * frame->forward.i + v->j * frame->forward.j + v->k * frame->forward.k;
    local->j = v->i * frame->left.i + v->j * frame->left.j + v->k * frame->left.k;
    local->k = v->i * frame->up.i + v->j * frame->up.j + v->k * frame->up.k;
    return asinf(local->k < -1.0f ? -1.0f : (local->k > 1.0f ? 1.0f : local->k));
}

// A look held at a vehicle's pitch range, drawn against the vehicle as drawn. player_control_modify_desired_angles
// clamps the look of a user in a vehicle seat whose camera has a pitch range to that range in the vehicle's frame
// (player_look_get_user_vehicle_pitch_range), every main-loop pass, with the vehicle's orientation at the current tick.
// While the look is held at the limit and the vehicle tilts, the limit moves once a tick: the look jumps with it on
// the pass after each tick (2-3 degrees of yaw and pitch at once in a Chopper rolling off a bump) and slides along it
// between, a 30 Hz sawtooth against the drawn vehicle, which is a tick behind. Drawn against the vehicle at the
// display time (the blend of its two ticks' placements), as the clamp does it (the heading in the frame kept, the
// pitch set to the limit): a look at the limit of the current tick's frame (the game clamped it there) is put at the
// drawn frame's limit, so it rides the limit as the vehicle is drawn; any other look is clamped to the drawn frame's
// range. Both give the game's look at the end of the tick, where the next tick's frames start from it. The turn from
// the look to the drawn one (axis, cosine, sine); false when there is none.
bool vehicle_pitch_range_turn(long user, const s_tick_record *previous, const s_tick_record *record, float t,
                              const real_vector3d *look, real_vector3d *axis, float *cosine, float *sine)
{
    const float k_at_limit = 1e-3f; // radians: the clamp's own rounding through the look's angles and back
    long vehicle_index;
    real_bounds range;
    if (!player_look_get_user_vehicle_pitch_range(user, &vehicle_index, &range))
        return false;
    const long absolute_index = vehicle_index & 0xffff;
    const short identifier = (short)(vehicle_index >> 16);
    if (absolute_index >= k_object_capacity)
        return false;
    const s_object_record *before = &previous->objects[absolute_index];
    const s_object_record *after = &record->objects[absolute_index];
    if (before->identifier != identifier || after->identifier != identifier || !before->root_valid ||
        !after->root_valid || same_bits(&before->root, &after->root))
        return false;
    real_vector3d local;
    const float pitch_now = frame_pitch(&after->root, look, &local);
    const bool held_lower = fabsf(pitch_now - range.lower) < k_at_limit;
    const bool held_upper = fabsf(pitch_now - range.upper) < k_at_limit;
    real_matrix4x3 frame = after->root;
    matrix_blend(&before->root, &frame, t);
    const float pitch = frame_pitch(&frame, look, &local);
    float clamped;
    if (held_lower || held_upper)
        clamped = held_lower ? range.lower : range.upper; // held at the limit: at the drawn frame's limit
    else if (pitch < range.lower || pitch > range.upper)
        clamped = pitch < range.lower ? range.lower : range.upper;
    else
        return false;
    // as the clamp does: the same heading in the frame, the pitch at the limit
    const float horizontal = square_root(local.i * local.i + local.j * local.j);
    if (!(horizontal > 1e-4f))
        return false;
    const float across = cosf(clamped) / horizontal;
    const float lx = local.i * across, ly = local.j * across, lz = sinf(clamped);
    real_vector3d drawn;
    drawn.i = frame.forward.i * lx + frame.left.i * ly + frame.up.i * lz;
    drawn.j = frame.forward.j * lx + frame.left.j * ly + frame.up.j * lz;
    drawn.k = frame.forward.k * lx + frame.left.k * ly + frame.up.k * lz;
    real_vector3d cross;
    cross.i = look->j * drawn.k - look->k * drawn.j;
    cross.j = look->k * drawn.i - look->i * drawn.k;
    cross.k = look->i * drawn.j - look->j * drawn.i;
    const float length = square_root(cross.i * cross.i + cross.j * cross.j + cross.k * cross.k);
    const float dot = look->i * drawn.i + look->j * drawn.j + look->k * drawn.k;
    if (!(length > 1e-6f) || !(dot >= k_camera_cut_cosine)) // nothing to turn; or more than 30 degrees: no tick's tilt
        return false;
    axis->i = cross.i / length;
    axis->j = cross.j / length;
    axis->k = cross.k / length;
    *sine = length;
    *cosine = dot;
    return true;
}

// Records each camera's tick-driven pose once per tick into record and blends it with previous when t < 1.
// A look-driven camera (first-person, player-attached, or turned between two frames of a tick) keeps its
// per-pass look and effects, and only its anchor (camera_anchor) is blended:
// the camera moves by the anchor's blend, so the vehicle or unit it follows is where it is drawn and the per-pass
// parts (the following camera's acceleration offset, the orbit arm -forward * focus_distance, collision) stay as they
// are. A tick-driven camera (scripted, cinematic) is blended whole: position, orientation and field of view.
// A camera whose anchor moved between two frames of a tick (a free camera, an observer transition, the main menu's
// eased camera_set pans) is pass-driven: drawn as it is.
void record_and_blend_cameras(const s_tick_record *previous, s_tick_record *record, bool same_tick, long time, float t)
{
    s_player_globals *observer = g_observer_globals;
    for (long user = 0; user < 4; user++) {
        s_camera_record *seen = &record->cameras[user];
        if (observer == 0 || !camera_result_valid(&observer->blocks[user])) {
            seen->valid = false;
            continue;
        }
        s_player_block *block = &observer->blocks[user];
        s_observer_result *result = &block->camera_result;
        const real_point3d anchor = camera_anchor(user, block);
        if (same_tick && seen->valid &&
            (seen->last_forward.i != result->forward.i || seen->last_forward.j != result->forward.j ||
             seen->last_forward.k != result->forward.k || seen->last_up.i != result->up.i ||
             seen->last_up.j != result->up.j || seen->last_up.k != result->up.k ||
             seen->last_fov != result->horizontal_field_of_view))
            g_interpolation.look_driven_until[user] = time + k_look_driven_ticks;
        if (same_tick && seen->valid && anchor_moved(&seen->anchor, &anchor))
            g_interpolation.anchor_pass_driven_until[user] = time + k_pass_driven_ticks;
        if (!same_tick || !seen->valid) {
            seen->valid = true;
            seen->anchor = anchor;
            seen->position = result->position;
            seen->forward = result->forward;
            seen->up = result->up;
            seen->horizontal_field_of_view = result->horizontal_field_of_view;
            seen->vertical_field_of_view = result->vertical_field_of_view;
        }
        seen->last_forward = result->forward;
        seen->last_up = result->up;
        seen->last_fov = result->horizontal_field_of_view;
        const s_camera_record *entry = &previous->cameras[user];
        if (t >= 1.0f || !entry->valid)
            continue;
        if (holds_until(g_interpolation.anchor_pass_driven_until[user], time, k_pass_driven_ticks))
            continue; // pass-driven: drawn as it is
        const bool player_attached = block->parent_unit_index != -1 || block->parent_object_index != -1;
        const bool first_person =
            g_director_globals && g_director_globals->slots[user].camera_type == _camera_mode_first_person;
        const bool look_driven = first_person || player_attached ||
            holds_until(g_interpolation.look_driven_until[user], time, k_look_driven_ticks);
        if (look_driven) {
            // the observer's own transition (parameter timers) moves the focus every pass: drawn as it is
            real_vector3d shift;
            shift.i = shift.j = shift.k = 0.0f;
            if (block->previous.parameter_timers[0] <= 0.0f &&
                distance_squared(&entry->anchor, &seen->anchor) <= k_camera_cut_distance * k_camera_cut_distance &&
                memcmp(&entry->anchor, &seen->anchor, sizeof(seen->anchor)) != 0) {
                const float inv_t = 1.0f - t;
                shift.i = inv_t * (entry->anchor.x - seen->anchor.x);
                shift.j = inv_t * (entry->anchor.y - seen->anchor.y);
                shift.k = inv_t * (entry->anchor.z - seen->anchor.z);
                result->position.x += shift.i;
                result->position.y += shift.j;
                result->position.z += shift.k;
            }
            // a look held at a vehicle's pitch range: the camera turns about its drawn focus to the range of the
            // vehicle as drawn (forward, up, and the arm from the focus: the following camera's orbit)
            real_vector3d axis;
            float cosine, sine;
            const bool turned =
                vehicle_pitch_range_turn(user, previous, record, t, &result->forward, &axis, &cosine, &sine);
            if (turned) {
                real_point3d focus = observer_world_focus_point(block);
                focus.x += shift.i;
                focus.y += shift.j;
                focus.z += shift.k;
                real_vector3d arm;
                arm.i = result->position.x - focus.x;
                arm.j = result->position.y - focus.y;
                arm.k = result->position.z - focus.z;
                arm = turn_vector(&arm, &axis, cosine, sine);
                result->position.x = focus.x + arm.i;
                result->position.y = focus.y + arm.j;
                result->position.z = focus.z + arm.k;
                result->forward = turn_vector(&result->forward, &axis, cosine, sine);
                result->up = turn_vector(&result->up, &axis, cosine, sine);
            }
            if (turned || shift.i != 0.0f || shift.j != 0.0f || shift.k != 0.0f) {
                s_location location;
                scenario_location_from_point(&location, &result->position);
                if (location.cluster_reference.bsp_index != -1)
                    result->location = location;
            }
            // a following camera whose up is its unit's (mover_has_variable_camera_up, command flag 0x200) takes it
            // from the tick pose: blend the up of the two ticks and keep it square to the forward the player turned to
            if ((block->previous.flags & 0x200u) != 0 &&
                memcmp(&entry->up, &seen->up, sizeof(seen->up)) != 0) {
                real_vector3d up;
                if (unit_vector_blend(&entry->up, &seen->up, t, &up)) {
                    const float along = up.i * result->forward.i + up.j * result->forward.j + up.k * result->forward.k;
                    up.i -= along * result->forward.i;
                    up.j -= along * result->forward.j;
                    up.k -= along * result->forward.k;
                    if (unit_vector_blend(&up, &up, 0.0f, &up))
                        result->up = up;
                }
            }
            continue;
        }
        // Tick-driven (scripted / cinematic) camera: blend position, orientation, and FOV.
        if (distance_squared(&entry->position, &seen->position) > k_camera_cut_distance * k_camera_cut_distance)
            continue;
        const bool turned = memcmp(&entry->forward, &seen->forward, sizeof(seen->forward)) != 0 ||
                            memcmp(&entry->up, &seen->up, sizeof(seen->up)) != 0 ||
                            entry->horizontal_field_of_view != seen->horizontal_field_of_view;
        const float cosine = entry->forward.i * seen->forward.i + entry->forward.j * seen->forward.j +
                             entry->forward.k * seen->forward.k;
        if (turned && !(cosine >= k_camera_cut_cosine))
            continue; // a tick-driven camera turned more than 30 degrees in one tick: a cut
        if (turned) {
            real_vector3d forward, up;
            if (unit_vector_blend(&entry->forward, &seen->forward, t, &forward) &&
                unit_vector_blend(&entry->up, &seen->up, t, &up)) {
                const float along = up.i * forward.i + up.j * forward.j + up.k * forward.k;
                up.i -= along * forward.i;
                up.j -= along * forward.j;
                up.k -= along * forward.k;
                if (unit_vector_blend(&up, &up, 0.0f, &up)) {
                    result->forward = forward;
                    result->up = up;
                    result->horizontal_field_of_view =
                        lerp(entry->horizontal_field_of_view, seen->horizontal_field_of_view, t);
                    result->vertical_field_of_view =
                        lerp(entry->vertical_field_of_view, seen->vertical_field_of_view, t);
                }
            }
        }
        if (memcmp(&entry->position, &seen->position, sizeof(seen->position)) == 0)
            continue;
        real_point3d position;
        point_lerp(&entry->position, &seen->position, t, &position);
        result->position = position;
        s_location location;
        scenario_location_from_point(&location, &position);
        if (location.cluster_reference.bsp_index != -1)
            result->location = location;
    }
}

// Each user's camera position as published, before record_and_blend_cameras moves it.
void published_camera_positions(real_point3d positions[4])
{
    const s_player_globals *observer = g_observer_globals;
    for (long user = 0; user < 4; user++) {
        if (observer != 0)
            positions[user] = observer->blocks[user].camera_result.position;
        else
            positions[user].x = positions[user].y = positions[user].z = 0.0f;
    }
}

// How far record_and_blend_cameras moved each user's camera this frame (zero: drawn as published).
void record_camera_offsets(const s_tick_record *record, const real_point3d positions[4])
{
    const s_player_globals *observer = g_observer_globals;
    for (long user = 0; user < 4; user++) {
        real_vector3d *offset = &g_interpolation.camera_offset[user];
        offset->i = offset->j = offset->k = 0.0f;
        if (observer == 0 || !record->cameras[user].valid)
            continue;
        const real_point3d *drawn = &observer->blocks[user].camera_result.position;
        offset->i = drawn->x - positions[user].x;
        offset->j = drawn->y - positions[user].y;
        offset->k = drawn->z - positions[user].z;
    }
}

void record_lights(s_tick_record *record)
{
    memset(record->lights, 0, sizeof(record->lights));
    const s_data_array *array = light_data;
    if (array == 0 || !array->valid || array->data == 0)
        return;
    const long limit = array->first_unallocated < k_light_capacity ? array->first_unallocated : k_light_capacity;
    const light_datum *lights = (const light_datum *)array->data;
    for (long i = 0; i < limit; i++) {
        const light_datum *light = &lights[i];
        if (light->identifier == 0)
            continue;
        s_light_record *entry = &record->lights[i];
        entry->identifier = light->identifier;
        entry->position = light->position;
        entry->endpoint = light->endpoint;
        entry->valid_point = light->valid_point;
        entry->bounding_sphere_center = light->bounding_sphere_center;
        entry->lod_bounding_sphere_center = light->lod_bounding_sphere_center;
        entry->collision_sphere_center = light->collision_sphere_center;
        entry->forward = light->forward;
        entry->up = light->up;
    }
}

void blend_lights(const s_tick_record *previous, float t)
{
    const s_data_array *array = light_data;
    if (array == 0 || !array->valid || array->data == 0)
        return;
    const long limit = array->first_unallocated < k_light_capacity ? array->first_unallocated : k_light_capacity;
    light_datum *lights = (light_datum *)array->data;
    const float max_d2 = k_teleport_distance * k_teleport_distance;
    for (long i = 0; i < limit; i++) {
        light_datum *light = &lights[i];
        const s_light_record *entry = &previous->lights[i];
        if (light->identifier == 0 || entry->identifier != light->identifier)
            continue;
        if (distance_squared(&entry->position, &light->position) > max_d2)
            continue;
        if (!same_point3d(&entry->position, &light->position))
            point_lerp(&entry->position, &light->position, t, &light->position);
        if (!same_point3d(&entry->endpoint, &light->endpoint))
            point_lerp(&entry->endpoint, &light->endpoint, t, &light->endpoint);
        if (!same_point3d(&entry->valid_point, &light->valid_point))
            point_lerp(&entry->valid_point, &light->valid_point, t, &light->valid_point);
        if (!same_point3d(&entry->bounding_sphere_center, &light->bounding_sphere_center))
            point_lerp(&entry->bounding_sphere_center, &light->bounding_sphere_center, t, &light->bounding_sphere_center);
        if (!same_point3d(&entry->lod_bounding_sphere_center, &light->lod_bounding_sphere_center))
            point_lerp(&entry->lod_bounding_sphere_center, &light->lod_bounding_sphere_center, t, &light->lod_bounding_sphere_center);
        if (!same_point3d(&entry->collision_sphere_center, &light->collision_sphere_center))
            point_lerp(&entry->collision_sphere_center, &light->collision_sphere_center, t, &light->collision_sphere_center);
        if (!same_vector3d(&entry->forward, &light->forward) || !same_vector3d(&entry->up, &light->up)) {
            real_vector3d forward, up;
            if (unit_vector_blend(&entry->forward, &light->forward, t, &forward) &&
                unit_vector_blend(&entry->up, &light->up, t, &up)) {
                const float along = up.i * forward.i + up.j * forward.j + up.k * forward.k;
                up.i -= along * forward.i;
                up.j -= along * forward.j;
                up.k -= along * forward.k;
                if (unit_vector_blend(&up, &up, 0.0f, &up)) {
                    light->forward = forward;
                    light->up = up;
                }
            }
        }
    }
}

// ---- antennas (object widgets: a two-vertex spring on an object's marker, in world space) ----

// antennas_update runs every frame (main_update_effects_and_sound) and moves the vertices once a tick
// (antenna_update): the base onto the marker of the tick's pose, the tip by its spring. Drawn as they are, an antenna
// leads its blended object by up to a tick.
void record_antennas(s_tick_record *record)
{
    for (long i = 0; i < k_antenna_capacity; i++)
        record->antennas[i].index = -1;
    if (g_antenna_data == 0 || !g_antenna_data->valid)
        return;
    c_data_iterator<antenna_datum> iterator;
    iterator.begin(g_antenna_data);
    while (iterator.next()) {
        const antenna_datum *antenna = iterator.get_datum();
        const long absolute_index = iterator.get_absolute_index();
        if (absolute_index < 0 || absolute_index >= k_antenna_capacity)
            continue;
        s_antenna_record *entry = &record->antennas[absolute_index];
        entry->index = iterator.get_index();
        entry->object_index = antenna->object_index;
        entry->position = antenna->position;
        entry->vertices[0] = antenna->vertices[0].position;
        entry->vertices[1] = antenna->vertices[1].position;
        entry->acceleration = antenna->acceleration;
    }
}

void blend_antennas(const s_tick_record *previous, float t)
{
    if (g_antenna_data == 0 || !g_antenna_data->valid)
        return;
    const float max_d2 = k_teleport_distance * k_teleport_distance;
    c_data_iterator<antenna_datum> iterator;
    iterator.begin(g_antenna_data);
    while (iterator.next()) {
        antenna_datum *antenna = iterator.get_datum();
        const long absolute_index = iterator.get_absolute_index();
        if (absolute_index < 0 || absolute_index >= k_antenna_capacity)
            continue;
        const s_antenna_record *entry = &previous->antennas[absolute_index];
        if (entry->index != iterator.get_index() || entry->object_index != antenna->object_index ||
            distance_squared(&entry->vertices[0], &antenna->vertices[0].position) > max_d2)
            continue; // a new antenna in this slot, or its object jumped: shown as it is
        point_lerp(&entry->position, &antenna->position, t, &antenna->position);
        point_lerp(&entry->vertices[0], &antenna->vertices[0].position, t, &antenna->vertices[0].position);
        point_lerp(&entry->vertices[1], &antenna->vertices[1].position, t, &antenna->vertices[1].position);
        real_vector3d acceleration;
        if (unit_vector_blend(&entry->acceleration, &antenna->acceleration, t, &acceleration))
            antenna->acceleration = acceleration;
    }
}

// ---- first-person effects ----

// Effects attached to a first-person weapon resolve its nodes through fpw_first_person::camera_matrix
// (effect_get_node_matrix), which the main thread estimates from the published camera every frame
// (first_person_weapons_update_camera_estimate). The render thread advances effects (effects_frame_advance) before
// a view rebuilds it (render_camera_build_first_person_models): estimated again from the drawn camera and the
// blended camera adjustment, so first-person effects stay on the drawn weapon.
void estimate_first_person_cameras()
{
    if (g_first_person_weapon_data == 0 || g_observer_globals == 0)
        return;
    for (long user = 0; user < k_fp_user_count; user++)
        first_person_weapons_update_camera_estimate(user);
}

// ---- the HUD's world points (chud persistent user data) ----

void record_chud(s_tick_record *record)
{
    memset(record->chud, 0, sizeof(record->chud));
    const c_chud_persistent_user_data *users = c_chud_manager::x_persistent_user_data;
    if (users == 0)
        return;
    for (long user = 0; user < k_chud_user_count; user++) {
        const c_chud_persistent_user_data *data = &users[user];
        s_chud_record *entry = &record->chud[user];
        for (long index = 0; index < k_navpoint_count; index++) {
            const c_chud_navpoint_manager::s_managed_navpoint *navpoint = &data->m_navpoint_manager.navpoints[index];
            s_navpoint_record *seen = &entry->navpoints[index];
            seen->valid = navpoint->valid;
            if (!seen->valid)
                continue;
            seen->id = navpoint->id;
            seen->type = navpoint->arrow_index;
            seen->position = navpoint->position;
            seen->scale = navpoint->scale;
        }
        entry->tracking = data->m_display_state.point_tracker_enabled;
        entry->tracked_position = data->m_display_state.point_tracker_position;
    }
}

// Blends the navpoints and tracked targets with the previous tick's; returns how many moved.
long blend_chud(const s_tick_record *previous, float t)
{
    c_chud_persistent_user_data *users = c_chud_manager::x_persistent_user_data;
    if (users == 0)
        return 0;
    const float max_d2 = k_teleport_distance * k_teleport_distance;
    long moved = 0;
    for (long user = 0; user < k_chud_user_count; user++) {
        c_chud_persistent_user_data *data = &users[user];
        const s_chud_record *entry = &previous->chud[user];
        for (long index = 0; index < k_navpoint_count; index++) {
            c_chud_navpoint_manager::s_managed_navpoint *navpoint = &data->m_navpoint_manager.navpoints[index];
            const s_navpoint_record *seen = &entry->navpoints[index];
            // the slot held another navpoint at the previous tick (submit_navpoint reuses a slot by id), or the
            // navpoint jumped (a respawn waypoint, a teleport): shown as it is
            if (!navpoint->valid || !seen->valid || seen->id != navpoint->id || seen->type != navpoint->arrow_index ||
                distance_squared(&seen->position, &navpoint->position) > max_d2)
                continue;
            if (memcmp(&seen->position, &navpoint->position, sizeof(navpoint->position)) != 0) {
                point_lerp(&seen->position, &navpoint->position, t, &navpoint->position);
                moved++;
            }
            navpoint->scale = lerp(seen->scale, navpoint->scale, t);
        }
        s_chud_persistent_display_state *display = &data->m_display_state;
        if (display->point_tracker_enabled && entry->tracking &&
            distance_squared(&entry->tracked_position, &display->point_tracker_position) <= max_d2 &&
            memcmp(&entry->tracked_position, &display->point_tracker_position, sizeof(real_point3d)) != 0) {
            real_point3d *tracked = &display->point_tracker_position;
            point_lerp(&entry->tracked_position, tracked, t, tracked);
            moved++;
        }
    }
    return moved;
}

// The directional damage indicators' points are one unit from the camera that published them, against the damage's
// direction (c_chud_directional_damage::update): moved with the drawn camera, every frame the camera was moved.
void offset_camera_relative_chud(long time)
{
    c_chud_persistent_user_data *users = c_chud_manager::x_persistent_user_data;
    if (users == 0)
        return;
    for (long user = 0; user < k_chud_user_count; user++) {
        const real_vector3d *offset = &g_interpolation.camera_offset[user];
        if (offset->i == 0.0f && offset->j == 0.0f && offset->k == 0.0f)
            continue;
        s_directional_damage *entries = users[user].m_directional_damage.entries;
        for (long index = 0; index < k_directional_damage_count; index++) {
            if (entries[index].end_time <= time)
                continue; // not drawn (chud_draw_directional_damage_overlay)
            real_point3d *point = &entries[index].tracker.m_world_position;
            point->x += offset->i;
            point->y += offset->j;
            point->z += offset->k;
        }
    }
}

} // namespace

void s_camera_effect_interpolation::sample(long time, float leftover, real_matrix4x3 *effect)
{
    if (!valid || time < tick) {
        valid = true;
        tick = previous_tick = time;
    } else if (time != tick) {
        previous_tick = tick;
        previous = current;
        tick = time;
    }
    current = *effect;
    if (previous_tick < tick) {
        const float t = tick_blend_fraction(previous_tick, tick, leftover);
        if (t < 1.0f)
            matrix_blend(&previous, effect, t);
    }
}

// The render thread, on the published mirror it is about to draw (process_published_game_state, main.cpp).
void render_interpolation_apply()
{
    s_interpolation_globals *globals = &g_interpolation;
    if (!globals->started) {
        globals->ticks[0].time = globals->ticks[1].time = -1;
        globals->started = true;
        const char *trace = getenv("HALO3_INTERPOLATION_TRACE");
        globals->trace = trace != 0 && trace[0] == '1' ? 1 : 0;
    }
    // off (HALO3_FPS=30, HALO3_INTERPOLATE=0): the frame is drawn as published; the trace still records it
    const bool enabled = port_d3d_frame_interpolation() != 0;
    if (!enabled && !globals->trace)
        return;
    const game_time_globals_definition *time_globals = game_time_globals;
    if (time_globals == 0 || !time_globals->initialized || object_header_data == 0) {
        globals->ticks[0].time = globals->ticks[1].time = -1;
        globals->object_cache_valid = false;
        return;
    }
    const long time = time_globals->time;
    s_tick_record *last = &globals->ticks[globals->last];
    const bool same_tick = last->time == time;
    if (!same_tick) {
        // a new tick: the latest record becomes the previous tick; a step back or a jump (a revert, a load, a long
        // stall) leaves nothing to blend from
        globals->object_cache_valid = false;
        globals->last ^= 1;
        if (last->time < 0 || time < last->time || time - last->time > 4)
            last->time = -1;
        last = &globals->ticks[globals->last];
    }
    const s_tick_record *previous = &globals->ticks[globals->last ^ 1];

    // The frames published since the last one: game time stood still in one of them (shell paused, game time not
    // advanced or paused, no simulation; main_loop_body's timing flags 1, 2, 3, 5): show the current tick as it is.
    const unsigned long flags = g_main_render_timing_data != 0 ? (unsigned long)g_main_render_timing_data->count : 0;
    const bool advancing = (flags & ((1u << 1) | (1u << 2) | (1u << 3) | (1u << 5))) == 0;
    float t = 1.0f;
    if (enabled && advancing && previous->time >= 0 && previous->time < time)
        t = tick_blend_fraction(previous->time, time, time_globals->leftover_ticks);

    // record this tick's poses once per tick (before blending them; the cameras read the objects' records), then
    // blend the mirror
    s_tick_record *record = last;
    record->time = time;
    if (!same_tick) {
        record_objects(record);
        record_lights(record);
        record_first_person(record);
        record_antennas(record);
        record_chud(record);
    }
    real_point3d published_cameras[4];
    published_camera_positions(published_cameras);
    record_and_blend_cameras(previous, record, same_tick, time, t);
    record_camera_offsets(record, published_cameras);
    s_blend_counts counts = { 0, 0, 0, 0, 0 };
    if (t < 1.0f) {
        blend_objects(previous, record, same_tick, time, t, &counts);
        blend_lights(previous, t);
        blend_first_person(previous, t, &counts);
        blend_antennas(previous, t);
        counts.chud = blend_chud(previous, t);
        estimate_first_person_cameras();
        offset_camera_relative_chud(time);
    }
    if (globals->trace) {
        // wall ms, game time, leftover, blend factor, user 0's camera as drawn and as published, objects blended (of
        // them moving), first-person weapon slots blended, HUD navpoints and tracked targets blended
        const s_player_globals *observer = g_observer_globals;
        const s_camera_record *published = &record->cameras[0];
        if (observer != 0 && published->valid) {
            const s_observer_result *drawn = &observer->blocks[0].camera_result;
            char line[320];
            csnzprintf(line, sizeof(line),
                       "interp: ms %lu time %ld leftover %.4f t %.4f cam %.4f %.4f %.4f fwd %.5f %.5f %.5f "
                       "published %.4f %.4f %.4f fwd %.5f %.5f %.5f objects %ld moving %ld first-person %ld chud %ld "
                       "pass-driven %ld\n",
                       GetTickCount(), time, time_globals->leftover_ticks, t, drawn->position.x, drawn->position.y,
                       drawn->position.z, drawn->forward.i, drawn->forward.j, drawn->forward.k, published->position.x,
                       published->position.y, published->position.z, published->forward.i, published->forward.j,
                       published->forward.k, counts.objects, counts.moving_objects, counts.first_person, counts.chud,
                       counts.pass_driven);
            port_write_error(line);
        }
    }
}
