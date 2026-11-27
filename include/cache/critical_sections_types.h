#ifndef CACHE_CRITICAL_SECTIONS_TYPES_H
#define CACHE_CRITICAL_SECTIONS_TYPES_H

enum e_critical_sections
{
    section_none = -1,
    k_crit_section_resource_threading_access_rw_lock = 45,
    k_crit_section_simple_resource_cache_access_rw_lock = 46
};

#endif // CACHE_CRITICAL_SECTIONS_TYPES_H
