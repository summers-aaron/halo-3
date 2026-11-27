// blamlibxbox/xenon_minidump_endian.obj: from the records 0x2ff3d8 0x2ff4e8 0x2ff5e0 0x2ff720
// @flags /TP /O2 /Oc
#include <stdlib.h>
#include "cseries/xenon_minidump_endian.h"

struct s_minidump_module
{
    unsigned __int64 base_of_image;
    unsigned long size_of_image;
    unsigned long checksum;
    unsigned long time_date_stamp;
    unsigned long module_name_rva;
    unsigned long signature;
    unsigned long struct_version;
    unsigned long file_version_ms;
    unsigned long file_version_ls;
    unsigned long product_version_ms;
    unsigned long product_version_ls;
    unsigned long file_flags_mask;
    unsigned long file_flags;
    unsigned long file_os;
    unsigned long file_type;
    unsigned long file_subtype;
    unsigned long file_date_hi;
    unsigned long file_date_lo;
    s_minidump_location_descriptor cv_record;
    s_minidump_location_descriptor misc_record;
};

struct s_minidump_memory_descriptor
{
    unsigned __int64 start_of_memory_range;
    s_minidump_location_descriptor memory;
};

struct s_minidump_thread
{
    unsigned long thread_id;
    unsigned long suspend_count;
    unsigned long priority_class;
    unsigned long priority;
    unsigned __int64 teb;
    s_minidump_memory_descriptor stack;
    s_minidump_location_descriptor thread_context;
};

union u_xenon_minidump_endian_word_pair
{
    unsigned __int64 value;
    struct
    {
        unsigned int high;
        unsigned int low;
    } words;
};

static __forceinline unsigned __int64 xenon_minidump_endian_byte_swap64(unsigned __int64 value)
{
    u_xenon_minidump_endian_word_pair source;
    source.value = value;
    return ((unsigned __int64)_byteswap_ulong(source.words.low) << 32) | _byteswap_ulong(source.words.high);
}

// @address 0x2ff3d8 xenon_minidump_endian_swap_system_info
extern "C" void xenon_minidump_endian_swap_system_info(_MINIDUMP_SYSTEM_INFO *info)
{
    if (info != NULL)
    {
        info->cpu.other.processor_features[0] = xenon_minidump_endian_byte_swap64(info->cpu.other.processor_features[0]);
        info->cpu.other.processor_features[1] = xenon_minidump_endian_byte_swap64(info->cpu.other.processor_features[1]);
        info->processor_architecture = _byteswap_ushort(info->processor_architecture);
        info->processor_level = _byteswap_ushort(info->processor_level);
        info->processor_revision = _byteswap_ushort(info->processor_revision);
        info->build_number = _byteswap_ulong(info->build_number);
        info->csd_version_rva = _byteswap_ulong(info->csd_version_rva);
        info->major_version = _byteswap_ulong(info->major_version);
        info->minor_version = _byteswap_ulong(info->minor_version);
        info->platform_id = _byteswap_ulong(info->platform_id);
        info->suite_mask = _byteswap_ushort(info->suite_mask);
    }
}

// @address 0x2ff4e8 xenon_minidump_endian_swap_thread
extern "C" void xenon_minidump_endian_swap_thread(s_minidump_thread *thread)
{
    if (thread != NULL)
    {
        thread->priority = _byteswap_ulong(thread->priority);
        thread->priority_class = _byteswap_ulong(thread->priority_class);
        thread->stack.memory.data_size = _byteswap_ulong(thread->stack.memory.data_size);
        thread->stack.memory.rva = _byteswap_ulong(thread->stack.memory.rva);
        thread->stack.start_of_memory_range = xenon_minidump_endian_byte_swap64(thread->stack.start_of_memory_range);
        thread->suspend_count = _byteswap_ulong(thread->suspend_count);
        thread->teb = xenon_minidump_endian_byte_swap64(thread->teb);
        thread->thread_context.data_size = _byteswap_ulong(thread->thread_context.data_size);
        thread->thread_context.rva = _byteswap_ulong(thread->thread_context.rva);
        thread->thread_id = _byteswap_ulong(thread->thread_id);
    }
}


// @address 0x2ff5e0 xenon_minidump_endian_swap_exception_stream
extern "C" void xenon_minidump_endian_swap_exception_stream(MINIDUMP_EXCEPTION_STREAM *stream)
{
    if (stream != NULL)
    {
        stream->__alignment = _byteswap_ulong(stream->__alignment);
        stream->exception_record.exception_code = _byteswap_ulong(stream->exception_record.exception_code);
        stream->exception_record.exception_flags = _byteswap_ulong(stream->exception_record.exception_flags);
        stream->exception_record.exception_address = xenon_minidump_endian_byte_swap64(stream->exception_record.exception_address);
        for (unsigned long i = 0; i < 15; i++)
        {
            stream->exception_record.exception_information[i] = xenon_minidump_endian_byte_swap64(stream->exception_record.exception_information[i]);
        }
        stream->thread_id = _byteswap_ulong(stream->thread_id);
        stream->exception_record.number_parameters = _byteswap_ulong(stream->exception_record.number_parameters);
        stream->thread_context.data_size = _byteswap_ulong(stream->thread_context.data_size);
        stream->thread_context.rva = _byteswap_ulong(stream->thread_context.rva);
        stream->exception_record.exception_record = xenon_minidump_endian_byte_swap64(stream->exception_record.exception_record);
    }
}

// @address 0x2ff720 xenon_minidump_endian_swap_module
extern "C" void xenon_minidump_endian_swap_module(s_minidump_module *module)
{
    if (module != NULL)
    {
        module->base_of_image = xenon_minidump_endian_byte_swap64(module->base_of_image);
        module->checksum = _byteswap_ulong(module->checksum);
        module->cv_record.data_size = _byteswap_ulong(module->cv_record.data_size);
        module->cv_record.rva = _byteswap_ulong(module->cv_record.rva);
        module->misc_record.data_size = _byteswap_ulong(module->misc_record.data_size);
        module->misc_record.rva = _byteswap_ulong(module->misc_record.rva);
        module->module_name_rva = _byteswap_ulong(module->module_name_rva);
        module->size_of_image = _byteswap_ulong(module->size_of_image);
        module->time_date_stamp = _byteswap_ulong(module->time_date_stamp);
        module->file_date_lo = _byteswap_ulong(module->file_date_lo);
        module->file_date_hi = _byteswap_ulong(module->file_date_hi);
        module->file_flags = _byteswap_ulong(module->file_flags);
        module->file_flags_mask = _byteswap_ulong(module->file_flags_mask);
        module->file_os = _byteswap_ulong(module->file_os);
        module->file_subtype = _byteswap_ulong(module->file_subtype);
        module->file_type = _byteswap_ulong(module->file_type);
        module->file_version_ls = _byteswap_ulong(module->file_version_ls);
        module->file_version_ms = _byteswap_ulong(module->file_version_ms);
        module->product_version_ls = _byteswap_ulong(module->product_version_ls);
        module->product_version_ms = _byteswap_ulong(module->product_version_ms);
        module->signature = _byteswap_ulong(module->signature);
        module->struct_version = _byteswap_ulong(module->struct_version);
    }
}
