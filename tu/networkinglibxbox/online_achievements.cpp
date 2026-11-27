// networkinglibxbox/online_achievements.obj: from the records 0x3a4ba0 0x3a4bb0 0x3a4c88 0x3a4d80 0x3a4de0 0x3a4df0 0x3a4f78 0x3a4fb8 0x3a50c0 0x3a5160 0x3a53c0 0x3a5458
#include "cseries/crt_xdk.h"
#include "shell/shell.h"
#include <string.h>
#include <ppcintrinsics.h>
#include <stdlib.h>
#include "game/game.h"
#include "cseries/async_xoverlapped.h"
#include "cseries/async_xoverlapped_memory.h"
#include "math/integer_math.h"
#include "memory/bit_vectors.h"
#include "networking/online/online_achievements.h"
#include "game/game_time.h"
#include "interface/controller_types.h"
#include "interface/achievement_globals_types.h"
#include "interface/controller_achievements_types.h"
#include "cseries/async_xoverlapped_types.h"
#include "interface/c_gui_screen_widget.h"
#include "xbox/online_services.h"
#include "cseries/async_xoverlapped_allocation.h"
#include "memory/tls_globals.h"
#include "interface/controller_inline.h"
#include "cutscene/cinematic_globals_types.h"
#include "networking/online/online_globals_types.h"
#include "interface/c_window_manager.h"
#include "interface/user_interface_window_manager.h"
// @flags /TP /O2 /Oc
// @address 0x3a4bb0 ?start@c_achievements_enumerate_overlapped_task@@UAAKPAX@Z
// @address 0x3a4c88 ?success@c_achievements_enumerate_overlapped_task@@UAAXK@Z
// @address 0x3a53c0 ?gamer_has_achievement@@YA_NW4e_controller_index@@W4e_achievement@@@Z
// @address 0x3a5458 ?gamer_grant_achievement@@YAXW4e_controller_index@@W4e_achievement@@@Z
// @address 0x119de8 ?bit_vector_lowest_bit_set@@YAJPBKJ@Z
// @address 0x20e6a0 ?overlapped_task_start_internal@@YA_NPAVc_overlapped_task@@PBDJ@Z
// @address 0x8dbd0 ?get_screen_by_name@c_window_manager@@QAAPAVc_gui_screen_widget@@W4e_window_index@@J@Z
// @address 0x3a5160 ?achievements_update@@YAXM@Z
// @address 0x3a4df0 ?start@c_achievement_write_overlapped_task@@UAAKPAX@Z
// @address 0x1142940 g_window_manager
// @address 0x8086b8 g_achievement_progress


class c_achievements_enumerate_overlapped_task : public c_overlapped_task {
public:
 c_achievements_enumerate_overlapped_task() : c_overlapped_task(0, 0), controller_index(k_no_controller),
     enumerator(0), details(0), achievement_count(0) {} 
 virtual const char *get_context_string() const;
 virtual void complete();
 virtual void success(unsigned long achievement_count);
 virtual unsigned long start(void *overlapped);
 e_controller_index controller_index;
 void *enumerator;
 XACHIEVEMENT_DETAILS *details;
 long achievement_count;
};
class c_achievement_write_overlapped_task : public c_overlapped_task {
public:
 c_achievement_write_overlapped_task() : c_overlapped_task(0, 0), controller_index(k_no_controller),
     achievements(), achievement_buffer(0)
 {
  memset(&achievements, 0, sizeof(achievements));
 }
 virtual unsigned long start(void *overlapped);
 virtual const char *get_context_string() const;
 virtual void complete();
 e_controller_index controller_index;
 s_controller_achievements achievements;
 XUSER_ACHIEVEMENT *achievement_buffer;
};
typedef char achievement_details_size[(sizeof(void *) != 4 || ((sizeof(XACHIEVEMENT_DETAILS) == 36))) ? 1 : -1];
typedef char achievement_write_controller_offset[(sizeof(void *) != 4 || ((offsetof(c_achievement_write_overlapped_task, controller_index) == 8))) ? 1 : -1];
typedef char achievement_write_bits_offset[(sizeof(void *) != 4 || ((offsetof(c_achievement_write_overlapped_task, achievements) == 12))) ? 1 : -1];
typedef char achievement_write_buffer_offset[(sizeof(void *) != 4 || ((offsetof(c_achievement_write_overlapped_task, achievement_buffer) == 20))) ? 1 : -1];

// @address 0x271d0 k_online_achievement_ids
// @rdata 0x820271d0..0x82027294
extern "C" {
const unsigned long k_online_achievement_ids[49] = {
    0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x10, 0x11, 0x12, 0x33, 0x36,
    0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
    0x19, 0x1A, 0x1B, 0x1F, 0x20, 0x21, 0x22, 0x34, 0x24, 0x25, 0x2A, 0x2B, 0x2C, 0x2D,
    0x26, 0x35, 0x2E, 0x23, 0x30, 0x28, 0x31
};
}
// @rdata-end
static __forceinline long achievement_get_by_id(unsigned long id) {
 long result=-1;
 for(long achievement=0;achievement<49;achievement++) { if(k_online_achievement_ids[achievement]==id) { result=achievement; break; } }
 return result;
}
#define BIT_VECTOR_TEST_FLAG(v, i) (((v)[(i) >> 5] & (1 << ((i) & 31))) != 0)
static __forceinline bool controller_has_achievement(const c_controller_interface *controller, long achievement)
{
    if ((unsigned long)achievement <= 48)
        return controller->achievements.test(achievement);
    return false;
}


static __forceinline void controller_reset_achievements(c_controller_interface *controller) {
 controller->achievements.get_writeable_bits_direct()[0] = 0;
 controller->achievements.get_writeable_bits_direct()[1] = 0;
}

const char *c_achievements_enumerate_overlapped_task::get_context_string() const
{
 return "XUserCreateAchievementEnumerator";
}

unsigned long c_achievements_enumerate_overlapped_task::start(void *overlapped)
{
 unsigned long result = 0;
 c_controller_interface *controller = controller_get(controller_index);
 controller_reset_achievements(controller);
 unsigned long buffer_size;
 result = ((c_controller_interface *)controller)->is_signed_in_to_machine()
  ? XUserCreateAchievementEnumerator(0, controller_index, 0, 32, 0, 49, &buffer_size, &enumerator)
  : 0x80004005;
 if (result == 0) {
  if (buffer_size <= sizeof(XACHIEVEMENT_DETAILS) * 49) {
   details = (XACHIEVEMENT_DETAILS *)overlapped_malloc_tracked(sizeof(XACHIEVEMENT_DETAILS) * 49, 0, 0);
   if (details) result = XEnumerate(enumerator, details, sizeof(XACHIEVEMENT_DETAILS) * 49, 0, overlapped);
   else result = 8;
  } else result = 8;
 }
 return result;
}

void c_achievements_enumerate_overlapped_task::success(unsigned long return_result) {
 c_controller_interface *controller=controller_get(controller_index);
 long achievement_count=return_result;
 if(achievement_count>49) achievement_count=49;
 for(long achievement_index=0;achievement_index<achievement_count;achievement_index++) {
  bool earned=(details[achievement_index].dwFlags & 0x20000)!=0;
  g_achievement_progress[achievement_index]=details[achievement_index].dwCred;
  if(earned) {
   unsigned long id=details[achievement_index].dwId;
   long achievement=achievement_get_by_id(id);
   if(achievement>=0 && achievement<49) controller->achievements.m_storage[achievement >> 5] |= 1 << (achievement & 31);
  }
 }
}

void c_achievements_enumerate_overlapped_task::complete()
{
 if (enumerator && enumerator != (void *)-1) {
  CloseHandle(enumerator);
  enumerator = (void *)-1;
 }
 if (details) {
  overlapped_free(details);
  details = 0;
 }
}

const char *c_achievement_write_overlapped_task::get_context_string() const
{
 return "XUserWriteAchievements";
}

void c_achievement_write_overlapped_task::complete()
{
 if (achievement_buffer) {
  overlapped_free(achievement_buffer);
  achievement_buffer = 0;
 }
}

unsigned long c_achievement_write_overlapped_task::start(void *overlapped)
{
    int achievement_count;
    achievement_count = 0;
    int achievement_index;
    achievement_index = 0;
    c_controller_interface *controller;
    controller = controller_get(this->controller_index);
    int storage_index;
    int achievement_id_or_mask;
    do {
        storage_index = achievement_index >> 5;
        achievement_id_or_mask = 1 << (achievement_index & 31);
        if (bit_vector_test(this->achievements.flags, achievement_index)) {
            int achievement_out_of_range = achievement_index < 0 ? 1 : achievement_index >= 49 ? 1 : 0;
            if (!achievement_out_of_range) {
                unsigned long *word_pointer = &controller->achievements.get_writeable_bits_direct()[storage_index];
                unsigned long &word = *word_pointer;
                word |= (unsigned int)achievement_id_or_mask;
            }
            achievement_count = achievement_count + 1;
        }
        achievement_index = achievement_index + 1;
    } while (achievement_index < 49);
    if (achievement_count > 0) {
        XUSER_ACHIEVEMENT *achievement_storage;
        achievement_storage = (XUSER_ACHIEVEMENT *)::overlapped_malloc_tracked(achievement_count * sizeof(XUSER_ACHIEVEMENT), 0, 0);
        this->achievement_buffer = achievement_storage;
        if (achievement_storage != 0) {
            storage_index = 0;
            achievement_index = 0;
            do {
                if (bit_vector_test(this->achievements.flags, achievement_index)) {
                    if ((unsigned int)achievement_index <= 48)
                        achievement_id_or_mask = k_online_achievement_ids[achievement_index];
                    if ((unsigned int)achievement_index <= 48 && (unsigned int)achievement_id_or_mask != 0) {
                        this->achievement_buffer[storage_index].dwUserIndex = this->controller_index;
                        this->achievement_buffer[storage_index].dwAchievementId = achievement_id_or_mask;
                        ++storage_index;
                    } else {
                        achievement_count = achievement_count - 1;
                    }
                }
                achievement_index = achievement_index + 1;
            } while (achievement_index < 49);
            if (achievement_count > 0 && controller->is_signed_in_to_machine()) {
                XUSER_ACHIEVEMENT *storage = this->achievement_buffer;
                return XUserWriteAchievements(achievement_count, storage, overlapped);
            } else {
                return 0x80004005;
            }
        } else {
            return 8;
        }
    } else {
        return 0x80004005;
    }
    return 0;
}

// @address 0x4462c ??_7c_achievements_enumerate_overlapped_task@@6B@
// @address 0x44650 ??_7c_achievement_write_overlapped_task@@6B@

void achievements_initialize(void)
{
 g_achievement_globals.enumerate_task = new (0, 0, _overlapped_allocation_marker) c_achievements_enumerate_overlapped_task;
 g_achievement_globals.write_task = new (0, 0, _overlapped_allocation_marker) c_achievement_write_overlapped_task;

 for (long controller = 0; controller < 4; controller++) {
  g_achievement_globals.enumeration_pending[controller] = 0;
  memset(&g_achievement_globals.controllers[controller], 0, sizeof(g_achievement_globals.controllers[controller]));
 }
 g_achievement_globals.last_update = 0.0f;
}

void achievements_dispose()
{
 if (g_achievement_globals.enumerate_task) {
  overlapped_track_delete(g_achievement_globals.enumerate_task);
  g_achievement_globals.enumerate_task = 0;
 }
 if (g_achievement_globals.write_task) {
  overlapped_track_delete(g_achievement_globals.write_task);
  g_achievement_globals.write_task = 0;
 }
}

bool gamer_has_achievement(e_controller_index controller_index, e_achievement achievement)
{
    return (unsigned char)(controller_has_achievement(controller_get(controller_index), achievement)
        || BIT_VECTOR_TEST_FLAG(g_achievement_globals.controllers[controller_index].flags, achievement));
}

void gamer_grant_achievement(e_controller_index controller_index, e_achievement achievement)
{
 if (game_is_cooperative()) return;
 s_controller *controller = controller_get(controller_index);
 if (!((c_controller_interface *)controller)->in_use()) return;
 if (((c_controller_interface *)controller)->is_guest()) return;
 if ((bool)(_rotl(controller->flags, 30) & 1)) return;
 if (gamer_has_achievement(controller_index, achievement)) return;
 g_achievement_globals.controllers[controller_index].flags[achievement >> 5] |= 1 << (achievement & 31);
}

template<class t_task> static __forceinline bool achievement_task_busy(const t_task *task)
{
 short state = (short)task->get_task_state();
 bool busy;
 if (state == 1 || state == 2 || state == 3)
  busy = true;
 else
  busy = false;
 return busy;
}
static __forceinline bool achievement_game_suppressed()
{
 bool result = false;
 if (cinematic_globals)
  result = cinematic_globals->in_progress;
 return result;
}
static __forceinline void achievement_write_initialize(c_achievement_write_overlapped_task *task, long controller, const s_controller_achievements *bits)
{
 task->controller_index = (e_controller_index)controller;
 task->achievements.flags[0] = bits->flags[0];
 task->achievements.flags[1] = bits->flags[1];
}

void achievements_update(float elapsed)
{
 s_achievement_globals *globals = &g_achievement_globals;
 long enumeration_controller = -1;
 long write_controller = -1;
 for (long controller = 0; controller < 4; ++controller)
 {
  if (enumeration_controller == -1 && globals->enumeration_pending[controller])
   enumeration_controller = controller;
  if (write_controller == -1 && bit_vector_lowest_bit_set(globals->controllers[controller].flags, 49) != -1)
   write_controller = controller;
 }
 if ((unsigned long)enumeration_controller <= 3 && globals->enumerate_task && !achievement_task_busy(globals->enumerate_task))
 {
  globals->enumerate_task->reset();
  globals->enumerate_task->controller_index = (e_controller_index)enumeration_controller;
  if (overlapped_task_start_internal((c_overlapped_task *)globals->enumerate_task, "..\\networking\\online\\online_achievements.cpp", 561))
   globals->enumeration_pending[enumeration_controller] = 0;
 }
 c_gui_screen_widget *screen = g_window_manager.get_screen_by_name(_console_window, 0x10081);
 if (game_time_is_paused() || online_guide_is_active() || screen)
  globals->last_update = 0.0f;
 else if (achievement_game_suppressed())
  globals->last_update = 2.0f;
 else if (globals->last_update > 0.0f)
  globals->last_update -= elapsed;
 if ((unsigned long)write_controller <= 3 && globals->write_task && !achievement_task_busy(globals->write_task) &&
     globals->last_update <= 0.0f)
 {
  globals->write_task->reset();
  achievement_write_initialize(globals->write_task, write_controller, &globals->controllers[write_controller]);
  if (overlapped_task_start_internal((c_overlapped_task *)globals->write_task, "..\\networking\\online\\online_achievements.cpp", 609))
   memset(&globals->controllers[write_controller], 0, sizeof(globals->controllers[write_controller]));
 }
}

// @address 0x808684 g_achievement_globals
#if !defined(_XBOX) || defined(__clang__)
s_achievement_globals g_achievement_globals;
#endif
