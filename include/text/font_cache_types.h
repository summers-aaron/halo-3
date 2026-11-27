#ifndef TEXT_FONT_CACHE_TYPES_H
#define TEXT_FONT_CACHE_TYPES_H

#include "text/font_id_types.h"
#include "text/character_data_index_types.h"
#include "interface/chud/utf32_types.h"
#include "text/font_cache.h"
#include "text/font_cache_lock.h"
#include "text/font_cache_thread_state_types.h"
#include "text/font_cache_globals_types.h"
#include "multithreading/synchronization.h"

struct s_font_character;

// c_font_cache owns a c_font_cache_scope_lock and defines the six
// virtual character operations while retaining that lock after the vfptr.
class c_font_cache_scope_lock {
public:
    c_font_cache_scope_lock()
    {
        m_state.active = 0;
        if (g_font_cache_globals == 0) {
            internal_critical_section_enter(37);
            m_state.active = 1;
            g_font_cache_globals = &g_internal_font_cache_globals;
        }
    }
    // match: 0x491af8: retain the direct font_cache_unlock call under inline_depth(0).
    // match: 0x492c20: keep the member cleanup inline under the same caller pragma.
    __forceinline ~c_font_cache_scope_lock() { font_cache_unlock(&m_state); }
private:
    s_font_cache_thread_state m_state;
};

class c_font_cache
{
public:
    c_font_cache();
    // match: 0x491af8: inline cleanup across the caller's inline_depth(0) region.
    // match: 0x492c20: retain inline vtable reset and scope-lock cleanup here too.
    __forceinline ~c_font_cache() {}
    virtual long character_data_get_hardware_block_index(e_character_data_index character_data_index);
    virtual void character_data_set_hardware_block_index(e_character_data_index character_data_index, long hardware_block_index);
    virtual void character_data_touch(e_character_data_index character_data_index);
    virtual bool get_pixels(e_font_id font, utf32 character, const s_font_character *font_character, unsigned short *pixels);
    virtual bool font_character_predict(e_font_id font, utf32 character);
    virtual bool get_character(
        e_font_id font, utf32 character, bool precache, unsigned long *out_key, long *out_index, const s_font_character **out_character);
private:
    c_font_cache_scope_lock m_lock;
};

#endif // TEXT_FONT_CACHE_TYPES_H
