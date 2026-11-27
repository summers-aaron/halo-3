// ailibxbox/path_smoothing.obj: from the records 0x11efd8 0x11f8d0 0x575930 0x575ab0 0xce208
// @flags /TP /O2 /Oc
#include "ai/pathfinding_collision.h"
#include "ai/path_state_types.h"
#include "ai/sector_ref.h"
#include <math.h>
#include "tag_files/tag_types.h"
#include "ai/ai_hint_definitions.h"
#include "ai/path_smoothing.h"
#include "ai/sector.h"
#include "math/matrix_math_types.h"
#include "math/real_math_globals.h"
#include "ai/pathfinding_geometry.h"
#include "ai/path_obstacle_types.h"
#include "math/real_math_types.h"
#include "math/real_math.h"
#include "math/real_math_normalize_inline.h"
#include "math/projection_math.h"
#include "ai/pathfinding_sector_types.h"
#include "ai/pathfinding_data.h"
#include "math/real_math_functions.h"
#include "ai/hint_ref.h"
#include "ai/ai_point3d.h"
#include "ai/path_types.h"
#include "ai/pathfinding_pill_result.h"
#include "ai/ai_reference_frame.h"

static __forceinline float path_smoothing_normalize2d(real_vector2d *v)
{
    float length = magnitude2d(v);
    if (fabsf(length - 0.0f) < 0.0001f)
        length = 0.0f;
    else
        scale_vector2d(v, 1.0f / length, v);
    return length;
}

// @address 0x11efd8 ?find_turning_point@@YA_NFPBTreal_point3d@@MJ_NF11PAT1@@Z
bool find_turning_point(short structure_bsp_index, const real_point3d *point, float radius, long link_index, bool clockwise,
    short projection, bool sign, bool unused, real_point3d *turning_point)
{
    pathfinding_data *data = pathfinding_data_get(structure_bsp_index);
    long blocked_sector = NONE;
    long first_vertex = NONE;
    long target_vertex = NONE;
    short iterations = 0;
    long last_vertex = NONE;
    long previous_vertex = NONE;
    bool allow_flagged = !(projection == 2 && sign);
    bool valid = true;
    long current_link_index = link_index;
    while (valid)
    {
        sector_link *link = TAG_BLOCK_GET_ELEMENT(&data->links, current_link_index, sector_link);
        bool forward = !(link->sectors[0] == NONE || link->sectors[0] == 0xffff) &&
                       ((pathfinding_get_sector(data, link->sectors[0])->flags & 1) ||
                           (allow_flagged && (pathfinding_get_sector(data, link->sectors[0])->flags & 0x8000))) &&
                       (!(pathfinding_get_sector(data, link->sectors[0])->flags & 2) ||
                           !sector_broken(structure_bsp_index, data,
                               pathfinding_get_sector(data, link->sectors[0])));
        long sector_index = link->sectors[!forward];
        if ((sector_index == NONE || sector_index == 0xffff))
            return false;
        pathfinding_sector *sector = pathfinding_get_sector(data, (short)sector_index);
        if (!(sector->flags & 1) && (!allow_flagged || !(sector->flags & 0x8000)))
            return false;
        if ((sector->flags & 2) && sector_broken(structure_bsp_index, data, sector))
            return false;
        long vertex0, vertex1;
        bool blocked = false;
        if (forward)
        {
            vertex0 = link->vertices[1];
            vertex1 = link->vertices[0];
        }
        else
        {
            vertex0 = link->vertices[0];
            vertex1 = link->vertices[1];
        }
        if (first_vertex == NONE)
        {
            blocked = true;
        }
        else
        {
            real_point2d p0, p1;
            project_point3d(TAG_BLOCK_GET_ELEMENT(&data->vertices, vertex0, real_point3d), projection, sign, &p0);
            project_point3d(TAG_BLOCK_GET_ELEMENT(&data->vertices, vertex1, real_point3d), projection, sign, &p1);
            real_vector2d edge;
            vector_from_points2d(&p0, &p1, &edge);
            real_vector2d normal;
            set_real_vector2d(&normal, edge.j, -edge.i);
            float length = magnitude2d(&normal);
            if (!(fabsf(length) < 0.0001f))
                scale_vector2d(&normal, 1.0f / length, &normal);
            else
                length = 0.0f;
            real_point2d left_point, right_point;
            point_from_line2d(&point->xy, &normal, radius, &left_point);
            point_from_line2d(&point->xy, &normal, -radius, &right_point);
            real_vector2d to_left;
            if (clockwise)
                vector_from_points2d(&left_point, &p0, &to_left);
            else
                vector_from_points2d(&left_point, &p1, &to_left);
            real_vector2d to_right;
            vector_from_points2d(&right_point, &p0, &to_right);
            if (length < 0.0001f)
                blocked = true;
            if ((dot_product2d(&to_left, &edge) < 0.0f) == clockwise && cross_product2d(&to_left, &edge) < 0.0f)
                blocked = true;
            if (cross_product2d(&to_right, &edge) < 0.0f)
                blocked = true;
        }
        long vertex_index;
        if ((blocked != forward) == clockwise)
            vertex_index = link->vertices[1];
        else
            vertex_index = link->vertices[0];
        if (vertex_index == target_vertex)
        {
            const real_point3d *vertex = TAG_BLOCK_GET_ELEMENT(&data->vertices, vertex_index, real_point3d);
            set_real_point3d(turning_point, vertex->n[global_projection3d_mappings[projection][sign][0]],
                vertex->n[global_projection3d_mappings[projection][sign][1]], vertex->n[global_projection3d_mappings[projection][sign][2]]);
            return true;
        }
        if (vertex_index == previous_vertex)
            return false;
        previous_vertex = last_vertex;
        last_vertex = vertex_index;
        if (vertex_index == first_vertex)
            return false;
        if (first_vertex == NONE)
            first_vertex = vertex_index;
        if (++iterations > 100)
            return false;

        short link_count = 0;
        long first_link_index = current_link_index;
        long current_sector = sector_index;
        long current_vertex = vertex_index;
        long next_vertex;
        while (true)
        {
            bool crossed = false;
            if (current_link_index != first_link_index && link->hint_index != -1)
            {
                pathfinding_hint *hint = &((pathfinding_hint *)data->hints.address)[link->hint_index];
                if (hint->type == 0)
                {
                    sector_link *other = TAG_BLOCK_GET_ELEMENT(&data->links, hint->link_index, sector_link);
                    if (other->sectors[0] == current_sector || other->sectors[1] == current_sector)
                    {
                        bool side = other->sectors[0] == current_sector;
                        current_sector = other->sectors[side];
                        current_link_index = runtime_cw_link(data, other->links[side], current_sector);
                        link = TAG_BLOCK_GET_ELEMENT(&data->links, current_link_index, sector_link);
                        long other_sector = link->sectors[0] == current_sector ? link->sectors[1] : link->sectors[0];
                        if (!(other_sector == NONE || other_sector == 0xffff))
                        {
                            pathfinding_sector *sector = pathfinding_get_sector(data, (short)other_sector);
                            if (((sector->flags & 1) || (allow_flagged && (sector->flags & 0x8000))) &&
                                (!(sector->flags & 2) || !sector_broken(structure_bsp_index, data, sector)))
                                blocked_sector = other_sector;
                        }
                        crossed = true;
                    }
                }
                else if (hint->type == 9)
                {
                    short seam_structure_bsp_index = NONE;
                    long seam_link_index = NONE;
                    if (::pathfinding_find_seam_link(structure_bsp_index, link->hint_index, (short *)(&seam_structure_bsp_index),
                            (long *)(&seam_link_index), (long *)(&current_sector)))
                    {
                        structure_bsp_index = seam_structure_bsp_index;
                        data = pathfinding_data_get(seam_structure_bsp_index);
                        current_link_index = seam_link_index;
                        link = TAG_BLOCK_GET_ELEMENT(&data->links, current_link_index, sector_link);
                        crossed = true;
                    }
                }
                if (crossed)
                {
                    if (clockwise)
                        next_vertex = link->sectors[0] == current_sector ? link->vertices[0] : link->vertices[1];
                    else
                        next_vertex = link->sectors[0] == current_sector ? link->vertices[1] : link->vertices[0];
                }
            }
            if (!crossed)
            {
                bool on_end = current_vertex == link->vertices[1];
                current_sector = link->sectors[(on_end ^ clockwise) == 0];
                next_vertex = current_vertex;
            }
            bool can_continue;
            if ((current_sector == NONE || current_sector == 0xffff))
            {
                can_continue = false;
            }
            else
            {
                pathfinding_sector *sector = pathfinding_get_sector(data, (short)current_sector);
                can_continue = ((sector->flags & 1) || (allow_flagged && (sector->flags & 0x8000))) &&
                               (!(sector->flags & 2) || !sector_broken(structure_bsp_index, data, sector)) &&
                               current_sector != blocked_sector;
            }
            if (!can_continue)
                break;
            current_vertex = next_vertex;
            unsigned short next_link_index;
            if (clockwise)
                next_link_index = runtime_cw_link(data, current_link_index, current_sector);
            else
                next_link_index = runtime_ccw_link(data, current_link_index, current_sector);
            current_link_index = next_link_index;
            if ((current_link_index == NONE || current_link_index == 0xffff))
            {
                valid = false;
                break;
            }
            link = TAG_BLOCK_GET_ELEMENT(&data->links, current_link_index, sector_link);
            if (current_link_index == first_link_index)
            {
                valid = false;
                break;
            }
            if (link->vertices[0] != current_vertex && link->vertices[1] != current_vertex)
            {
                valid = false;
                break;
            }
            if (++link_count > 50)
            {
                valid = false;
                break;
            }
        }
        target_vertex = current_vertex;
    }
    return valid;
}

// @address 0x11f8d0 ?find_avoidance_point@@YAXPBTreal_point3d@@00M_NF1PAT1@@Z
void find_avoidance_point(union real_point3d const *a, union real_point3d const *center, union real_point3d const *b, float radius,
    bool left, short index, bool flag, union real_point3d *out)
{
    radius += 0.05f;
    *out = *center;
    real_point2d tangents[2];
    find_tangent_point(&a->xy, &center->xy, radius, left, &tangents[0]);
    find_tangent_point(&b->xy, &center->xy, radius, !left, &tangents[1]);
    float distance = radius;
    bool success = false;
    real_vector2d directions[2];
    vector_from_points2d(&center->xy, &tangents[0], &directions[0]);
    vector_from_points2d(&center->xy, &tangents[1], &directions[1]);
    float cross = cross_product2d(&directions[0], &directions[1]);
    if (!(fabsf(cross - 0.0f) < 0.0001f))
    {
        float radius_squared = radius * radius;
        cross = radius_squared / cross;
        set_real_point2d(
            &out->xy, center->x - cross * (directions[0].j - directions[1].j), center->y + cross * (directions[0].i - directions[1].i));
        success = true;
    }
    if (!success || distance_squared2d(&center->xy, &out->xy) > (radius * radius) * 4.0f)
    {
        real_vector2d direction;
        vector_from_points2d(&a->xy, &tangents[0], &direction);
        if (path_smoothing_normalize2d(&direction) == 0.0f)
            direction = *global_left2d;
        point_from_line2d(&tangents[0], &direction, distance, &out->xy);
    }
}
// @address 0x575930 ?find_tangent_point@@YAXPBTreal_point2d@@0M_NPAT1@@Z
void find_tangent_point(const real_point2d *point, const real_point2d *center, float radius, bool left, real_point2d *tangent)
{
    const float zero = 0.0f;
    float center_x = center->x;
    real_vector2d v;
    vector_from_points2d(center, point, &v);
    float magnitude = magnitude_squared2d(&v);
    float scale = magnitude > zero ? radius / magnitude : zero;
    float square = magnitude - radius * radius;
    if (square > zero)
    {
        square = sqrtf(square);
        real_point2d candidates[2];
        set_real_point2d(&candidates[0], center_x + scale * (v.i * radius + v.j * square), center->y + scale * (v.j * radius - v.i * square));
        set_real_point2d(&candidates[1], center_x + scale * (v.i * radius - v.j * square), center->y + scale * (v.j * radius + v.i * square));
        real_vector2d directions[2];
        vector_from_points2d(point, &candidates[0], &directions[0]);
        vector_from_points2d(point, &candidates[1], &directions[1]);
        *tangent = candidates[(cross_product2d(&directions[0], &directions[1]) > zero) != left];
    }
    else
    {
        real_vector2d direction;
        vector_from_points2d(center, point, &direction);
        float (*normalize)(real_vector2d *) = normalize2d;
        if (normalize(&direction) == zero)
            direction = *global_left2d;
        tangent->x = center_x + direction.i * radius;
        tangent->y = center->y + direction.j * radius;
    }
}

static const float k_path_smoothing_two_pi = 6.2831855f;

// @address 0x575ab0 ?choose_turning_point@@YA_NPBTreal_point3d@@0000@Z
bool choose_turning_point(const real_point3d *start, const real_point3d *left, const real_point3d *right, const real_point3d *middle, const real_point3d *end)
{
    bool result;
    real_vector2d path;
    vector_from_points2d(&middle->xy, &end->xy, &path);
    if (path_smoothing_normalize2d(&path) <= 0.0f)
    {
        result = true;
    }
    else
    {
        real_vector2d perpendicular = { -path.j, path.i };
        real_vector2d left_middle, right_middle;
        normalize2d(vector_from_points2d(&left->xy, &middle->xy, &left_middle));
        normalize2d(vector_from_points2d(&right->xy, &middle->xy, &right_middle));
        float right_dot = dot_product2d(&right_middle, &perpendicular);
        float left_dot = dot_product2d(&left_middle, &perpendicular);
        if (fabs(right_dot) < 0.001)
            right_dot = 0.0f;
        if (fabs(left_dot) < 0.001)
            left_dot = 0.0f;
        if (right_dot * left_dot >= 0.0f)
        {
            real_vector2d left_start, right_start, left_end, right_end;
            normalize2d(vector_from_points2d(&left->xy, &start->xy, &left_start));
            normalize2d(vector_from_points2d(&right->xy, &start->xy, &right_start));
            normalize2d(vector_from_points2d(&left->xy, &end->xy, &left_end));
            normalize2d(vector_from_points2d(&right->xy, &end->xy, &right_end));
            float left_start_angle = signed_angle_between_vectors2d(&left_start, &left_middle);
            float left_end_angle = signed_angle_between_vectors2d(&left_middle, &left_end);
            float right_start_angle = signed_angle_between_vectors2d(&right_start, &right_middle);
            float right_end_angle = signed_angle_between_vectors2d(&right_middle, &right_end);
            if (right_end_angle > 0.0f)
                right_end_angle -= k_path_smoothing_two_pi;
            if (left_end_angle < 0.0f)
                left_end_angle += k_path_smoothing_two_pi;
            float left_angle = left_start_angle + left_end_angle;
            float right_angle = -(right_start_angle + right_end_angle);
            result = left_angle < right_angle;
        }
        else
        {
            float left_distance = distance2d(&start->xy, &left->xy) + distance2d(&end->xy, &left->xy);
            float right_distance = distance2d(&start->xy, &right->xy) + distance2d(&end->xy, &right->xy);
            result = left_distance < right_distance;
        }
    }
    return result;
}

static __forceinline bool path_smoothing_process_step(const path_step *step)
{
    return step->type == NONE && step->sector.valid();
}

// @address 0xce208 ?path_smooth@@YAXPAUpath_state@@FPBUpath_step@@PAFPAU2@PA_N@Z
void path_smooth(path_state *state, short raw_step_count, const path_step *raw_steps, short *smoothed_step_count, path_step *smoothed_steps,
    bool *steps_finish_path)
{
    float radius = state->input.radius;

    if (raw_step_count > 1)
    {
        short raw_step_index = 0;
        short smoothed_step_index = 0;
        bool finished = false;
        real_point3d current_point = *state->input.start.get_point();
        short current_reference_frame = state->input.start.reference_frame();
        c_sector_ref current_sector_ref = raw_steps->sector;

        while (raw_step_index < raw_step_count)
        {
            pathfinding_sector *current_sector = 0;
            long collision_link_index = 0xffff;
            short collision_step_index = NONE;
            short collision_structure_bsp_index = NONE;
            bool collision = false;
            short hint_type = NONE;
            short projection = 2;
            bool projection_sign = true;
            c_hint_ref hint_ref;
            hint_ref.clear();

            if (current_sector_ref.valid())
            {
                current_sector = pathfinding_get_sector(current_sector_ref);
                real_vector3d normal;
                get_sector_normal(current_sector, &normal, true);
                ai_reference_frame_transform_vector(current_reference_frame, &normal, &normal);
                projection = projection_from_vector3d(&normal);
                projection_sign = projection_sign_from_vector3d(&normal, projection);
            }

            bool step_valid = path_smoothing_process_step(&raw_steps[raw_step_index]);
            if (!step_valid || !current_sector)
            {
                collision = true;
                collision_step_index = raw_step_index;
                collision_structure_bsp_index = raw_steps->sector.structure_bsp_index;
                hint_ref = raw_steps[raw_step_index].hint;
                hint_type = raw_steps[raw_step_index].type;
            }
            else
            {
                long sector_flags = current_sector->flags & 0x3f0;
                if (sector_flags & 0x10)
                    sector_flags |= 0x1000;
                else if (current_sector->flags & 0x1000)
                    sector_flags |= 0x10;
                if (current_sector->flags & 0x3c0)
                {
                    if (sector_flags & 0xc0)
                        sector_flags |= 0x300;
                    else if (sector_flags & 0x300)
                        sector_flags |= 0xc0;
                }
                else if (current_sector->flags & 0x20)
                {
                    sector_flags |= 0x3c0;
                }

                short step_index;
                for (step_index = raw_step_index; step_index < raw_step_count; step_index++)
                {
                    bool step_passable = true;
                    if (!path_smoothing_process_step(&raw_steps[step_index]))
                    {
                        step_passable = false;
                    }
                    else
                    {
                        pathfinding_sector *step_sector = pathfinding_get_sector(raw_steps[step_index].sector);
                        step_passable = (step_sector->flags & sector_flags) != 0;
                    }
                    if (!step_passable)
                    {
                        collision_step_index = step_index;
                        collision_structure_bsp_index = NONE;
                        collision_link_index = 0xffff;
                        collision = true;
                        break;
                    }
                }

                for (short test_step_index = step_index - 1; test_step_index >= raw_step_index; test_step_index--)
                {
                    pathfinding_test_pill2d_result pill_result;
                    real_vector3d direction;
                    vector_from_points3d(&current_point, raw_steps[test_step_index].point.get_point(), &direction);
                    float distance = normalize3d(&direction);
                    if (distance > 0.0f &&
                        pathfinding_test_pill2d(false, 0, NONE, projection, projection_sign, &current_point, current_sector_ref,
                            raw_steps[test_step_index].sector, &direction, radius, distance, false, test_step_index == raw_step_count - 1,
                            false, &state->error, &pill_result))
                    {
                        collision_step_index = test_step_index;
                        collision_structure_bsp_index = pill_result.structure_bsp_index;
                        collision_link_index = pill_result.link_index;
                        collision = true;
                    }
                    else
                    {
                        break;
                    }
                }
            }

            if (collision)
            {
                bool smoothed = false;
                if (step_valid && collision_link_index != NONE && collision_link_index != 0xffff &&
                    path_smoothing_process_step(&raw_steps[collision_step_index]))
                {
                    real_point3d projected_point;
                    project_point3d(&current_point, projection, projection_sign, &projected_point);
                    real_point3d clockwise_point;
                    bool clockwise_valid = find_turning_point(collision_structure_bsp_index, &projected_point, radius, collision_link_index,
                        true, projection, projection_sign, state->input.allow_broken_sectors, &clockwise_point);
                    real_point3d counterclockwise_point;
                    bool counterclockwise_valid = find_turning_point(collision_structure_bsp_index, &projected_point, radius,
                        collision_link_index, false, projection, projection_sign, state->input.allow_broken_sectors, &counterclockwise_point);
                    if (counterclockwise_valid && clockwise_valid)
                    {
                        const real_point3d *previous_point = collision_step_index > 0
                            ? raw_steps[collision_step_index - 1].point.get_point()
                            : state->input.start.get_point();
                        real_point3d projected_previous_point;
                        project_point3d(previous_point, projection, projection_sign, &projected_previous_point);
                        real_point3d projected_collision_point;
                        project_point3d(
                            raw_steps[collision_step_index].point.get_point(), projection, projection_sign, &projected_collision_point);
                        bool clockwise = choose_turning_point(&projected_point, &clockwise_point, &counterclockwise_point,
                            &projected_previous_point, &projected_collision_point);
                        for (short attempt = 0; attempt < 2; attempt++)
                        {
                            real_point3d turning_point;
                            if (clockwise)
                                turning_point = clockwise_point;
                            else
                                turning_point = counterclockwise_point;
                            real_point3d avoidance_point;
                            find_avoidance_point(&projected_point, &turning_point, &projected_collision_point, radius, clockwise,
                                projection, projection_sign, &avoidance_point);
                            real_point3d new_point;
                            inverse_project_point3d(&avoidance_point, projection, projection_sign, &new_point);
                            path_collision_result collision_result;
                            pathfinding_test_line(&current_point, current_sector_ref, &new_point, g_sector_none, &state->error, &collision_result);
                            if (!collision_result.collision)
                            {
                                c_sector_ref new_sector_ref = collision_result.sector;
                                path_collision_result final_collision_result;
                                if (!pathfinding_test_line(&new_point, new_sector_ref, raw_steps[collision_step_index].point.get_point(),
                                        raw_steps[collision_step_index].sector, &state->error, &final_collision_result))
                                {
                                    current_sector_ref = new_sector_ref;
                                    current_point = new_point;
                                    current_reference_frame = raw_steps[collision_step_index].point.reference_frame();
                                    smoothed = true;
                                    break;
                                }
                            }
                            clockwise = !clockwise;
                        }
                    }
                }

                if (!smoothed)
                {
                    if (collision_step_index == 0 || collision_step_index == raw_step_index)
                    {
                        current_point = *raw_steps[collision_step_index].point.get_point();
                        current_sector_ref = raw_steps[collision_step_index].sector;
                        current_reference_frame = raw_steps[collision_step_index].point.reference_frame();
                        collision_step_index = raw_step_index + 1;
                    }
                    else
                    {
                        current_point = *raw_steps[collision_step_index - 1].point.get_point();
                        current_reference_frame = raw_steps[collision_step_index - 1].point.reference_frame();
                        current_sector_ref = raw_steps[collision_step_index - 1].sector;
                    }
                }

                path_step *smoothed_step = &smoothed_steps[smoothed_step_index];
                smoothed_step_index++;
                smoothed_step->sector = current_sector_ref;
                smoothed_step->point.set(&current_point, current_reference_frame);
                smoothed_step->flags = raw_steps[raw_step_index].flags;
                if (hint_type != NONE)
                {
                    smoothed_step->type = raw_steps[raw_step_index].type;
                    smoothed_step->hint = hint_ref;
                }
                else
                {
                    smoothed_step->type = NONE;
                    smoothed_step->hint.clear();
                }

                if (smoothed_step_index >= 4)
                    break;
                else if (!state->input.allow_narrow_links && ai_distance_squared3d(&state->input.start, &smoothed_step->point) > 400.0f)
                    break;
                raw_step_index = collision_step_index;
            }
            else
            {
                smoothed_steps[smoothed_step_index] = raw_steps[raw_step_count - 1];
                smoothed_step_index++;
                finished = true;
                break;
            }
        }

        *smoothed_step_count = smoothed_step_index;
        if (!finished)
            *steps_finish_path = false;
    }
    else
    {
        *smoothed_step_count = 1;
        smoothed_steps[0] = raw_steps[0];
    }
}
