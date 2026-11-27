#ifndef INTERFACE_GUI_SCREENS_GUI_SCREEN_IN_PROGRESS_H
#define INTERFACE_GUI_SCREENS_GUI_SCREEN_IN_PROGRESS_H

class c_gui_screen_widget;
class c_gui_widget;

extern "C" void gui_screen_in_progress_update_player_display(c_gui_screen_widget *screen);
extern "C" void gui_screen_in_progress_update_title(c_gui_screen_widget *screen);
extern "C" void gui_screen_in_progress_update_rank_widget(c_gui_widget *widget, unsigned int row, unsigned int column);

#include "cseries/cseries.h"
#include "cseries/crt_xdk.h"
#include "interface/c_gui_screen_widget.h"

class c_gui_screen_in_progress : public c_gui_screen_widget
{
public:
    __forceinline c_gui_screen_in_progress(long name) :
        c_gui_screen_widget(name), m_title(NONE), m_message(NONE), m_show_progress(false)
    {
    }
    virtual void update(unsigned long current_milliseconds);

    __forceinline void setup(long title, long message) { m_title = title; m_message = message; }
    __forceinline void set_show_progress(bool show_progress) { m_show_progress = show_progress; }

    long m_title;
    long m_message;
    // Progress screens use the flag; rank displays use the whole word.
    union { long m_second_rank; bool m_show_progress; };
    long m_second_grade;
};

typedef char gui_in_progress_show_progress_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_screen_in_progress, m_show_progress) == 0x1a90))) ? 1 : -1];

class c_gui_screen_spartan_dialog : public c_gui_screen_widget
{
public:
    virtual void update_render_state(unsigned long current_milliseconds);

protected:
    __forceinline c_gui_screen_spartan_dialog(long name) : c_gui_screen_widget(name) {}
};

class c_gui_screen_spartan_milestone_dialog : public c_gui_screen_spartan_dialog
{
public:
    __forceinline c_gui_screen_spartan_milestone_dialog(long name) :
        c_gui_screen_spartan_dialog(name), m_milestone(0), m_input_enable_time(0)
    {
        m_input_enable_time = GetTickCount() + 2000;
    }

    virtual void update(unsigned long current_milliseconds);
    virtual bool handle_controller_input_message(const c_controller_input_message *message);

private:
    long m_milestone;
    unsigned long m_input_enable_time;
};

class c_gui_screen_spartan_rank_dialog : public c_gui_screen_spartan_dialog
{
public:
    __forceinline c_gui_screen_spartan_rank_dialog(long name) :
        c_gui_screen_spartan_dialog(name), m_previous_rank(0), m_previous_grade(0), m_current_rank(0), m_current_grade(0)
    {
    }

    virtual void initialize();
    virtual void update(unsigned long current_milliseconds);

private:
    long m_previous_rank;
    long m_previous_grade;
    long m_current_rank;
    long m_current_grade;
};

typedef char spartan_milestone_dialog_size[(sizeof(void *) != 4 || sizeof(c_gui_screen_spartan_milestone_dialog) == 0x1a90) ? 1 : -1];
typedef char spartan_rank_dialog_size[(sizeof(void *) != 4 || sizeof(c_gui_screen_spartan_rank_dialog) == 0x1a98) ? 1 : -1];

#endif // INTERFACE_GUI_SCREENS_GUI_SCREEN_IN_PROGRESS_H
