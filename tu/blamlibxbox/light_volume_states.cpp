// blamlibxbox/light_volume_states.obj: from the records 0x300598 0x300688
// @compiler xdk6130
// @flags /TP /O2 /Oc

#include "tag_files/tag_types.h"
#include "memory/datum_index.h"
#include "tag_files/tag_groups.h"
#include "memory/tls_globals.h"
#include "effects/light_volume_states.h"
#include "effects/light_volume_types.h"

// @address 0x300688 ?set_light_volume_system@c_light_volume_states@@QAAXPBVc_light_volume_system@@@Z
void c_light_volume_states::set_light_volume_system(const c_light_volume_system *system)
{
    const c_light_volume_system *old = m_system;
    if (system == old)
        return;
    unsigned long old_flags = 0;
    const cache_file_tag_instance *instances = global_tag_instances;
    if (old)
    {
        s_light_volume_system_definition *definition =
            (s_light_volume_system_definition *)instances[datum_index_to_absolute_index(old->m_definition_index)].definition;
        int count = definition->light_volumes.count;
        if (count > 0)
        {
            s_light_volume_definition *entry = (s_light_volume_definition *)definition->light_volumes.address;
            do
            {
                old_flags |= entry->flags;
                entry++;
            } while (--count != 0U);
        }
    }
    unsigned long kept = m_flags & ~old_flags;
    m_system = system;
    unsigned long new_flags = 0;
    m_flags = kept;
    m_dirty = m_dirty & ~8 & ~0x700;
    if (system)
    {
        s_light_volume_system_definition *definition =
            (s_light_volume_system_definition *)instances[datum_index_to_absolute_index(system->m_definition_index)].definition;
        int count = definition->light_volumes.count;
        if (count > 0)
        {
            unsigned long *flags = &((s_light_volume_definition *)definition->light_volumes.address)->flags;
            do
            {
                new_flags |= *flags;
                flags += sizeof(s_light_volume_definition) / sizeof(unsigned long);
            } while (--count != 0U);
        }
    }
    m_flags |= new_flags;
    update_states(3980);
}

// @address 0x300598 ?update_states@c_light_volume_states@@QAAXK@Z
void c_light_volume_states::update_states(unsigned long mask)
{
    unsigned long pending = m_flags & ~m_dirty & mask;
    if (pending != 0)
    {
        if ((pending & 96) != 0)
        {
            const c_light_volume *volume = m_light_volume;
            m_values[5] = volume->m_random_values[0];
            m_values[6] = volume->m_random_values[1];
            m_values[7] = volume->m_offset;
        }

        if ((pending & 0x708) != 0)
        {
            const c_light_volume_system *system = m_system;
            m_values[3] = system->m_age;
            m_values[8] = (float)(double)system->m_lod;
            m_values[9] = system->m_effect_scale_a;
            m_values[10] = system->m_effect_scale_b;
        }

        if ((pending & 4) != 0)
        {
            game_time_globals_definition *globals = tls_game_time_globals();
            m_values[2] = (float)(double)(globals->time % globals->ticks_per_second) * globals->tick_length;
        }
    }

    m_dirty = m_dirty | pending;
}
