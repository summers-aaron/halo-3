// blamlibxbox/retail_minidump_info_provider.obj: from the records 0x2aa098 0x2aa108 0x2aa178 0x2aa1e0 0x2aa2e8 0x2aa320 0x2aa398 0x2aa500 0x2aa518 0x2aa528 0x2aa5c0 0x2aa678 0x2aa718 0x2aa758 0x2aa7a8 0x2aa7b0
// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0x589ef0 SuspendThread
// @address 0x6f2d98 xenon_minidump_enumerate_thread_handles
// @address 0x6f2e60 xenon_minidump_get_thread_handle
// @address 0x6f2eb0 xenon_minidump_get_thread_information
// @address 0x6f2f68 xenon_minidump_get_thread_context
// @address 0x6f30a8 xenon_minidump_get_loaded_module_count
// @address 0x6f3108 xenon_minidump_get_module_info
// @address 0x6f3200 tag_resource_cache_module_section_count
// @address 0x6f3250 tag_resource_cache_module_get_section
// @address 0x6f32f0 tag_resource_cache_next_readable_memory_range

#include <xtl.h>
#include <string.h>
#include "cseries/cseries.h"
#include "cseries/minidump_writer.h"
#include "cseries/minidump_thread_types.h"
#include "cseries/retail_minidump_info_provider.h"
#include "cseries/xenon_minidump.h"
#include "memory/static_arrays.h"
#include "simulation/simulation.h"

struct s_thread_handle_list
{
    unsigned int count;
    void *handles[63];
};

struct s_readable_memory_range
{
    unsigned long address;
    unsigned long size;
    unsigned long protection;
};

struct s_module_section
{
    unsigned long address;
    unsigned long size;
    // The module-section query copies the eight-byte PE section name at offset 8.
    char section_name[264];
};

static __forceinline bool address_is_user_memory(unsigned long address)
{
    bool result = false;
    if (address < 0x7FFFFFFF)
        result = true;
    return result;
}

// @address 0x42d380 ?minidump_begin@c_retail_minidump_info_provider@@UAA_NPAU_EXCEPTION_POINTERS@@@Z
bool c_retail_minidump_info_provider::minidump_begin(_EXCEPTION_POINTERS *)
{
    return true;
}

// @address 0x5bec98 ?minidump_end@c_retail_minidump_info_provider@@UAAXXZ
void c_retail_minidump_info_provider::minidump_end()
{
}

// @address 0x2aa098 ?suspend_non_calling_threads@c_retail_minidump_info_provider@@UAAXXZ
void c_retail_minidump_info_provider::suspend_non_calling_threads()
{
    unsigned int i;
    s_thread_handle_list list;
    xenon_minidump_enumerate_thread_handles(2, &list);
    i = 0;
    if (list.count != 0) {
        do {
            SuspendThread(list.handles[i]);
            CloseHandle(list.handles[i]);
            i++;
        } while (i < list.count);
    }
}

// @address 0x2aa108 ?resume_non_calling_threads@c_retail_minidump_info_provider@@UAAXXZ
void c_retail_minidump_info_provider::resume_non_calling_threads()
{
    unsigned int i;
    s_thread_handle_list list;
    xenon_minidump_enumerate_thread_handles(2, &list);
    i = 0;
    if (list.count != 0) {
        do {
            ResumeThread(list.handles[i]);
            CloseHandle(list.handles[i]);
            i++;
        } while (i < list.count);
    }
}

// @address 0x5bec98 ?begin_thread_dump@c_retail_minidump_info_provider@@UAAXXZ
void c_retail_minidump_info_provider::begin_thread_dump()
{
}

// @address 0x5bec98 ?end_thread_dump@c_retail_minidump_info_provider@@UAAXXZ
void c_retail_minidump_info_provider::end_thread_dump()
{
}

// @address 0x2aa178 ?get_thread_id_array@c_retail_minidump_info_provider@@UAAXPAKK0@Z
void c_retail_minidump_info_provider::get_thread_id_array(unsigned long *ids, unsigned long maximum, unsigned long *count)
{
    unsigned int thread_count;
    s_thread_handle_list list;
    xenon_minidump_enumerate_thread_handles(1, &list);
    if (list.count > maximum) {
        thread_count = maximum;
        list.count = thread_count;
    }
    thread_count = list.count;
    if (count) {
        *count = thread_count;
    }
    if (ids) {
        memcpy(ids, list.handles, thread_count * sizeof(*ids));
    }
}

// @address 0x2aa1e0 ?get_thread_information@c_retail_minidump_info_provider@@UAAXKPAUs_thread_information@@@Z
void c_retail_minidump_info_provider::get_thread_information(unsigned long id, s_thread_information *info)
{
    if (info) {
        void *thread = xenon_minidump_get_thread_handle(id);
        s_xenon_minidump_thread_information information;
        xenon_minidump_get_thread_information(thread, &information);
        info->context.context_flags = 23;
        xenon_minidump_get_thread_context(thread, &info->context);
#ifdef HALO3_64BIT
        __int64 position = (__int64)info->context.gpr[1];
#else
        __int64 position = (int)info->context.gpr[1];
#endif
        int suspend_count = information.suspend_count;
        info->priority = information.priority;
        __int64 begin = information.stack_limit;
        info->priority_class = 0;
        __int64 end = information.stack_base;
        info->suspend_count = suspend_count;
        info->teb = information.tls_base;
        info->context_data_size = 2624;
        info->context_rva = 0;
        info->thread_id = id;
#ifdef HALO3_64BIT
        __int64 stack_base_addr;
        __int64 stack_size_bytes;
        if (position >= begin && position < end && position - (__int64)information.stack_slack_space >= begin) {
            stack_base_addr = position - (__int64)information.stack_slack_space;
            stack_size_bytes = (end - position) + (__int64)information.stack_slack_space;
        } else {
            stack_base_addr = begin;
            stack_size_bytes = end - begin;
        }
        info->stack.base = (int)stack_base_addr;
        info->stack.size = (int)stack_size_bytes;
        info->stack_rva = 0;
        info->stack_data_size = (unsigned long)stack_size_bytes;
        info->stack_start = (unsigned __int64)stack_base_addr;
#else
        if (position >= begin && position < end && position - information.stack_slack_space >= begin) {
            info->stack.base = (int)position - information.stack_slack_space;
            info->stack.size = ((__int64)(int)end - (int)position) + information.stack_slack_space;
        } else {
            info->stack.base = (int)begin;
            info->stack.size = (__int64)(int)end - (int)begin;
        }
        info->stack_rva = 0;
        info->stack_data_size = info->stack.size;
        info->stack_start = (int)info->stack.base;
#endif
        CloseHandle(thread);
    }
}

// @address 0x2aa2e8 ?get_loaded_module_count@c_retail_minidump_info_provider@@UAAXPAK@Z
void c_retail_minidump_info_provider::get_loaded_module_count(unsigned long *count)
{
    if (count)
        *count = static_cast<unsigned long>(xenon_minidump_get_loaded_module_count());
}

// @address 0x2aa320 ?get_loaded_module@c_retail_minidump_info_provider@@UAAXKPAU_MINIDUMP_MODULE@@@Z
void c_retail_minidump_info_provider::get_loaded_module(unsigned long index, _MINIDUMP_MODULE *module)
{
    s_xenon_minidump_module_info info = {0};
    if (module && xenon_minidump_get_module_info(index, &info) != 0) {
        module->module_name_rva = 0;
        module->base_of_image = (__int64)(int)info.image_base;
        module->size_of_image = info.image_size;
        module->checksum = info.checksum;
        module->time_date_stamp = info.timestamp;
    }
}

static __forceinline void replace_extension(char *extension, const char *replacement)
{
    if (extension && replacement)
    {
        strncpy(extension, replacement, strlen(replacement) + 1);
    }
}

// @address 0x2aa398 ?get_loaded_module_name@c_retail_minidump_info_provider@@UAAXKPADK@Z
void c_retail_minidump_info_provider::get_loaded_module_name(unsigned long index, char *name, unsigned long size)
{
    if (name)
    {
        s_xenon_minidump_module_info info;
        xenon_minidump_get_module_info(index, &info);
        strncpy(name, info.filename, size);
        char *extension = 0;
        const char *replacement = 0;
        if (info.flags & 1)
        {
            extension = strstr(name, ".xex");
            replacement = ".dll";
        }
        else if (strstr(name, "xboxkrnl"))
        {
            extension = strstr(name, ".exe");
            if (extension)
                replacement = "d.exe";
        }
        else
        {
            c_static_string<260> executable;
            executable.m_string[0] = 0;
            executable.print("halo3");
            static_string_append_bounded(executable.m_string, "_");
            static_string_append_bounded(executable.m_string, "cache");
            static_string_append_bounded(executable.m_string, "_");
            static_string_append_bounded(executable.m_string, "release");
            static_string_append_bounded(executable.m_string, ".exe");
            strncpy(name, executable.m_string, size);
            name[size - 1] = 0;
        }
        replace_extension(extension, replacement);
    }
}

// @address 0x5bec98 ?update_exception_information@c_retail_minidump_info_provider@@UAAXPAUMINIDUMP_EXCEPTION_STREAM@@@Z
void c_retail_minidump_info_provider::update_exception_information(MINIDUMP_EXCEPTION_STREAM *)
{
}

// @address 0x2aa500 ?start_committed_region_query@c_retail_minidump_info_provider@@UAA_NXZ
bool c_retail_minidump_info_provider::start_committed_region_query()
{
    m_committed_region_address = 0;
    return true;
}

// @address 0x2aa518 ?end_committed_region_query@c_retail_minidump_info_provider@@UAAXXZ
void c_retail_minidump_info_provider::end_committed_region_query()
{
    m_committed_region_address = 0;
}

// @address 0x2aa528 ?get_committed_region_count@c_retail_minidump_info_provider@@UAAXPAK@Z
void c_retail_minidump_info_provider::get_committed_region_count(unsigned long *count)
{
    if (count == 0)
        return;
    *count = 0;
    s_readable_memory_range range;
    unsigned int handle = tag_resource_cache_next_readable_memory_range(0, &range);
    while (handle != 0)
    {
        if (address_is_user_memory(range.address))
            ++(*count);
        handle = tag_resource_cache_next_readable_memory_range(handle, &range);
    }
}

// @address 0x2aa5c0 ?get_next_committed_region_info@c_retail_minidump_info_provider@@UAA_NPAK0@Z
bool c_retail_minidump_info_provider::get_next_committed_region_info(unsigned long *base, unsigned long *size)
{
    bool found = false;
    if (base && size)
    {
        s_readable_memory_range range;
        unsigned int handle = tag_resource_cache_next_readable_memory_range(m_committed_region_address, &range);
        while (handle != 0 && !found)
        {
            bool in_range = address_is_user_memory(range.address);
            if (in_range)
            {
                found = true;
                *base = range.address;
                *size = range.size;
            }
            else
            {
                handle = tag_resource_cache_next_readable_memory_range(handle, &range);
            }
        }
        m_committed_region_address = handle;
    }
    return found;
}

// @address 0x42d380 ?should_resolve_memory@c_retail_minidump_info_provider@@UAA_NXZ
bool c_retail_minidump_info_provider::should_resolve_memory()
{
    return true;
}

// @address 0x2aa678 ?resolve_memory@c_retail_minidump_info_provider@@UAA_NPBXKPAXPAK@Z
bool c_retail_minidump_info_provider::resolve_memory(const void *source, unsigned long size, void *destination, unsigned long *resolved_size)
{
    bool result = false;
    if (source && destination && resolved_size && size)
    {
        if (((const char *)source + size <= (const char *)destination || (const char *)destination + size <= (const char *)source)
            && !minidump_test_memory_readable(source, size))
        {
            memcpy(destination, source, size);
            *resolved_size = size;
            result = true;
        }
    }
    return result;
}

// @address 0x2aa758 ?start_module_section_memory_query@c_retail_minidump_info_provider@@UAA_NK@Z
bool c_retail_minidump_info_provider::start_module_section_memory_query(unsigned long module)
{
    memset(&m_sections, 0, sizeof(m_sections));
    m_sections.module = module;
    m_sections.count = tag_resource_cache_module_section_count(module);
    return true;
}

// @address 0x2aa7a8 ?end_module_section_memory_query@c_retail_minidump_info_provider@@UAAXXZ
void c_retail_minidump_info_provider::end_module_section_memory_query()
{
    csmemset(&m_sections, sizeof(m_sections), 0);
}

// @address 0x2aa718 ?get_loaded_module_section_count@c_retail_minidump_info_provider@@UAAXKPAK@Z
void c_retail_minidump_info_provider::get_loaded_module_section_count(unsigned long module, unsigned long *count)
{
    if (count != 0)
    {
        *count = tag_resource_cache_module_section_count(module);
    }
}

// @address 0x2aa7b0 ?get_next_module_section_memory_info@c_retail_minidump_info_provider@@UAA_NPAK0@Z
bool c_retail_minidump_info_provider::get_next_module_section_memory_info(unsigned long *base, unsigned long *size)
{
    bool result = false;
    if (base && size && m_sections.index < m_sections.count)
    {
        s_module_section section;
        if (tag_resource_cache_module_get_section(m_sections.module, m_sections.index, &section))
        {
            *base = section.address;
            *size = section.size;
            ++m_sections.index;
            result = true;
        }
    }
    return result;
}
