#ifndef INCLUDE_INTERFACE_GUI_PLAYER_SELECT_DEPENDENCIES_H
#define INCLUDE_INTERFACE_GUI_PLAYER_SELECT_DEPENDENCIES_H

#include "shell/shell_types.h"
struct s_medal;

extern "C" bool medal_compare(s_medal a, s_medal b, void *context);
// PC port: ai_dialogue.cpp defines extern "C" bool dialogue_compare_player_weighting(long, long, const void *).
extern "C" bool dialogue_compare_player_weighting(long first_actor_index, long second_actor_index, const void *context);
extern "C" void online_guide_update_get_service_record(void *widget, e_controller_index controller, unsigned char flag);

class c_gui_widget;
class c_player_select_focus_item;

extern "C" unsigned long XUserAreUsersFriends(unsigned long user_index, const unsigned __int64 *xuids, unsigned long xuid_count,
    int *results, void *overlapped);
extern "C" long g_experience_rank_icon_element_handles[14][4];        // 14 ranks by 4 grades of the rank icon element handles
extern "C" c_player_select_focus_item *gui_widget_find_child_by_name(c_gui_widget *widget, long type);
extern "C" c_player_select_focus_item *gui_widget_get_next_list_item(c_gui_widget *widget, bool require_valid);
extern "C" unsigned long XShowPlayerReviewUI(unsigned long user_index, unsigned __int64 xuid);
extern "C" unsigned long XShowFriendRequestUI(unsigned long user_index, unsigned __int64 xuid);

#endif // INCLUDE_INTERFACE_GUI_PLAYER_SELECT_DEPENDENCIES_H
