// interfacelibxbox/user_interface_objectives.obj: from the records 0x211d60 0x495878 0x495888 0x4959c8 0x495b10 0x495c00 0x495cf0 0x495d90 0x495da8
// @compiler xdk6130
#include "scenario/scenario.h"
// @address 0x11df83c g_objectives_state
extern "C" {
__declspec(thread) signed char *g_objectives_state;
}
#include "multithreading/thread_tls.h"
// @flags /TP /O2
#include "memory/static_arrays.h"
#include "cache/restricted_memory_regions.h"
#include "text/unicode.h"
#include "interface/user_interface_objectives.h"
#include "game/player_control_deterministic_globals.h"
#include "interface/gui_location_manager.h"
#include "interface/user_interface_objectives_allocator.h"
#include "scenario/scenario_tag.h"
#include "cseries/language.h"
#include "text/text_group.h"
#include "memory/tls_data_arrays.h"
#include "cache/restricted_memory_layout.h"

#include "cache/restricted_memory.h"
#include "cache/restricted_memory_registration.h"
#include "cache/restricted_memory_regions_inline.h"

// @address 0x1ab008 ?ustrnzcat@@YAPA_WPA_WPB_WJ@Z
// @address 0x1d6bb0 ?restricted_region_free_member@@YAXJJ@Z


// @address 0x4fd78 k_objectives_line_break
// @rdata 0x8204fd78..0x8204fd80
const wchar_t k_objectives_line_break[] = { 0x000d, 0x000a, 0x0000, 0x0000 };
// @rdata-end

static __forceinline scenario *user_interface_objectives_scenario() { return global_scenario; }

void __tls_set_g_objectives_allocator(void *allocation)
{
    g_player_control_globals_deterministic = (s_player_control_deterministic_globals *)allocation;
}

// @address 0x495878 __tls_set_g_objectives_state_allocator
void __tls_set_g_objectives_state_allocator(void *address)
{
    g_objectives_state = (signed char *)address;
}

// @address 0x495b10 ?primary_objectives_string_id_from_index@@YAJJ@Z
long primary_objectives_string_id_from_index(long index)
{
    long result = 0;
    switch (index) {
    case 0:
        result = 0x10432;
        break;
    case 1:
        result = 0x10433;
        break;
    case 2:
        result = 0x10434;
        break;
    case 3:
        result = 0x10435;
        break;
    case 4:
        result = 0x10436;
        break;
    case 5:
        result = 0x10437;
        break;
    case 6:
        result = 0x10438;
        break;
    case 7:
        result = 0x10439;
        break;
    case 8:
        result = 0x1043a;
        break;
    case 9:
        result = 0x1043b;
        break;
    }
    return result;
}

// @address 0x495c00 ?secondary_objectives_string_id_from_index@@YAJJ@Z
long secondary_objectives_string_id_from_index(long index)
{
    long result = 0;
    switch (index) {
    case 0:
        result = 0x1043c;
        break;
    case 1:
        result = 0x1043d;
        break;
    case 2:
        result = 0x1043e;
        break;
    case 3:
        result = 0x1043f;
        break;
    case 4:
        result = 0x10440;
        break;
    case 5:
        result = 0x10441;
        break;
    case 6:
        result = 0x10442;
        break;
    case 7:
        result = 0x10443;
        break;
    case 8:
        result = 0x10444;
        break;
    case 9:
        result = 0x10445;
        break;
    }
    return result;
}

// @address 0x495cf0 ?reserve_memory@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_objectives_allocator@@YAXPAX@Z@@QAAPAXPBD0IJ@Z
template<>
void *t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0), &__tls_set_g_objectives_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
  int member_index = restricted_memory_register_member(&g_restricted_regions[1], name, type, size, 0,
      __tls_set_g_objectives_state_allocator, 0, 0);
  unsigned int offset = g_restricted_regions[1].get_member_offset(member_index);
  void *address = restricted_region_get_base_address(1) + offset;
  int owner = get_current_thread_index();
  m_thread_id = owner;
  m_member_index = member_index;
  g_objectives_state = (signed char *)address;
  return restricted_region_get_member_address(1, member_index);
}


// @address 0x495da8 ?deallocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_objectives_allocator@@YAXPAX@Z@@UAAXPAX@Z
template <> void t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_objectives_allocator>::deallocate(void *memory)
{
    long value = m_member_index;
    g_objectives_state = 0;
    restricted_region_free_member(1, value);
}

// @address 0x495d90 ?allocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_objectives_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<> void *t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_objectives_allocator>::allocate(unsigned int size, const char *name)
{
    return (void *)reserve_memory(name, "", size, 0);
}

// @address 0x4959c8 ?user_interface_build_secondary_objectives_text@@YAXPAV?$c_static_wchar_string@$0BAA@@@@Z
void user_interface_build_secondary_objectives_text(c_static_wchar_string<256> *text)
{
    long list = user_interface_objectives_scenario()->objectives_string_list.index;
    text->clear();
    if (list != -1)
    {
        long index = 0;
        e_language language = g_current_language;
        e_language call_language = g_current_language;
        do
        {
            if ((unsigned long)index <= 9)
            {
                int state = g_objectives_state[index + 10];
                if (state != 0)
                {
                    c_static_wchar_string<256> line;
                    line.clear();
                    if (state == 2)
                    {
                        wchar_t s[3] = { 0x2610, 32, 0 };
                        text->append(s);
                    }
                    else if (state == 1)
                    {
                        wchar_t s[3] = { 0x2717, 32, 0 };
                        text->append(s);
                    }
                    else
                    {
                        wchar_t s[3] = { 0x2611, 32, 0 };
                        text->append(s);
                    }
                    int string_id = secondary_objectives_string_id_from_index(index);
                    if (call_language == -1)
                    {
                        unsigned long xbox_language = XTLGetLanguage();
                        language = ::get_e_language_from_xbox_language(xbox_language);
                        g_current_language = language;
                        call_language = language;
                    }
                    call_language = language;
                    string_list_get_normal_string_by_language(list, string_id, language, &line);
                    wchar_t *line_end = ustrnzcat(text->m_string, line.m_string, 256);
                    text->m_string[255] = 0;
                    ustrnzcat(line_end, k_objectives_line_break, 256);
                    text->m_string[255] = 0;
                }
            }
            index++;
        } while (index < 10);
    }
}

// @address 0x495888 ?user_interface_build_primary_objectives_text@@YAXPAV?$c_static_wchar_string@$0BAA@@@@Z
void user_interface_build_primary_objectives_text(c_static_wchar_string<256> *text)
{
    long list = user_interface_objectives_scenario()->objectives_string_list.index;
    text->clear();
    if (list != -1)
    {
        long index = 0;
        e_language language = g_current_language;
        e_language call_language = g_current_language;
        do
        {
            if ((unsigned long)index <= 9)
            {
                int state = g_objectives_state[index];
                if (state != 0)
                {
                    c_static_wchar_string<256> line;
                    line.clear();
                    if (state == 2)
                    {
                        wchar_t s[3] = { 0x2610, 32, 0 };
                        text->append(s);
                    }
                    else if (state == 1)
                    {
                        wchar_t s[3] = { 0x2717, 32, 0 };
                        text->append(s);
                    }
                    else
                    {
                        wchar_t s[3] = { 0x2611, 32, 0 };
                        text->append(s);
                    }
                    int string_id = primary_objectives_string_id_from_index(index);
                    if (call_language == -1)
                    {
                        language = ::get_e_language_from_xbox_language(XTLGetLanguage());
                        g_current_language = language;
                        call_language = language;
                    }
                    string_list_get_normal_string_by_language(list, string_id, language, &line);
                    wchar_t *line_end = ustrnzcat(text->m_string, line.m_string, 256);
                    text->m_string[255] = 0;
                    ustrnzcat(line_end, k_objectives_line_break, 256);
                    text->m_string[255] = 0;
                }
            }
            index++;
        } while (index < 10);
    }
}


c_objectives_allocation_manager g_objectives_allocator;

// 0x827c341c bytes: 01 00 00 00 00 00 00 00 00 00 00 00 FF FF FF FF 00 00 00 1E
// @address 0x7c341c g_lobby_location_manager
c_gui_location_manager g_lobby_location_manager = { true, 0, false, 0xFFFFFFFF, 30 };
