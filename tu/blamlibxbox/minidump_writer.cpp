// blamlibxbox/minidump_writer.obj: from the records 0x2a7020 0x2a7788 0x2a78f0 0x2a7b80 0x2a7d08 0x2a7df8 0x2a7fd8 0x2a80b8 0x2a81b8 0x2a8310 0x2a99f8 0x2a9d18 0x2a9df8
// @flags /TP /O2 /Oc
#include <string.h>
#include <stdlib.h>
#include <crtdefs.h>
#include "cseries/crt_xdk.h"
#include "main/main.h"
#include "cseries/xenon_minidump_endian.h"
#include "cseries/minidump_writer.h"

struct _EXCEPTION_RECORD
{
    unsigned long ExceptionCode;
    unsigned long ExceptionFlags;
    _EXCEPTION_RECORD *ExceptionRecord;
    void *ExceptionAddress;
    unsigned long NumberParameters;
    unsigned long ExceptionInformation[15];
};

struct _EXCEPTION_POINTERS
{
    _EXCEPTION_RECORD *ExceptionRecord;
    void *ContextRecord;
};


struct s_thread_information;

#include "cseries/minidump_thread_types.h"


#include "cseries/minidump_directory_types.h"

struct s_minidump_writer
{
    c_minidump_file_output_interface *device;
    _EXCEPTION_POINTERS *exception_pointers;
    unsigned long thread_id;
    c_minidump_provider_interface *name_list;
    long stream_format;
    long byte_swap;
    void (*progress)(float percent);
    s_minidump_entry entries[8];
    s_minidump_memory_range memory_ranges[64];
    unsigned long entry_count;
    unsigned long context_rva;
    unsigned long memory_range_count;
};

struct s_minidump_thread;



#if defined(_MSC_VER) && !defined(__clang__)
// match: 0x2a7020: a hand-written PowerPC body.
// @address 0x2a7020 minidump_writer_get_caller_address
extern "C" __declspec(naked) int minidump_writer_get_caller_address()
{
    __asm {
        mflr r12
        stw r12, -8(r1)
        std r30, -24(r1)
        li r30, -4
        stwu r1, -112(r1)
        mflr r3
        addi r1, r1, 112
        add r3, r3, r30
        lwz r12, -8(r1)
        mtlr r12
        ld r30, -24(r1)
        blr
    }
}
#else
extern "C" int minidump_writer_get_caller_address()
{
    // The address of the call instruction: the return address less one instruction.
    return (int)(size_t)__builtin_return_address(0) - 4;
}
#endif

// @address 0x2a7788 minidump_writer_write_system_info
extern "C" bool minidump_writer_write_system_info(s_minidump_writer *writer)
{
    bool result = false;
    if (writer->device->is_valid() && writer->name_list != NULL)
    {
        _MINIDUMP_SYSTEM_INFO info;
        char csd_version[260];
        memset(&info, 0, sizeof(info));
        writer->name_list->get_system_information(&info);
        info.csd_version_rva = writer->device->tell() + 56;
        minidump_writer_add_directory_entry(writer, writer->device->tell(), sizeof(info), 7);
        if (writer->byte_swap == 0)
            xenon_minidump_endian_swap_system_info(&info);
        bool written = writer->device->write(&info, sizeof(info));
        writer->name_list->get_system_version_string(csd_version, sizeof(csd_version));
        result = written && minidump_writer_write_string(writer, csd_version);
    }
    return result;
}


static bool minidump_writer_write_thread_list_header(s_minidump_writer *writer, unsigned long thread_count)
{
    bool result = false;
    if (writer->device->is_valid())
    {
        if (writer->byte_swap == 0)
            thread_count = _byteswap_ulong(thread_count);
        result = writer->device->write(&thread_count, sizeof(thread_count));
    }
    return result;
}

// @address 0x2a78f0 minidump_writer_write_thread_list
extern "C" bool minidump_writer_write_thread_list(s_minidump_writer *writer)
{
    bool result = false;
    if (writer->device->is_valid() && writer->name_list != NULL)
    {
        unsigned long thread_count = 0;
        unsigned long thread_ids[64];
        s_thread_information threads[64];
        memset(thread_ids, 0, sizeof(thread_ids));
        memset(threads, 0, sizeof(threads));
        writer->name_list->begin_thread_dump();
        writer->name_list->get_thread_id_array(thread_ids, 64, &thread_count);
        minidump_writer_add_directory_entry(writer, writer->device->tell(), thread_count * 48 + 4, 3);
        bool success = true;
        success = success && minidump_writer_write_thread_list_header(writer, thread_count);
        for (unsigned long thread_index = 0; thread_index < thread_count; thread_index++)
        {
            writer->name_list->get_thread_information(thread_ids[thread_index], &threads[thread_index]);
            s_thread_information *thread = &threads[thread_index];
            bool is_current_thread = thread->thread_id == GetCurrentThreadId();
            bool is_exception_thread = thread->thread_id == writer->thread_id;
            if (is_exception_thread)
            {
                memcpy(&thread->context, writer->exception_pointers->ContextRecord, sizeof(thread->context));
            }
            else if (is_current_thread)
            {
                unsigned __int64 stack_pointer = 0;
                unsigned __int64 instruction_pointer = minidump_writer_get_caller_address();
                thread->context.gpr[1] = (unsigned __int64)&stack_pointer;
                stack_pointer = __gregister_get(1);
                thread->context.iar = (unsigned long)instruction_pointer;
                thread->context.gpr[1] = stack_pointer;
            }
        }
        success = success && minidump_writer_write_thread_entries(writer, threads, thread_count);
        result = success && minidump_writer_write_thread_contexts(writer, threads, thread_count);
        writer->name_list->end_thread_dump();
    }
    return result;
}

static __forceinline bool minidump_writer_add_memory_range(s_minidump_writer *writer, const s_minidump_memory_range *range)
{
    bool result = false;
    if (writer->memory_range_count < 64)
    {
        result = true;
        writer->memory_ranges[writer->memory_range_count] = *range;
        writer->memory_range_count++;
    }
    return result;
}

// @address 0x2a7b80 minidump_writer_write_thread_entries
extern "C" bool minidump_writer_write_thread_entries(s_minidump_writer *writer, s_thread_information *threads, long thread_count)
{
    bool result = false;
    if (writer->device->is_valid())
    {
        result = true;
        unsigned long context_rva = writer->device->tell() + thread_count * 48;
        long index = 0;
        s_thread_information *thread = threads;
        do
        {
            if (index >= thread_count)
                break;

            thread->context_data_size = sizeof(thread->context);
            thread->context_rva = context_rva;
            thread->stack_rva = 0;
            thread->stack_data_size = thread->stack.size;
            if (writer->stream_format == 0)
            {
                s_minidump_memory_range range;
                range.base = thread->stack.base;
                range.size = thread->stack.size;
                result = result && minidump_writer_add_memory_range(writer, &range);
            }
            if (writer->byte_swap == 0)
                xenon_minidump_endian_swap_thread((s_minidump_thread *)thread);
            result = result && writer->device->write(thread, 48);
            if (writer->byte_swap == 0)
                xenon_minidump_endian_swap_thread((s_minidump_thread *)thread);
            index++;
            context_rva += sizeof(thread->context);
            thread++;
        } while (result);
    }
    return result;
}

// @address 0x2a7d08 minidump_writer_write_thread_contexts
extern "C" bool minidump_writer_write_thread_contexts(s_minidump_writer *writer, const s_thread_information *threads, long thread_count)
{
    bool result = false;
    if (writer->device->is_valid())
    {
        result = true;
        long index = 0;
        const s_thread_information *thread = threads;
        do
        {
            if (index >= thread_count)
                break;

            if (writer->thread_id == thread->thread_id)
            {
                writer->context_rva = writer->device->tell();
                result = result && writer->device->write(writer->exception_pointers->ContextRecord, 2624);
            }
            else
            {
                result = result && writer->device->write(&thread->context, 2624);
            }
            index++;
            thread++;
        } while (result);
    }
    return result;
}

// @address 0x2a7df8 minidump_writer_write_module_list
extern "C" bool minidump_writer_write_module_list(s_minidump_writer *writer)
{
    bool result = false;
    unsigned long count = 0;
    char name[260] = {0};

    if (writer->device->is_valid() && writer->name_list != NULL)
    {
        writer->name_list->get_loaded_module_count(&count);
        minidump_writer_add_directory_entry(writer, writer->device->tell(), count * sizeof(_MINIDUMP_MODULE) + 4, 4);
        result = main_write_uint32_value(writer, count);
        long module_size = count * sizeof(_MINIDUMP_MODULE);
        long base = writer->device->tell() + module_size;
        unsigned long name_offset = 0;
        unsigned long index = 0;
        while (result)
        {
            if (index >= count)
                break;
            _MINIDUMP_MODULE entry;
            memset(&entry, 0, sizeof(entry));
            writer->name_list->get_loaded_module(index, &entry);
            writer->name_list->get_loaded_module_name(index, name, sizeof(name));
            entry.module_name_rva = base + name_offset;
            name_offset += (strlen(name) + 3) * 2;
            if (writer->byte_swap == 0)
                xenon_minidump_endian_swap_module(&entry);
            result = writer->device->write(&entry, sizeof(entry));
            index++;
        }
    }
    return result;
}

// @address 0x2a7fd8 minidump_writer_write_module_names
extern "C" bool minidump_writer_write_module_names(s_minidump_writer *writer)
{
    unsigned long index = 0;
    unsigned long count = 0;
    char name[260];
    bool result;

    if (!writer->device->is_valid() || writer->name_list == NULL)
    {
        result = false;
    }
    else
    {
        memset(name, 0, sizeof(name));
        writer->name_list->get_loaded_module_count(&count);

        result = true;
        do
        {
            if (index >= count)
                break;
            writer->name_list->get_loaded_module_name(index, name, sizeof(name));
            result = minidump_writer_write_string(writer, name);
            index++;
        } while (result);
    }
    return result;
}

// @address 0x2a80b8 minidump_writer_write_exception_stream
extern "C" bool minidump_writer_write_exception_stream(s_minidump_writer *writer)
{
    bool result = false;
    if (writer->device->is_valid() && writer->name_list != NULL)
    {
        MINIDUMP_EXCEPTION_STREAM stream;
        memset(&stream, 0, sizeof(stream));
        minidump_writer_initialize_exception_stream(writer, &stream);
        writer->name_list->update_exception_information(&stream);
        if (writer->byte_swap == 0)
            xenon_minidump_endian_swap_exception_stream(&stream);
        minidump_writer_add_directory_entry(writer, writer->device->tell(), sizeof(stream), 6);
        result = writer->device->write(&stream, sizeof(stream));
    }
    return result;
}

// @address 0x2a81b8 minidump_writer_write_version_block
extern "C" bool minidump_writer_write_version_block(s_minidump_writer *writer)
{
    bool result = false;
    if (writer->device->is_valid() && writer->name_list != NULL)
    {
        _MINIDUMP_MISC_INFO block;
        memset(&block, 0, sizeof(block));
        writer->name_list->get_misc_information(&block);
        if (writer->byte_swap == 0)
        {
            block.flags1 = _byteswap_ulong(block.flags1);
            block.process_create_time = _byteswap_ulong(block.process_create_time);
            block.process_id = _byteswap_ulong(block.process_id);
            block.process_kernel_time = _byteswap_ulong(block.process_kernel_time);
            block.process_user_time = _byteswap_ulong(block.process_user_time);
            block.size_of_info = _byteswap_ulong(block.size_of_info);
        }
        minidump_writer_add_directory_entry(writer, writer->device->tell(), sizeof(block), 15);
        result = writer->device->write(&block, sizeof(block));
    }
    return result;
}

// @address 0x2a99f8 minidump_writer_write_string
extern "C" bool minidump_writer_write_string(s_minidump_writer *writer, const char *name)
{
    bool result = false;
    if (name != NULL && writer->device->is_valid())
    {
        wchar_t buffer[260] = {0};
        wchar_t *destination = buffer;
        const char *source = name;
        for (long count = 0; count < 259; count++)
        {
            if (*source == 0)
                break;
            *destination++ = *source++;
        }
        *destination = 0;
        long length = wcslen(buffer);
        bool header_written = main_write_uint32_value(writer, length * sizeof(wchar_t));
        if (writer->byte_swap == 0)
        {
            for (wchar_t *character = buffer; *character != 0; character++)
                *character = _byteswap_ushort(*character);
        }
        bool success = header_written && writer->device->write(buffer, length * sizeof(wchar_t));
        result = success && writer->device->write(&buffer[length], sizeof(wchar_t));
    }
    return result;
}

// @address 0x2a9d18 minidump_writer_initialize_exception_stream
extern "C" void minidump_writer_initialize_exception_stream(s_minidump_writer *writer, MINIDUMP_EXCEPTION_STREAM *stream)
{
    if (stream == NULL)
        return;

    stream->thread_id = writer->thread_id;
    stream->exception_record.exception_address = (__int64)(intptr_t)writer->exception_pointers->ExceptionRecord->ExceptionAddress;
    stream->exception_record.exception_code = writer->exception_pointers->ExceptionRecord->ExceptionCode;
    stream->exception_record.exception_flags = writer->exception_pointers->ExceptionRecord->ExceptionFlags;
    for (long index = 0; index < 15; index++)
        stream->exception_record.exception_information[index] = writer->exception_pointers->ExceptionRecord->ExceptionInformation[index];
    stream->exception_record.number_parameters = writer->exception_pointers->ExceptionRecord->NumberParameters;
    unsigned long rva = writer->context_rva;
    stream->thread_context.data_size = 0xa40;
    stream->thread_context.rva = rva;
}

// @address 0x2a9df8 minidump_writer_add_directory_entry
extern "C" void minidump_writer_add_directory_entry(s_minidump_writer *writer, long rva, long data_size, long stream_type)
{
    if (writer->entry_count < 8)
    {
        writer->entries[writer->entry_count].rva = rva;
        writer->entries[writer->entry_count].data_size = data_size;
        writer->entries[writer->entry_count].stream_type = stream_type;
        writer->entry_count++;
    }
}

struct s_main_block_writer;
struct s_main_block_writer;

// @address 0x2a8310 minidump_writer_write_memory_list
extern "C" int minidump_writer_write_memory_list(s_minidump_writer *writer)
{
    int result = 0;
    if (writer->device->is_valid() && writer->name_list != NULL)
    {
        long stream_format = writer->stream_format;
        if (stream_format == 0)
            result = main_write_memory_list_stream((s_main_block_writer *)writer);
        else if (stream_format == 1)
            result = main_write_minidump_memory_list((s_main_block_writer *)writer);
        else if (stream_format == 2)
            result = main_write_minidump_memory_list_excluding_sections((s_main_block_writer *)writer);
        else if (stream_format == 3)
            result = main_write_minidump_memory64_list((s_main_block_writer *)writer);
    }
    return result;
}
