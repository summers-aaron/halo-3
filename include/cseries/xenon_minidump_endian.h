#ifndef CSERIES_XENON_MINIDUMP_ENDIAN_H
#define CSERIES_XENON_MINIDUMP_ENDIAN_H

// The minidump stream records (the layout the dump file stores), and the functions defined in
// tu/blamlibxbox/xenon_minidump_endian.cpp that swap them.

struct s_minidump_location_descriptor
{
    unsigned long data_size;
    unsigned long rva;
};

struct _MINIDUMP_SYSTEM_INFO
{
    unsigned short processor_architecture;
    unsigned short processor_level;
    unsigned short processor_revision;
    unsigned char number_of_processors;
    unsigned char product_type;
    unsigned long major_version;
    unsigned long minor_version;
    unsigned long build_number;
    unsigned long platform_id;
    unsigned long csd_version_rva;
    unsigned short suite_mask;
    unsigned short reserved;
    union
    {
        struct
        {
            unsigned long vendor_id[3];
            unsigned long version_information;
            unsigned long feature_information;
            unsigned long amd_extended_cpu_features;
        } x86;
        struct
        {
            unsigned __int64 processor_features[2];
        } other;
    } cpu;
};

struct s_minidump_exception
{
    unsigned long exception_code;
    unsigned long exception_flags;
    unsigned __int64 exception_record;
    unsigned __int64 exception_address;
    unsigned long number_parameters;
    unsigned long __unused_alignment;
    unsigned __int64 exception_information[15];
};

struct MINIDUMP_EXCEPTION_STREAM
{
    unsigned long thread_id;
    unsigned long __alignment;
    s_minidump_exception exception_record;
    s_minidump_location_descriptor thread_context;
};

struct s_minidump_thread;

extern "C" void xenon_minidump_endian_swap_system_info(_MINIDUMP_SYSTEM_INFO *info);
extern "C" void xenon_minidump_endian_swap_thread(s_minidump_thread *thread);
extern "C" void xenon_minidump_endian_swap_exception_stream(MINIDUMP_EXCEPTION_STREAM *stream);

#endif // CSERIES_XENON_MINIDUMP_ENDIAN_H
