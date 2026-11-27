// hkbase/hkError.obj: from the records 0x60bf00 0x60bf90 0x60c098
// @flags /TP /O2 /Oc
#include "phys/phys_base_types.h"
#include "phys/phys_base.h"
#include "phys/phys_err_stream.h"
#include "phys/phys_error.h"
// @address 0x60bf90 ?phys_remove_reference_error@@YAXPBVphys_ref_object@@PBD@Z
#include "phys/phys_out_stream.h"
#include "phys/phys_buffered_writer.h"

// @address 0x60bf00 ??0phys_err_stream@@QAA@PAXH@Z
// @address 0x60c098 phys_error_message
template<> phys_error *phys_singleton<phys_error>::s_instance = 0;

phys_err_stream::phys_err_stream(void *buffer, int size) : phys_out_stream((phys_writer *)0)
{
#ifdef HALO3_64BIT
  int reserved_size = (int)sizeof(phys_buffered_writer);
  size_t raw_end = (size_t)((char *)buffer + size - reserved_size);
  void *writer_mem = (void *)(raw_end & ~(size_t)(__alignof(phys_buffered_writer) - 1));
  int usable_size = (int)((char *)writer_mem - (char *)buffer);
  m_writer = (phys_writer *)new (writer_mem) phys_buffered_writer(buffer, usable_size, true);
#else
  int reserved_size = 32;
  void *writer_mem = (char *)buffer + size - reserved_size;
  m_writer = (phys_writer *)new (writer_mem) phys_buffered_writer(buffer, size - reserved_size, true);
#endif
  m_writer->add_object_reference();
}

void phys_remove_reference_error(const phys_ref_object *object, const char *name)
{
  char buffer[512];
  phys_err_stream stream(buffer, 512);
  int ref_count = object->m_object_ref_count;
  const char *overflow_hint = " * Do you have more than 32768 references? (unlikely)\n";
  const char *valid_object_hint = " * Is this a valid object?\n";
  const char *remove_ref_hint = " * Have you called removeReference too many times?\n";
  const char *delete_hint = " * Are you calling delete instead of removeReference?\n";
  const char *line_ending = ".\n";
  const char *msg_name_prefix = " in ";
  const char *count_prefix = " with ref count of ";
  const char *message_prefix = "Reference count error on object ";
  stream << message_prefix << (const void *)object << count_prefix << ref_count << msg_name_prefix << name << line_ending << delete_hint
         << remove_ref_hint << valid_object_hint << overflow_hint;
  phys_error *error = phys_singleton<phys_error>::s_instance;
  error->message(phys_error::MESSAGE_ERROR, 0x2c66f2d8, buffer, ".\\error\\hkError.cpp", 30);
}

extern "C" void phys_error_message(const char *message)
{
  char buffer[512];
  phys_err_stream stream(buffer, 512);
  stream << message;
  phys_error *error = phys_singleton<phys_error>::s_instance;
  error->message(phys_error::MESSAGE_ERROR, 0x2636fe25, buffer, ".\\error\\hkError.cpp", 37);
}
