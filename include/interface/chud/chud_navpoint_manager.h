#ifndef INTERFACE_CHUD_CHUD_NAVPOINT_MANAGER_H
#define INTERFACE_CHUD_CHUD_NAVPOINT_MANAGER_H
#include "interface/chud/chud_navpoints.h"
#include "interface/chud/chud_navpoint_types.h"
#include "interface/chud/chud_point_tracker_types.h"
#include <string.h>
#include <math.h>

class c_chud_cinematic_fade {
public:
    void set_fade(float, float);
    __forceinline void update()
    {
        if (m_fade < m_target_fade)
        {
            m_fade += (float)fabs(m_fade_rate);
            if (m_fade > m_target_fade)
            {
                m_fade = m_target_fade;
                m_fade_rate = 0.0f;
            }
        }
        else if (m_fade > m_target_fade)
        {
            m_fade -= (float)fabs(m_fade_rate);
            if (m_fade < m_target_fade)
            {
                m_fade = m_target_fade;
                m_fade_rate = 0.0f;
            }
        }
    }
public:
    __forceinline float get_fade() { return m_fade; }
private:
    float m_fade;
    float m_target_fade;
    float m_fade_rate;
};

class c_chud_navpoint_manager
{
public:
    struct s_managed_navpoint
    {
        long id;
        long arrow_index;
        long icon_index;
        long color_type;
        unsigned long flags;
        float alpha;
        wchar_t text[4];
        real_point3d position;
        bool valid;
        long time;
        float scale;
        c_chud_point_tracker tracker;
        long state;
        long count;
        float progress;
    };
    void submit_navpoint(s_chud_navpoint *navpoint);
    s_managed_navpoint navpoints[20];
    long selected_player_index;
    wchar_t text[17];
    float text_progress;
    float text_width;
    long selected_index;
    float distance_progress;
    wchar_t distance_text[10];
    float distance_width;
    float distance_index;
    long campaign_object_index;
    short scripted_navpoint_cutscene_flag_indices[8];
    float scripted_navpoint_offsets[8];
    long scripted_navpoint_count;
    bool campaign_navpoints_enabled;
    void update(long user_index);
    __forceinline void reset()
    {
        for (long i = 0; i < 20; i++)
            navpoints[i].valid = false;
        selected_player_index = -1;
        selected_index = -1;
        campaign_object_index = -1;
        scripted_navpoint_count = 0;
        campaign_navpoints_enabled = true;
        memset(scripted_navpoint_cutscene_flag_indices, -1, sizeof(scripted_navpoint_cutscene_flag_indices));
        memset(scripted_navpoint_offsets, 0, sizeof(scripted_navpoint_offsets));
    }
    __forceinline void add_scripted_navpoint(short type, short navpoint_id, float value)
    {
        if (type != -1 && navpoint_id != -1 && scripted_navpoint_count < 8) {
            scripted_navpoint_cutscene_flag_indices[scripted_navpoint_count] = navpoint_id;
            scripted_navpoint_offsets[scripted_navpoint_count] = value;
            scripted_navpoint_count++;
        }
    }
private:
    void submit_scripted_navpoints(long user_index);
    long determine_distanced_navpoint(long user_index);
    friend void chud_draw_navpoints(c_chud_navpoint_manager *, long);
    static unsigned long get_managed_navpoint_arrow_color(s_managed_navpoint *navpoint);
    static unsigned long get_managed_navpoint_text_color(s_managed_navpoint *navpoint);
};

typedef char chud_navpoint_manager_size[(sizeof(c_chud_navpoint_manager) == 0x950) ? 1 : -1];

#endif // INTERFACE_CHUD_CHUD_NAVPOINT_MANAGER_H
