// hkbase/hkOstream.obj: from the records 0x5c9fd8 0x60c708 0x60c778 0x60c7b8 0x60c820 0x60c8a8 0x60c8f8 0x60c960 0x60c9c8 0x60ca30 0x60ca98 0x60cb00 0x60cb68 0x60cbd0 0x60cc38 0x60ccd0 0x60cce8 0x60cd00 0x60cda0 0x60ce00 0x60ce40 0x60ce80 0x60cee0 0x60cf90 0x60cfd8 0x60d050 0x60d0d0 0x60d148 0x60d150 0x60d168 0x60d240
// @flags /TP /O2 /Oc
// @address 0x60cc38 ?printf@phys_out_stream@@QAAXPBDZZ
// @address 0x60d0d0 ?null_terminate@phys_array_writer@@QAAXXZ
#include "phys/phys_base_types.h"
#include "phys/phys_out_stream.h"
#include "phys/phys_buffered_writer.h"
#include "phys/phys_singleton.h"
// @address 0x60c118 ??_Gphys_out_stream@@UAAPAXI@Z
// @address 0x60c708 ?write_text@@YAXPAVphys_writer@@PBD@Z
// @address 0x60c778 ?is_stream_ok@phys_out_stream@@QBA?AVphys_bool@@XZ
// @address 0x60c7b8 ??6phys_out_stream@@QAAAAV0@PBX@Z
// @address 0x60c8a8 ??6phys_out_stream@@QAAAAV0@D@Z
// @address 0x60c8f8 ??6phys_out_stream@@QAAAAV0@PBD@Z
// @address 0x60c960 ??6phys_out_stream@@QAAAAV0@F@Z
// @address 0x60c9c8 ??6phys_out_stream@@QAAAAV0@G@Z
// @address 0x60ca30 ??6phys_out_stream@@QAAAAV0@H@Z
// @address 0x60ca98 ??6phys_out_stream@@QAAAAV0@I@Z
// @address 0x60cb00 ??6phys_out_stream@@QAAAAV0@M@Z
// @address 0x60cb68 ??6phys_out_stream@@QAAAAV0@_J@Z
// @address 0x60cbd0 ??6phys_out_stream@@QAAAAV0@_K@Z
// @address 0x60ccd0 ?flush@phys_out_stream@@QAAXXZ
// @address 0x60cce8 ?write@phys_out_stream@@QAAHPBDH@Z
// @address 0x60cd00 ?set_stream_writer@phys_out_stream@@QAAXPAVphys_writer@@@Z
// @address 0x60cda0 ?endl@phys_out_stream_manip@@YAAAVphys_out_stream@@AAV2@@Z
// @address 0x60ce00 ?flush@phys_out_stream_manip@@YAAAVphys_out_stream@@AAV2@@Z
// @address 0x60ce40 ??0phys_out_stream@@QAA@PAVphys_writer@@@Z
// @address 0x60ce80 ??0phys_out_stream@@QAA@PBD@Z
// @address 0x60cee0 ??1phys_out_stream@@UAA@XZ
// @address 0x60cf90 ??6phys_out_stream@@QAAAAV0@ABVphys_string@@@Z
// @address 0x60cfd8 ??0phys_out_stream@@QAA@PAXHVphys_bool@@@Z
// @address 0x60d150 ?clear@phys_array_writer@@UAAXXZ
#include <stdarg.h>
#include "phys/phys_string.h"
#include <stddef.h>
#include "physics/havok_halo_external.h"
#include "phys/phys_math.h"
#include "phys/phys_object.h"
#include "phys/phys_thread_memory.h"
#include "phys/phys_memory_class.h"
#include "phys/phys_statistics_collector.h"
#include "phys/phys_streambuf_factory.h"
#include "phys/phys_array_writer.h"
template<typename T> class phys_array;

void write_text(phys_writer *writer, const char *text)
{
  if (text != 0) writer->write(text, phys_string::str_len(text));
  else writer->write("(null)", 6);
}

phys_bool phys_out_stream::is_stream_ok() const
{
  return m_writer->is_stream_ok();
}

phys_out_stream &phys_out_stream::operator<<(const void *value)
{
  char buffer[10024];
  phys_string::format_string_n(buffer, 10024, "%p", value);
  write_text(m_writer, buffer);
  return *this;
}

// @address 0x60c820 ??6phys_out_stream@@QAAAAV0@Vphys_bool@@@Z
phys_out_stream &phys_out_stream::operator<<(phys_bool value)
{
    const char *string;
    if (value) string="true";
    else string="false";
    phys_writer *writer = m_writer;
    if (string != 0) writer->write(string, phys_string::str_len(string));
    else writer->write("(null)",6);
    return *this;
}

phys_out_stream &phys_out_stream::operator<<(char value)
{
  m_writer->write(&value, 1);
  return *this;
}

phys_out_stream &phys_out_stream::operator<<(const char *string)
{
    phys_writer *o = m_writer;
    if (string != 0) o->write(string, phys_string::str_len(string));
    else o->write("(null)",6);
    return *this;
}

phys_out_stream &phys_out_stream::operator<<(short value)
{
  char buffer[10024];
  phys_string::format_string_n(buffer, 10024, "%i", value);
  write_text(m_writer, buffer);
  return *this;
}

phys_out_stream &phys_out_stream::operator<<(unsigned short value)
{
  char buffer[10024];
  phys_string::format_string_n(buffer, 10024, "%u", value);
  write_text(m_writer, buffer);
  return *this;
}

phys_out_stream &phys_out_stream::operator<<(int value)
{
  char buffer[10024];
  phys_string::format_string_n(buffer, 10024, "%i", value);
  write_text(m_writer, buffer);
  return *this;
}

phys_out_stream &phys_out_stream::operator<<(unsigned int value)
{
  char buffer[10024];
  phys_string::format_string_n(buffer, 10024, "%u", value);
  write_text(m_writer, buffer);
  return *this;
}

phys_out_stream &phys_out_stream::operator<<(float value)
{
  char buffer[10024];
  phys_string::format_string_n(buffer, 10024, "%f", value);
  write_text(m_writer, buffer);
  return *this;
}

phys_out_stream &phys_out_stream::operator<<(__int64 value)
{
  char buffer[10024];
  phys_string::format_string_n(buffer, 10024, "%I64i", value);
  write_text(m_writer, buffer);
  return *this;
}

phys_out_stream &phys_out_stream::operator<<(unsigned __int64 value)
{
  char buffer[10024];
  phys_string::format_string_n(buffer, 10024, "%I64u", value);
  write_text(m_writer, buffer);
  return *this;
}

void phys_out_stream::printf(const char *format, ...)
{
  char buffer[10024];
  va_list args;
  va_start(args, format);
  phys_string::vsnprintf(buffer, 10024, format, args);
  va_end(args);
  phys_writer *writer = m_writer;
  writer->write(buffer, phys_string::str_len(buffer));
}

void phys_out_stream::flush()
{
  m_writer->flush();
}

int phys_out_stream::write(const char *buffer, int size)
{
  return m_writer->write(buffer, size);
}

void phys_out_stream::set_stream_writer(phys_writer *writer)
{
  writer->add_object_reference();
  m_writer->object_remove_ref();
  m_writer = writer;
}

namespace phys_out_stream_manip {
phys_out_stream &endl(phys_out_stream &m_writer)
{
  phys_writer *writer = m_writer.get_stream_writer();
  writer->write("\r\n", phys_string::str_len("\r\n"));
  m_writer.flush();
  return m_writer;
}

phys_out_stream &flush(phys_out_stream &m_writer)
{
  m_writer.flush();
  return m_writer;
}
}

phys_out_stream::phys_out_stream(phys_writer *writer) : m_writer(writer)
{
  if (m_writer) m_writer->add_object_reference();
}

phys_out_stream::phys_out_stream(const char *filename)
{
  m_writer = phys_singleton<phys_streambuf_factory>::s_instance->open_writer(filename);
}

phys_out_stream::~phys_out_stream()
{
  if (m_writer) m_writer->object_remove_ref();
}

phys_out_stream &phys_out_stream::operator<<(const phys_string &value)
{
  m_writer->write(value.string_c_str(), value.get_string_length());
  return *this;
}

phys_out_stream::phys_out_stream(void *buffer, int size, phys_bool memory_backed)
{
  m_writer = new phys_buffered_writer(buffer, size, memory_backed);
}

// @address 0x60d050 ??0phys_out_stream@@QAA@AAV?$phys_array@D@@@Z
phys_out_stream::phys_out_stream(phys_array<char> &buf)
{
  m_writer = new phys_array_writer(&buf, phys_array_writer::WRITER_ARRAY_BORROWED);
}

void phys_array_writer::null_terminate()
{
  phys_array<char> *buffer = array;
  int required = buffer->m_size + 1;
  if ((int)(buffer->m_capacity_and_flags & 0x3fffffff) < required) {
    int grown_capacity = (buffer->m_capacity_and_flags & 0x3fffffff) << 1;
    phys_array_util::reserve_array_storage(buffer, required < grown_capacity ? grown_capacity : required, 1);
  }
  array->m_data[array->m_size] = 0;
}

// @address 0x60d148 phys_array_writer_get_position
extern "C" int phys_array_writer_get_position(phys_array_writer *writer)
{
  return writer->position;
}

void phys_array_writer::clear()
{
  array->m_size = 0;
  position = 0;
  null_terminate();
}

// @address 0x60d168 ?write@phys_array_writer@@UAAHPBXH@Z
int phys_array_writer::write(const void *source_buffer, int byte_count)
{
    phys_array<char> *s = array;
    int len = s->m_size;
    int avail_bytes = len - position;
    if (byte_count > avail_bytes) {
        int n = (len - avail_bytes) + byte_count;
        int cap = s->m_capacity_and_flags & 0x3fffffff;
        int need = n + 1;
        if (cap < need) {
            int doubled_cap = cap << 1;
            
            phys_array_util::reserve_array_storage((void *)s, need < doubled_cap ? doubled_cap : need, 1);
        }
        array->m_size = n;
        array->m_data[n] = 0;
    } else {
        if ((s->m_capacity_and_flags & 0x3fffffff) > len)
            s->m_data[len] = 0;
    }
    ::phys_string::mem_copy(array->m_data + position, source_buffer, byte_count);
    position = position + byte_count;
    return byte_count;
}

// @address 0x5c9fd8 ?tell@phys_array_writer@@UBAHXZ
int phys_array_writer::tell() const
{
#if !defined(_XBOX) || defined(__clang__)
    // GAME-CHANGES F125: the Xbox hkArrayStreamWriter vtable 0x82014ae4 slot 7 is 0x60d148 (the write position);
    // 0x5c9fd8 is a shared `lwz r3,8(r3)` getter that returns the array pointer.
    return phys_array_writer_get_position(const_cast<phys_array_writer *>(this));
#else
    return (int)array;
#endif
}

// @address 0x60d240 ?seek@phys_array_writer@@UAA?AW4phys_result@@HW4seek_whence@phys_writer@@@Z
phys_result phys_array_writer::seek(int offset, seek_whence seek_origin)
{
  int abs_offset = position;
  switch (seek_origin) {
    case SEEK_FROM_START:
      abs_offset = offset;
      break;
    case SEEK_FROM_CURRENT:
      abs_offset = position + offset;
      break;
    case AGENT_STREAM_END:
      abs_offset = array->m_size - offset;
      break;
    default:
      break;
  }
  if (abs_offset >= 0) {
    if (abs_offset > array->getSize()) {
      int array_new_size = abs_offset + 1;
      array->set_array_size(array_new_size, 0);
      array->set_size_unchecked(abs_offset);
    }
    position = abs_offset;
    return PHYS_SUCCESS;
  }
  return PHYS_FAILURE;
}

