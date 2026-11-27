// hkbase/hkStreamReader.obj: from the records 0x60e0c0 0x60e128 0x60e180 0x60e188 0x60e190 0x60e198 0x60e1a0 0x60e1b0
// @flags /TP /O2 /Oc
#include "phys/phys_base_types.h"
#include "phys/phys_base.h"
// @address 0x60e0c0 ?skip@phys_reader@@UAAHH@Z
// @address 0x60e128 ?read_char@phys_reader@@QAAHXZ
// @address 0x60e180 ?set_stream_mark@phys_reader@@UAA?AW4phys_result@@H@Z
// @address 0x60e188 ?rewind_to_mark@phys_reader@@UAA?AW4phys_result@@XZ
// @address 0x60e190 ?seek@phys_reader@@UAA?AW4phys_result@@HW4seek_whence@1@@Z
// @address 0x60e198 ?tell@phys_reader@@UBAHXZ
// @address 0x60e1a0 ?mark_supported@phys_reader@@UBA?AVphys_bool@@XZ
// @address 0x60e1b0 ?seek_tell_supported@phys_reader@@UBA?AVphys_bool@@XZ

#include "phys/phys_reader.h"

int phys_reader::skip(int size)
{
  int remaining = size;
  char buffer[512];
  while (remaining != 0) {
    int stream_bytes_read = read(buffer, remaining > 512 ? 512 : remaining);
    if (stream_bytes_read == 0)
      break;
    remaining -= stream_bytes_read;
  }
  return size - remaining;
}

int phys_reader::read_char()
{
  char character;
  if (read(&character, 1) != 0)
    return character;
  return -1;
}

phys_result phys_reader::set_stream_mark(int size)
{
  return PHYS_FAILURE;
}
phys_result phys_reader::rewind_to_mark()
{
  return PHYS_FAILURE;
}
phys_result phys_reader::seek(int offset, seek_whence seek_origin)
{
  return PHYS_FAILURE;
}
int phys_reader::tell() const
{
  return -1;
}
phys_bool phys_reader::mark_supported() const
{
  return phys_bool(false);
}
phys_bool phys_reader::seek_tell_supported() const
{
  return phys_bool(false);
}
