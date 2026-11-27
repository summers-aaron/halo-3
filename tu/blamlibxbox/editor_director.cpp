// blamlibxbox/editor_director.obj: from the records 0x295130 0x2952b0 0x2952b8
// @flags /TP /O2 /Oc
#include "game/player_mapping_globals.h"
#include "game/player_mapping_types.h"
#include "game/players.h"
#include "math/matrix_math_types.h"
#include "math/real_math_types.h"
#include "tag_files/tag_types.h"
#include "camera/editor_director.h"
#include "scenario/scenario_tag.h"
#include "camera/camera_types.h"
#include "math/real_math_functions.h"
#include "math/real_math_globals.h"
#include "scenario/scenario_player_starting_location.h"
#include "scenario/scenario.h"

// March editor_director_save_position and the constructor use three separate
// statics.
// match: 0x295130: one sparse BSS section preserves the saved-position base across foreign storage.
// @split_bss
// @sparse_bss 0x828c426a..0x828c4298
// @address 0x8c426a g_editor_saved_camera_valid
static bool g_editor_saved_camera_valid;
// @address 0x8c4284 g_editor_saved_position
static real_point3d g_editor_saved_position;
// @address 0x8c4290 g_editor_saved_angles
static real_euler_angles2d g_editor_saved_angles;
// @address 0x40e58 ??_7c_editor_director@@6B@
// @address 0x428ad0 ?get_type@c_editor_director@@UBA?AW4e_director_mode@@XZ
// @address 0x42d298 ?should_draw_hud@c_editor_director@@UBA_NXZ
// @address 0x42d298 ?inhibits_facing@c_director@@UBA_NXZ
// @address 0x42d298 ?inhibits_input@c_director@@UBA_NXZ
// @address 0x42d298 ?should_draw_hud_saved_film@c_editor_director@@UBA_NXZ
// @address 0x42d380 ?can_use_camera_mode@c_director@@UBA_NW4e_camera_mode@@@Z
// @address 0x201558 ?handle_deleted_player@c_director@@UAAXJ@Z

// @address 0x295130 ??0c_editor_director@@QAA@J@Z
c_editor_director::c_editor_director(long user_index) : c_director(user_index)
{
    set_camera_mode(_camera_mode_flying, 0.0f, 0);
    c_flying_camera *camera = static_cast<c_flying_camera *>(get_camera());
    real_vector3d forward = *global_forward3d;
    long current_user_index = m_user_index;
    if (current_user_index == inl_next_active_user(-1) && g_editor_saved_camera_valid) {
        vector3d_from_euler_angles2d(&forward, &g_editor_saved_angles);
        camera->set_position(&g_editor_saved_position);
        g_editor_saved_camera_valid = false;
    } else if (global_scenario->player_starting_locations.count != 0 && global_scenario->player_starting_locations.address != 0) {
        s_scenario_player_starting_location *location = (s_scenario_player_starting_location *)global_scenario->player_starting_locations.address;
        vector3d_from_angle(&forward, location->facing);
        camera->set_position(&location->position);
    }
    camera->set_forward(&forward);
    camera->set_lock_in_xy_plane(true);
    camera->set_collision(false);
    m_unit_focus = -1;
    m_scripted = false;
    m_use_roll = false;
}

// @address 0x2952b8 ?handle_deleted_object@c_editor_director@@UAAXJ@Z
void c_editor_director::handle_deleted_object(long object_index)
{
    get_camera()->handle_deleted_object(object_index);
    if (m_unit_focus == object_index)
        m_unit_focus = -1;
}

// @address 0x2952b0 ?update@c_editor_director@@UAAXM@Z
void c_editor_director::update(float dt)
{
    c_director::update(dt);
}

struct s_memory_pool;
// @address 0x8c4268 g_num_entity_iterators_in_use
extern "C" {
short g_num_entity_iterators_in_use;
}
// @address 0x8c4274 g_overlapped_memory_pool
extern "C" {
s_memory_pool *g_overlapped_memory_pool;
}
