// interfacelibxbox/gui_screen_terminals.obj: from the records 0x495dc8 0x495e50 0x495f90 0x4964c0 0x4969f8 0x496c30 0x496fe8 0x497108 0x497388 0x497418
// @flags /TP /O2 /Oc
#include "interface/gui_widget_types.h"
#include "interface/gui_screen_widget_legacy.h"
// @address 0x11df67c players_globals
// @address 0x485fb0 ?update@c_gui_screen_widget@@UAAXK@Z
#include <new>
#include "interface/gui_custom_bitmap_widget.h"
#include "game/game_time_inline.h"
#include <stddef.h>
#include <string.h>
#include "game/game_globals_storage.h"
#include "interface/gui_widget.h"
// @address 0x4969f8 ?update@c_gui_screen_terminal@@UAAXK@Z
// @address 0x496fe8 gui_screen_terminal_has_string
// @address 0x495e50 gui_screen_terminal_try_and_get_string
// @address 0x497388 ?handle_controller_input_message@c_gui_screen_terminal@@UAA_NPBVc_controller_input_message@@@Z
// @address 0x497418 ?apply_initial_state@c_load_start_menu_pane_message@@UBAXPAVc_gui_screen_widget@@@Z
// @address 0x495dc8 ?create_bitmap_widget@c_gui_screen_terminal@@UAAPAVc_gui_bitmap_widget@@PBUs_runtime_bitmap_widget_definition@@@Z
// @address 0x4e6438 ??0c_gui_bitmap_widget@@QAA@XZ
// @address 0x491eb0 ??0c_gui_custom_bitmap_widget@@QAA@XZ
// @address 0x484e90 ?user_interface_malloc_tracked@@YAPAXIPBDJ@Z
// @address 0x48bfa0 ?get_child_widget@c_gui_widget@@QAAPAV1@W4e_gui_widget_type@@J@Z
// @address 0x489840 ?set_visible@c_gui_widget@@QAAX_N@Z
// @address 0x1fd070 ?unspatialized_impulse_sound_new@@YAJJM@Z
// @address 0x486130 ?handle_controller_input_message@c_gui_screen_widget@@UAA_NPBVc_controller_input_message@@@Z
// @address 0x1fda88 ?game_sound_impulse_new_internal@@YAJJPAUs_game_sound_impulse_internal_parameters@@@Z
#include "cseries/cseries.h"
#include <math.h>
#include <stdlib.h>
#include <ppcintrinsics.h>
#include "interface/user_interface_messages.h"
// Bungie's bit-vector macros (March: BIT_VECTOR_SET_FLAG / BIT_VECTOR_TEST_FLAG inside c_static_flags_no_init).
// Declaration debt: they belong in cseries/cseries.h beside FLAG/TEST_BIT; unit-local until that header takes them.
#define BIT_VECTOR_SIZE_IN_LONGS(count) (((count) + 31) >> 5)
#define BIT_VECTOR_SIZE_IN_BYTES(count) (BIT_VECTOR_SIZE_IN_LONGS(count) * sizeof(unsigned long))
#define BIT_VECTOR_SET_FLAG(vector, bit, enable) ((enable) ? ((vector)[(bit) >> 5] |= FLAG((bit) & 31)) : ((vector)[(bit) >> 5] &= ~FLAG((bit) & 31)))
#define BIT_VECTOR_TEST_FLAG(vector, bit) (((vector)[(bit) >> 5] & FLAG((bit) & 31)) != 0)
#include "interface/user_interface_memory.h"
#include "sound/game_sound.h"
#include "text/unicode.h"
#include "tag_files/tag_types.h"
#include "interface/gui_screens/terminals/gui_screen_terminals.h"
#include "interface/gui_screens/terminals/terminals_string_ids.h"
#include "text/text_group.h"
#include "interface/c_gui_bitmap_widget.h"
#include "interface/c_gui_text_widget.h"
#include "cache/cache_files.h"
#include "objects/object_header.h"
#include "devices/device_terminals.h"
#include "tag_files/tag_groups.h"
#include "sound/game_sound_impulse_parameters.h"


#include "interface/c_gui_screen_widget.h"


// The source payload follows the 0x3c-byte load-screen message prefix.
#include "interface/gui_screens/start_menu/load_start_menu_pane_message.h"
#include "interface/gui_screens/start_menu/start_menu_pane_screen_widget.h"

typedef c_static_wchar_string<1024> c_static_wchar_string_1024;

// @rdata 0x82024714..0x82024820
static const long k_gui_screen_terminals_progress_names[5] = {0x104fc, 0x104fd, 0x104fe, 0x104ff, 0x10500};
static const long k_gui_screen_terminals_text_names[5] = {0x104ee, 0x104ef, 0x104f0, 0x104f1, 0x104f2};
static const bool k_gui_screen_terminals_bottom_aligned[8] = {true, true, false, true, false, true, false, false};
static const long k_gui_screen_terminals_string_names_a[11] = {
    0x104e3, 0x104e4, 0x104e5, 0x104e6, 0x104e7, 0x104e8, 0x104e9, 0x104ea, 0x104eb, 0x104ec, 0x104ed};
static const long k_gui_screen_terminals_string_names_b[29] = {
    0x1050a, 0x1050b, 0x1050c, 0x1050d, 0x1050e, 0x1050f, 0x10510, 0x10511, 0x10512, 0x10513,
    0x10514, 0x10515, 0x10516, 0x10517, 0x10518, 0x10519, 0x1051a, 0x1051b, 0x1051c, 0x1051d,
    0x1051e, 0x1051f, 0x10520, 0x10521, 0x10522, 0x10523, 0x10524, 0x10525, 0x10526};
static const long k_gui_screen_terminals_icon_names_a[3] = {0x104d1, 0x104d4, 0x104d3};
static const long k_gui_screen_terminals_icon_names_b[10] = {
    0x104d1, 0x104d4, 0x104d3, 0x104d6, 0x104d7, 0x104d8, 0x104d9, 0x104da, 0x105be, 0x105bd};
static const short k_gui_screen_terminals_string_counts[4] = {0, 5, 11, 15};
// @rdata-end




enum e_terminal_screen_state { _terminal_screen_state_none, _terminal_screen_state_reading, _terminal_screen_state_exiting, k_terminal_screen_state_count = 6 };





c_gui_bitmap_widget *c_gui_screen_terminal::create_bitmap_widget(const s_runtime_bitmap_widget_definition *definition)
{
    if (definition->name == _terminals_string_images) {
        void *memory = user_interface_malloc_tracked(sizeof(c_gui_custom_bitmap_widget), 0, 0);
        if (memory != 0) {
            return new (memory) c_gui_custom_bitmap_widget();
        }
    } else {
        void *memory = user_interface_malloc_tracked(sizeof(c_gui_bitmap_widget), 0, 0);
        if (memory != 0) {
            return new (memory) c_gui_bitmap_widget();
        }
    }
    return 0;
}

extern "C" bool gui_screen_terminal_try_and_get_string(long tag_index, short index, short group, c_static_wchar_string_1024 *string)
{
    bool result = false;
    const long *table;
    long slot;
    long string_id;
    do
    {
        if (tag_index == -1) {
            break;
        }
        if (group == 0) {
            if (index < 0 || index >= 11) {
                break;
            }
            table = k_gui_screen_terminals_string_names_a;
            slot = index;
        } else {
            if (group < 0 || group >= 14 || index != 0) {
                break;
            }
            table = k_gui_screen_terminals_string_names_b;
            slot = group;
        }
        string_id = table[slot];
        if (string_id == -1 || !multilingual_unicode_string_list_test_string(tag_index, string_id)) {
            break;
        }
        if (string != 0) {
            result = text_group_get_string(tag_index, string_id, string);
        } else {
            result = true;
        }
    } while (false);
    return result;
}

// @address 0x72fe18 ?screen_state_names@@3PAPBJA
#if !defined(_XBOX) || defined(__clang__)
static const long k_gui_screen_terminals_state_1_string_names[5] = {0x104de, 0x104df, 0x104e0, 0x104e1, 0x104e2};
// State 3 reads the fifteen names that follow the fourteen string_names_b entries.
const long *screen_state_names[4] = {
    NULL,
    k_gui_screen_terminals_state_1_string_names,
    k_gui_screen_terminals_string_names_a,
    k_gui_screen_terminals_string_names_b + 14,
};
#endif

// @address 0x7c8828 k_terminal_first_page_text_rate
float k_terminal_first_page_text_rate = 0.065f;

// @rdata 0x82022838
static const float k_gui_screen_terminals_text_rate = 0.05f;
// @rdata 0x820221fc
static const float k_gui_screen_terminals_one = 1.0f;

static const wchar_t *next_newline(const wchar_t *string, const wchar_t *current, short length)
{
    const wchar_t *result = 0;
    while (current - string < length - 1)
    {
        if (current[0] == L'|' && current[1] == L'n')
        {
            result = current;
            break;
        }
        current++;
    }
    return result;
}

static short terminal_page_index(short page_number)
{
    return (short)((page_number - 1) % 4 + 1);
}

static c_gui_widget *terminal_get_child_group_widget(c_gui_screen_terminal *terminal, long name)
{
    return ((c_gui_widget *)terminal)->get_child_widget(_gui_group_widget, name);
}

static c_gui_text_widget *terminal_get_child_text_widget(c_gui_screen_terminal *terminal, long name)
{
    return (c_gui_text_widget *)((c_gui_widget *)terminal)->get_child_widget(_gui_text_widget, name);
}

// @address 0x495f90 gui_screen_terminal_update_paged_text
void gui_screen_terminal_update_paged_text(c_gui_screen_terminal *terminal, long elapsed)
{
    long string_list = -1;
    game_globals_storage *game_globals = tls_game_globals();
    s_terminal_definition *definition = (s_terminal_definition *)tag_get('term', object_get(terminal->m_terminal_index)->definition_index);
    long campaign_difficulty;
    if (game_globals->options.game_mode == 1)
        campaign_difficulty = game_globals->options.campaign_difficulty;
    else
        campaign_difficulty = 1;
    short difficulty = (short)campaign_difficulty;
    if (difficulty >= 0 && difficulty < 4)
        string_list = definition->displays[difficulty].string_list_c.index;

    if (terminal->m_page_complete && string_list != -1)
    {
        short next_page = terminal->m_page_index + 1;
        if (gui_screen_terminal_try_and_get_string(string_list, 0, next_page, 0))
        {
            c_gui_widget *progress = terminal_get_child_group_widget(terminal, k_gui_screen_terminals_progress_names[terminal_page_index(next_page)]);
            if (progress)
            {
                progress->set_visible(true);
                elapsed = 0;
                terminal->m_page_index = next_page;
                terminal->m_page_complete = 0;
                terminal->m_page_reset_sentinel = -1;
                terminal->m_elapsed_pending = 1;
                if (next_page > 2)
                {
                    c_gui_text_widget *title = terminal_get_child_text_widget(terminal, 0x104ee);
                    if (title)
                        title->set_visible(false);
                }
            }
        }
    }

    short page = terminal_page_index(terminal->m_page_index);
    short max_lines = page != 0 ? 19 : 29;
    if (page > 0)
    {
        c_gui_text_widget *body = terminal_get_child_text_widget(terminal, 0x104ce);
        body->set_visible(false);
    }
    bool bottom_aligned = k_gui_screen_terminals_bottom_aligned[page];
    c_gui_text_widget *text_widget = terminal_get_child_text_widget(terminal, k_gui_screen_terminals_text_names[page]);
    if (!text_widget || string_list == -1)
        return;

    wchar_t *text = terminal->m_text.get_buffer();
    c_static_wchar_string<1024> string;
    bool changed = false;
    if (!terminal->m_page_complete)
    {
        text[0] = 0;
        short start_line = -1;
        short last_line = -1;
        for (short pass = 0; pass < 2; pass++)
        {
            if (!gui_screen_terminal_try_and_get_string(string_list, 0, terminal->m_page_index, &string))
                continue;

            wchar_t *line_start = string.get_buffer();
            long length = string.length();
            long remaining = elapsed;
            short string_index = 0;
            short line = 0;
            if (pass == 1 && bottom_aligned && last_line != -1)
            {
                short padding = max_lines - last_line - 1;
                for (short i = 0; i < padding; i++)
                    ustrnzcat(text, L"|n", 1024);
            }
            while (true)
            {
                wchar_t *newline = (wchar_t *)next_newline(string.get_string(), line_start, (short)length);
                if (newline)
                    newline[0] = 0;
                if (start_line != -1 && line >= start_line)
                {
                    ustrnzcat(text, line_start, 1024);
                    if (newline)
                        ustrnzcat(text, L"|n", 1024);
                }
                float delay;
                if (newline)
                {
                    float rate;
                    short page_number = terminal->m_page_index;
                    if (page_number == 0)
                        rate = k_terminal_first_page_text_rate;
                    else
                        rate = k_gui_screen_terminals_text_rate - __fsqrts((float)page_number * 0.07692308f) * 0.0425f;
                    short line_length = (short)wcslen(line_start);
                    delay = ((float)sin(6.2831855f * ((float)line_length * 0.025f)) * 0.75f + k_gui_screen_terminals_one) * rate * 1000.0f;
                }
                else
                {
                    delay = 0.0f;
                }
                remaining -= (long)delay;
                if (remaining < 0)
                {
                    last_line = line;
                    start_line = MAX(line - max_lines + 1, 0);
                    changed = true;
                    break;
                }
                if (newline)
                {
                    line_start = newline + 2;
                }
                else
                {
                    string_index++;
                    if (!gui_screen_terminal_try_and_get_string(string_list, string_index, terminal->m_page_index, &string))
                    {
                        last_line = line;
                        start_line = MAX(line - max_lines + 1, 0);
                        changed = true;
                        terminal->m_page_complete = 1;
                        break;
                    }
                    length = string.length();
                    line_start = string.get_buffer();
                }
                line++;
            }
        }
    }
    if (changed)
        text_widget->set_text(text);
}

// @address 0x7c8830 k_terminal_scramble_glyphs
wchar_t k_terminal_scramble_glyphs[24] = {
    0xe033, 0xe034, 0xe035, 0xe036, 0xe037, 0xe038, 0xe039, 0xe040,
    0xe041, 0xe042, 0xe043, 0xe044, 0xe045, 0xe046, 0xe047, 0xe048,
    0xe049, 0xe050, 0xe051, 0xe052, 0xe053, 0xe054, 0xe055, 0,
};
// @address 0x7c8860 k_terminal_translate_threshold_max
float k_terminal_translate_threshold_max = 0.9997f;
// @address 0x119a55c k_terminal_translate_threshold_min
float k_terminal_translate_threshold_min;


static __forceinline long gui_terminal_impulse_sound_new(long sound_index, float scale)
{
    s_game_sound_impulse_internal_parameters parameters;
    parameters.additional_override_flags = 0;
    parameters.source.volume_type = 0;
    parameters.source.flags = 0;
    parameters.source.spatialization = 0;
    parameters.source.volume = scale;
    parameters.source.name = 0;
    parameters.object_index = -1;
    parameters.playback_label = -1;
    parameters.tracking_data = 0;
    parameters.tracking = 0;
    parameters.permutation = 0;
    return game_sound_impulse_new_internal(sound_index, &parameters);
}

static __forceinline s_terminal_datum *gui_terminal_object_get(long terminal_index)
{
    return (s_terminal_datum *)object_get(terminal_index);
}

static __forceinline unsigned short gui_terminal_get_screen_state(long terminal_index)
{
    return gui_terminal_object_get(terminal_index)->screen_state;
}

static __forceinline s_terminal_definition *gui_terminal_definition_get(long definition_index)
{
    return (s_terminal_definition *)tag_get('term', definition_index);
}

// @rdata 0x82026484
static const float k_gui_screen_terminals_exit_fade_seconds = 1.5f;
// @rdata 0x82032900
static const float k_gui_screen_terminals_exit_seconds = 20.0f;

// @address 0x4964c0 gui_screen_terminal_update_translated_text
void gui_screen_terminal_update_translated_text(c_gui_screen_terminal *terminal, float progress)
{
    c_static_wchar_string<1024> display_text(terminal->m_text.get_string());
    wchar_t *translated = terminal->m_translated_text.get_buffer();
    if (progress == 0.0f)
    {
        terminal->m_translated_text.set(terminal->m_text.get_string());

        unsigned long protected_characters[BIT_VECTOR_SIZE_IN_LONGS(1024)];
        memset(protected_characters, 0, BIT_VECTOR_SIZE_IN_BYTES(1024));
        const wchar_t *tag_starts[2] = { L"<color", L"</color>" };
        const wchar_t *tag_ends[2] = { L">", L"" };
        for (short tag_index = 0; tag_index < NUMBEROF(tag_starts); tag_index++)
        {
            const wchar_t *search = terminal->m_translated_text.get_string();
            while (translated && search)
            {
                const wchar_t *tag_start = wcsstr(search, tag_starts[tag_index]);
                if (!tag_start)
                    break;
                short first = (short)(tag_start - translated);
                short last = (short)(first + wcslen(tag_starts[tag_index]) - 1);
                if (wcslen(tag_ends[tag_index]) != 0)
                {
                    const wchar_t *tag_end = wcsstr(translated + last + 1, tag_ends[tag_index]);
                    if (!tag_end)
                        break;
                    last = (short)(tag_end - translated);
                }
                if (first >= last)
                    break;
                for (short character_index = first; character_index <= last; character_index++)
                    BIT_VECTOR_SET_FLAG(protected_characters, character_index, true);
                search = translated + last + 1;
                if (!search)
                    break;
            }
        }

        long length = terminal->m_translated_text.length();
        for (short character_index = 0; character_index < length; character_index++)
        {
            if (translated[character_index] != L' ')
            {
                bool translated_character = false;
                if (translated[character_index] == L'|')
                {
                    character_index++;
                }
                else if (!BIT_VECTOR_TEST_FLAG(protected_characters, character_index) && translated[character_index] >= L'A' &&
                    translated[character_index] <= L'z')
                {
                    while (!translated_character)
                    {
                        short glyph_index = (short)((float)rand() / 32767.0f * 23.0f);
                        if (VALID_INDEX(glyph_index, 23u))
                        {
                            translated_character = true;
                            translated[character_index] = k_terminal_scramble_glyphs[glyph_index];
                        }
                    }
                }
            }
        }
    }

    long display_length = display_text.length();
    if (display_length > 0)
    {
        float width = 250.0f / (float)display_length;
        float offset = (1.0f + width) * progress - width;
        for (short character_index = 0; character_index < display_length; character_index++)
        {
            float position = (float)character_index / (float)display_length;
            float random = (float)rand() / 32767.0f;
            float threshold = 1.0f - (position - offset) / width;
            threshold = PIN(threshold, k_terminal_translate_threshold_min, k_terminal_translate_threshold_max);

            s_terminal_datum *object = gui_terminal_object_get(terminal->m_terminal_index);
            if (object->screen_state == _terminal_screen_state_exiting &&
                gui_terminal_get_screen_state(terminal->m_terminal_index) == _terminal_screen_state_exiting && !object->screen_flags.test(3))
            {
                long ticks_remaining = game_seconds_to_ticks_round(k_gui_screen_terminals_exit_seconds) - object->screen_state_ticks;
                float seconds_remaining;
                if ((float)ticks_remaining >= 0.0f)
                    seconds_remaining = tls_game_time_globals()->tick_length * (float)ticks_remaining;
                else
                    seconds_remaining = 0.0f;
                if (seconds_remaining < k_gui_screen_terminals_exit_fade_seconds)
                {
                    if (!terminal->m_exit_requested)
                    {
                        s_terminal_definition *definition = gui_terminal_definition_get(object->definition_index);
                        if (definition->exit_sound.index != -1)
                            gui_terminal_impulse_sound_new(definition->exit_sound.index, 1.0f);
                        terminal->m_exit_requested = true;
                    }
                    float fade = (k_gui_screen_terminals_exit_fade_seconds - seconds_remaining) / 1.5f;
                    float fade_limit = 1.0f - fade * fade;
                    if (threshold > fade_limit)
                        threshold = fade_limit;
                }
            }

            if (random > threshold)
                display_text.get_buffer()[character_index] = translated[character_index];
        }
    }

    c_gui_text_widget *text_widget = terminal_get_child_text_widget(terminal, 0x104ce);
    if (text_widget)
        text_widget->set_text(display_text.get_string());
}

extern "C" bool gui_screen_terminal_has_string(c_gui_screen_terminal *terminal, short string_index)
{
    bool result = false;
    if (string_index >= 0) {
        long state = terminal->m_state;
        if (string_index < k_gui_screen_terminals_string_counts[state]) {
            game_globals_storage *game_globals = tls_game_globals();
            s_terminal_definition *definition = (s_terminal_definition *)tag_get('term', object_get(terminal->m_terminal_index)->definition_index);
            long campaign_difficulty;
            if (game_globals->options.game_mode == 1) {
                campaign_difficulty = game_globals->options.campaign_difficulty;
            } else {
                campaign_difficulty = 1;
            }
            short difficulty = (short)campaign_difficulty;
            if (difficulty >= 0 && difficulty < 4) {
                s_terminal_difficulty_definition *entry = &definition->displays[difficulty];
                long string_list;
                switch (state) {
                case 1:
                    string_list = entry->string_list_a.index;
                    break;
                case 3:
                    string_list = entry->string_list_b.index;
                    break;
                default:
                    string_list = -1;
                    break;
                }
                if (string_list != -1) {
                    if (multilingual_unicode_string_list_test_string(string_list, screen_state_names[state][string_index])) {
                        result = true;
                    }
                }
            }
        }
    }
    return result;
}

// @address 0x497108 gui_screen_terminal_set_selected_string
extern "C" int gui_screen_terminal_set_selected_string(c_gui_screen_terminal *terminal, short selected_index)
{
    int result = 0;
    game_globals_storage *game_globals;
    s_terminal_definition *definition;
    long campaign_difficulty;
    short difficulty;
    s_terminal_difficulty_definition *entry;
    long string_list;
    long string_id;
    int text_id;
    c_static_wchar_string_1024 *text;
    c_gui_widget *child;
    c_gui_widget *widget_a;
    c_gui_widget *widget_b;
    bool visible_a;
    bool visible_b;

    do
    {
        if (selected_index < 0) break;
        if (selected_index >= k_gui_screen_terminals_string_counts[terminal->m_state]) break;
        game_globals = tls_game_globals();
        definition = (s_terminal_definition *)tag_get('term', object_get(terminal->m_terminal_index)->definition_index);
        if (game_globals->options.game_mode == 1) {
            campaign_difficulty = game_globals->options.campaign_difficulty;
        } else {
            campaign_difficulty = 1;
        }
        difficulty = (short)campaign_difficulty;
        if (difficulty < 0 || difficulty >= 4) break;
        entry = &definition->displays[difficulty];
        switch (terminal->m_state) {
        case 1:
            string_list = entry->string_list_a.index;
            break;
        case 2:
            string_list = entry->string_list_c.index;
            break;
        case 3:
            string_list = entry->string_list_b.index;
            break;
        default:
            string_list = -1;
            break;
        }
        if (string_list == -1) break;
        string_id = screen_state_names[terminal->m_state][selected_index];
        if (!multilingual_unicode_string_list_test_string(string_list, string_id)) break;
        text = &terminal->m_text;
        if (!text_group_get_string(string_list, string_id, text)) break;
        if (terminal->m_state == 2) {
            text_id = 0x104ee;
        } else {
            text_id = 0x104ce;
        }
        child = ((c_gui_widget *)terminal)->get_child_widget(_gui_text_widget, text_id);
        if (child == 0) break;
        if (terminal->m_state == 3 || terminal->m_state == 1) {
            long sound_index = definition->page_sound.index;
            if (sound_index != -1) {
                unspatialized_impulse_sound_new(sound_index, 1.0f);
            }
        }
        ((c_gui_text_widget *)child)->set_text(text->get_string());
        result = 1;
        terminal->m_selected_index = selected_index;
        terminal->m_text_ticks = 0;
    } while (false);
    widget_a = ((c_gui_widget *)terminal)->get_child_widget(_gui_bitmap_widget, 0x104dc);
    widget_b = ((c_gui_widget *)terminal)->get_child_widget(_gui_bitmap_widget, 0x104dd);
    visible_a = false;
    visible_b = false;
    if (terminal->m_state == 1) {
        visible_a = true;
    }
    else if (terminal->m_state == 3) {
        if (gui_screen_terminal_has_string(terminal, (short)(terminal->m_selected_index + 1))) {
            visible_a = true;
        } else {
            visible_b = true;
        }
    }
    if (widget_a != 0) {
        widget_a->set_visible(visible_a);
    }
    if (widget_b != 0) {
        widget_b->set_visible(visible_b);
    }
    return result;
}

// @address 0x72fdf8 k_terminal_map_image_names
const char *k_terminal_map_image_names[8] = {
    "d:\\maps\\images\\term_1.blf",
    "d:\\maps\\images\\term_2.blf",
    "d:\\maps\\images\\term_3.blf",
    "d:\\maps\\images\\term_4.blf",
    "d:\\maps\\images\\term_5.blf",
    "d:\\maps\\images\\term_6.blf",
    "d:\\maps\\images\\term_7.blf",
    "d:\\maps\\images\\term_8.blf",
};

// @rdata 0x82022dc0
static const float k_gui_screen_terminals_rounding_bias = 0.5f;

// @address 0x496c30 gui_screen_terminal_set_state
void gui_screen_terminal_set_state(c_gui_screen_terminal *terminal, long state)
{
    short i;
    s_terminal_difficulty_definition *display;
    game_globals_storage *game_globals;
    s_terminal_definition *definition;
    long campaign_difficulty;
    short difficulty;
    bool show_text;
    bool show_progress;
    bool show_icons;
    c_gui_widget *child;
    unsigned long icon_flags;
    short icon_mask;
    long unmasked;
    c_gui_custom_bitmap_widget *image_widget;
    short image_index;
    unsigned short image_id;

    if (terminal->m_state == state)
        return;
    display = 0;
    game_globals = tls_game_globals();
    // match: 0x496c30: in-place read holds objhdr in r8, globals in r11; object_get swaps to r10/r8, sinks lwz/cmpwi
    definition = (s_terminal_definition *)global_tag_instances[
        DATUM_INDEX_TO_ABSOLUTE_INDEX(((s_object_header *)object_header_data->data)[
            DATUM_INDEX_TO_ABSOLUTE_INDEX(terminal->m_terminal_index)].datum->definition_index)].definition;
    if (game_globals->options.game_mode == 1)
        campaign_difficulty = game_globals->options.campaign_difficulty;
    else
        campaign_difficulty = 1;
    difficulty = (short)campaign_difficulty;
    if (difficulty >= 0 && difficulty < 4)
        display = &definition->displays[difficulty];
    terminal->m_state = state;
    terminal->m_selected_index = 0;
    terminal->m_state_ticks = 0;
    gui_screen_terminal_set_selected_string(terminal, 0);
    show_text = true;
    show_progress = false;
    show_icons = false;
    switch (state) {
    case 2:
        terminal->m_page_complete = 0;
        show_progress = true;
        terminal->m_page_index = 0;
        terminal->m_elapsed_pending = 1;
        terminal->m_page_reset_sentinel = -1;
        break;
    case 1:
    case 3:
        show_icons = true;
        terminal->m_countdown = (short)(long)((float)tls_game_time_globals()->ticks_per_second + k_gui_screen_terminals_rounding_bias);
        // fall through
    case 0:
        show_text = false;
        break;
    default:
        // match: 0x496c30: unreachable hint omits dead branch check
        __assume(0);
    }
    child = ((c_gui_widget *)terminal)->get_child_widget(_gui_text_widget, 0x104ce);
    child->set_visible(show_text);
    if (state != 2) {
        child = ((c_gui_widget *)terminal)->get_child_widget(_gui_bitmap_widget, 0x104db);
        icon_flags = 0;
        if (show_icons && display) {
            if (state == 1)
                icon_flags = display->visible_images_a;
            else
                icon_flags = display->visible_images_b;
        }
        if (child)
            child->set_visible(show_icons);
        for (i = 0; i < 3; i++) {
            child = ((c_gui_widget *)terminal)->get_child_widget(_gui_group_widget, k_gui_screen_terminals_icon_names_a[i]);
            if (child)
                child->set_visible(((1 << i) & (short)icon_flags) != 0);
        }
        unmasked = (short)icon_flags;
        if (state != 1)
            unmasked = icon_flags & 0x3f8;
        icon_mask = (short)unmasked;
        for (i = 0; i < 10; i++) {
            child = ((c_gui_widget *)terminal)->get_child_widget(_gui_bitmap_widget, k_gui_screen_terminals_icon_names_b[i]);
            if (child)
                child->set_visible(((1 << i) & icon_mask) != 0);
        }
    }
    if (show_progress) {
        child = ((c_gui_widget *)terminal)->get_child_widget(_gui_group_widget, 0x104fc);
        if (child)
            child->set_visible(true);
    } else {
        for (i = 0; i < 5; i++) {
            child = ((c_gui_widget *)terminal)->get_child_widget(_gui_group_widget, k_gui_screen_terminals_progress_names[i]);
            if (child)
                child->set_visible(false);
        }
    }
    if (!display)
        return;
    child = ((c_gui_widget *)terminal)->get_child_widget(_gui_bitmap_widget, _terminals_string_images);
    if (!child)
        return;
    image_widget = (c_gui_custom_bitmap_widget *)child;
    image_index = -1;
    if (state == 1) {
        image_id = display->image_a;
        if (image_id == 0xffff)
            child->set_visible(false);
        else
            image_index = (short)image_id;
    } else if (state == 3) {
        image_id = display->image_b;
        if (image_id == 0xffff)
            child->set_visible(false);
        else
            image_index = (short)image_id;
    } else if (state != 2) {
        child->set_visible(false);
    }
    if (image_index >= 0 && image_index < 8) {
        strncpy(image_widget->m_map_image, k_terminal_map_image_names[image_index], 0x100);
        image_widget->m_loading = false;
        image_widget->m_task_id = 0;
        child->set_visible(true);
    }
}

bool c_gui_screen_terminal::handle_controller_input_message(const c_controller_input_message *message)
{
    bool handled = false;
    if (m_state != 2 && message->get_event_type() == 13)
    {
        if (message->get_component() != 0)
        {
            if (message->get_component() == 2)
            {
                gui_screen_terminal_set_selected_string(this, m_selected_index - 1);
                handled = true;
            }
        }
        else
        {
            gui_screen_terminal_set_selected_string(this, m_selected_index + 1);
            handled = true;
        }
    }
    if (!handled) {
        c_gui_screen_widget::handle_controller_input_message(message);
    }
    return handled;
}

void c_load_start_menu_pane_message::apply_initial_state(c_gui_screen_widget *screen_widget) const
{
    c_start_menu_pane_screen_widget *pane = static_cast<c_start_menu_pane_screen_widget *>(screen_widget);
    float rotation = m_rotation;
    float translation = m_translation;
    pane->m_owner_player_identifier = m_player_id;
    pane->m_target_player_identifier = m_target_player_id;
    pane->m_target_xuid = m_target_player_xuid;
    pane->m_service_record_identity = m_service_record_identity;
    pane->m_translation = translation;
    pane->m_rotation = rotation;
}

void c_gui_screen_terminal::update(unsigned long current_milliseconds)
{
    if (m_initializing)
    {
        gui_screen_terminal_set_state(this, 0);
        m_initializing = 0;
        m_exit_requested = 0;
        c_gui_widget *widget = this->get_child_widget(_gui_bitmap_widget, 0x104db);
        if (widget != 0)
            widget->set_visible(true);
    }

    switch (m_state)
    {
    case 1:
    case 3:
    {
        short countdown = m_countdown;
        if (countdown > 0)
        {
            countdown = (short)(countdown - 1);
            m_countdown = countdown;
            if (countdown == 0)
            {
                c_gui_widget *widget = this->get_child_widget(_gui_text_widget, 0x104ce);
                widget->set_visible(true);
            }
        }
        if (m_countdown <= 0)
        {
            float fraction = (float)m_text_ticks / (float)game_seconds_to_ticks_round(2.0f);
            fraction = PIN(fraction, 0.0f, 1.0f);
            gui_screen_terminal_update_translated_text(this, fraction);
        }
        break;
    }
    case 2:
        if (m_elapsed_pending)
        {
            m_text_start_time = current_milliseconds;
            m_elapsed_pending = 0;
        }
        {
            unsigned long elapsed = current_milliseconds - m_text_start_time > 0x7fffffff
                ? 0x7fffffff : current_milliseconds - m_text_start_time;
            gui_screen_terminal_update_paged_text(this, elapsed);
        }
        break;
    }

    if (m_state_ticks < 0x7fff)
        m_state_ticks++;
    if (m_countdown <= 0 && m_text_ticks < 0x7fff)
        m_text_ticks++;

    c_gui_screen_widget::update(current_milliseconds);

    if (!players_globals->suppressed)
    {
        transition_out_with_transition_type(0, _screen_transition_normal);
    }
}


