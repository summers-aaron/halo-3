#ifndef CSERIES_RETAIL_MINIDUMP_INFO_PROVIDER_H
#define CSERIES_RETAIL_MINIDUMP_INFO_PROVIDER_H

#include "cseries/minidump_provider_common.h"

class c_retail_minidump_info_provider : public c_minidump_provider_common
{
public:
    virtual bool minidump_begin(_EXCEPTION_POINTERS *exception);
    virtual void minidump_end();
    virtual void suspend_non_calling_threads();
    virtual void resume_non_calling_threads();
    virtual void begin_thread_dump();
    virtual void end_thread_dump();
    virtual void get_thread_id_array(unsigned long *ids, unsigned long maximum, unsigned long *count);
    virtual void get_thread_information(unsigned long id, s_thread_information *info);
    virtual void get_loaded_module_count(unsigned long *count);
    virtual void get_loaded_module(unsigned long index, _MINIDUMP_MODULE *module);
    virtual void get_loaded_module_name(unsigned long index, char *name, unsigned long size);
    virtual void update_exception_information(MINIDUMP_EXCEPTION_STREAM *stream);
    virtual bool start_committed_region_query();
    virtual void end_committed_region_query();
    virtual void get_committed_region_count(unsigned long *count);
    virtual bool get_next_committed_region_info(unsigned long *base, unsigned long *size);
    virtual bool should_resolve_memory();
    virtual bool resolve_memory(const void *source, unsigned long size, void *destination, unsigned long *resolved_size);
    virtual bool start_module_section_memory_query(unsigned long module);
    virtual void end_module_section_memory_query();
    virtual void get_loaded_module_section_count(unsigned long module, unsigned long *count);
    virtual bool get_next_module_section_memory_info(unsigned long *base, unsigned long *size);

private:
    struct s_module_section_query
    {
        long module;
        unsigned long index;
        unsigned long count;
    };

    unsigned long m_committed_region_address;
    s_module_section_query m_sections;
};

#endif // CSERIES_RETAIL_MINIDUMP_INFO_PROVIDER_H
