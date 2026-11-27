#ifndef NETWORKING_ONLINE_MINIDUMP_FILE_OUTPUT_SECURE_H
#define NETWORKING_ONLINE_MINIDUMP_FILE_OUTPUT_SECURE_H

#include "cseries/minidump_interfaces.h"
#include "memory/data_compress_encrypted_writer_types.h"
#include "networking/online/online_file_manager.h"
#include "tag_files/file_reference.h"

class c_minidump_file_output_secure : public c_minidump_file_output_interface
{
public:
    void open(const char *name) override;
    bool close() override;
    bool is_valid() override;
    bool write(const void *data, long size) override;
    bool seek(long position) override;
    long tell() override;

    bool m_open;
    long m_count;
    long m_value;
    s_file_reference file;
    s_online_file_buffered_writer buffered_writer;
    s_online_files_encrypted_writer encrypted_writer;
};

#endif // NETWORKING_ONLINE_MINIDUMP_FILE_OUTPUT_SECURE_H
