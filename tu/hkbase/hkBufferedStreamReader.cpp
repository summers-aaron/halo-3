// hkbase/hkBufferedStreamReader.obj: from the records 0x60f4d8 0x60f5a8 0x60f668 0x60f730 0x60f7b8 0x60f828 0x60f840 0x60f868 0x60f890 0x60f8d0 0x60f900 0x60f970 0x60f9f0 0x60fa40 0x60fab0 0x60fb80
// @compiler xdk6130
// @flags /TP /O2 /Oc /GF /EHs-c- /GR-
// @address 0x60f4d8 ?prepare_buffer_refill@phys_buffered_reader@@IAAXXZ
// @address 0x60f5a8 ?refill_buffer@phys_buffered_reader@@MAA?AW4phys_result@@XZ
// @address 0x60f668 ?read@phys_buffered_reader@@UAAHPAXH@Z
// @address 0x60f730 ?skip@phys_buffered_reader@@UAAHH@Z
// @address 0x60f7b8 ?is_stream_ok@phys_buffered_reader@@UBA?AVphys_bool@@XZ
// @address 0x60f828 ?mark_supported@phys_buffered_reader@@UBA?AVphys_bool@@XZ
// @address 0x60f840 ?set_stream_mark@phys_buffered_reader@@UAA?AW4phys_result@@H@Z
// @address 0x60f868 ?rewind_to_mark@phys_buffered_reader@@UAA?AW4phys_result@@XZ
// @address 0x60f890 ?seek_tell_supported@phys_buffered_reader@@UBA?AVphys_bool@@XZ
// @address 0x60f8d0 ?seek@phys_buffered_reader@@UAA?AW4phys_result@@HW4seek_whence@phys_reader@@@Z
// @address 0x60f900 ?tell@phys_buffered_reader@@UBAHXZ
// @address 0x60f970 ??0Buffer@phys_buffered_reader@@QAA@H@Z
// @address 0x60f9f0 ??1Buffer@phys_buffered_reader@@QAA@XZ
// @address 0x60fa40 ??0phys_buffered_reader@@QAA@PAVphys_reader@@H@Z
// @address 0x60fab0 ??1phys_buffered_reader@@UAA@XZ
// @address 0x60fb80 ??_Gphys_buffered_reader@@UAAPAXI@Z
#define PHYS_CONFIG_SIMD 1

#include "phys/phys_base.h"

#include "phys/phys_buffered_reader.h"

static const int READER_BUFFER_ALIGN = 64;
static const int READER_BLOCK_BYTES = 512;

#define READER_IS_POW2(A) (((A)&((A)-1))==0)

phys_buffered_reader::Buffer::Buffer(int cap)
	:	begin( phys_aligned_alloc<char>( READER_BUFFER_ALIGN, cap, PHYS_MEM_STREAM ) ),
		current(0),
		size(0),
		capacity(cap),
		mark_pos(-1),
		mark_limit(-1)
{
}

phys_buffered_reader::Buffer::~Buffer()
{
	phys_aligned_free<char>(begin);
}

phys_buffered_reader::phys_buffered_reader(phys_reader* s, int buffer_bytes)
	:	m_stream(s),
		m_read_buf(buffer_bytes)

{
	m_stream->add_object_reference();
}

phys_buffered_reader::~phys_buffered_reader()
{
	m_stream->object_remove_ref();
}

void phys_buffered_reader::prepare_buffer_refill()
{
	if( m_read_buf.mark_pos < 0 )
	{

		m_read_buf.current = 0;
		m_read_buf.size = 0;
	}
	else if( m_read_buf.current - m_read_buf.mark_pos > m_read_buf.mark_limit )
	{

		m_read_buf.current = 0;
		m_read_buf.size = 0;
		m_read_buf.mark_pos = -1;
		m_read_buf.mark_limit = -1;
	}
	else if( m_read_buf.mark_pos > 0 )
	{

		int marked_bytes = m_read_buf.current - m_read_buf.mark_pos;
		int rounded = marked_bytes % READER_BLOCK_BYTES;
		int dest_pos = rounded ? READER_BLOCK_BYTES - rounded : 0;
		phys_string::mem_move(	m_read_buf.begin + dest_pos,
							m_read_buf.begin + m_read_buf.mark_pos,
							marked_bytes );
		m_read_buf.mark_pos = dest_pos;
		int blocks_used = (marked_bytes / READER_BLOCK_BYTES) + (rounded ? 1 : 0);
		m_read_buf.current = blocks_used * READER_BLOCK_BYTES;
		m_read_buf.size = m_read_buf.current;
	}
}

phys_result phys_buffered_reader::refill_buffer()
{
	if( m_stream->is_stream_ok()==false )
	{
		return PHYS_FAILURE;
	}
	prepare_buffer_refill();

	int nbytes = m_read_buf.capacity - m_read_buf.size;
	int stream_bytes_read = 0;
	while( stream_bytes_read < nbytes )
	{
		int chunk_read = m_stream->read( m_read_buf.begin + m_read_buf.current, nbytes);
		stream_bytes_read += chunk_read;
		m_read_buf.size += chunk_read;

		if( chunk_read != nbytes )
		{
			return stream_bytes_read ? PHYS_SUCCESS : PHYS_FAILURE;
		}
	}

	return PHYS_SUCCESS;
}

int phys_buffered_reader::read(void* buf, int nbytes)
{
	int stream_bytes_left = nbytes;
	int bytes_available = m_read_buf.size - m_read_buf.current;
	char* char_buffer = static_cast<char*>(buf);

	while( stream_bytes_left > bytes_available )
	{
		phys_string::mem_copy(char_buffer, m_read_buf.begin+m_read_buf.current, bytes_available );
		char_buffer += bytes_available;
		stream_bytes_left -= bytes_available;
		m_read_buf.current += bytes_available;

		if( refill_buffer() != PHYS_SUCCESS )
		{
			return nbytes - stream_bytes_left;
		}
		bytes_available = m_read_buf.size - m_read_buf.current;
	}

	phys_string::mem_copy(char_buffer, m_read_buf.begin+m_read_buf.current, stream_bytes_left );
	m_read_buf.current += stream_bytes_left;

	return nbytes;
}

int phys_buffered_reader::skip(int nbytes)
{
	int stream_bytes_left = nbytes;
	int bytes_available = m_read_buf.size - m_read_buf.current;

	while( stream_bytes_left > bytes_available )
	{
		stream_bytes_left -= bytes_available;

		if( refill_buffer() != PHYS_SUCCESS )
		{
			return nbytes - stream_bytes_left;
		}
		bytes_available = m_read_buf.size - m_read_buf.current;
	}

	m_read_buf.current += stream_bytes_left;
	return nbytes;
}

phys_bool phys_buffered_reader::is_stream_ok() const
{
	return m_read_buf.current != m_read_buf.size || m_stream->is_stream_ok();
}

phys_bool phys_buffered_reader::mark_supported() const
{
	return m_read_buf.capacity != 0;
}

phys_result phys_buffered_reader::set_stream_mark(int mark_limit)
{
	m_read_buf.mark_pos = m_read_buf.current;
	m_read_buf.mark_limit = mark_limit;
	return mark_limit <= m_read_buf.capacity ? PHYS_SUCCESS : PHYS_FAILURE;
}

phys_result phys_buffered_reader::rewind_to_mark()
{
	if(m_read_buf.mark_pos >= 0)
	{
		m_read_buf.current = m_read_buf.mark_pos;
		return PHYS_SUCCESS;
	}
	return PHYS_FAILURE;
}

phys_bool phys_buffered_reader::seek_tell_supported() const
{
	return m_stream->seek_tell_supported();
}

phys_result phys_buffered_reader::seek( int offset, seek_whence seek_origin)
{
	m_read_buf.mark_pos = -1;
	m_read_buf.mark_limit = -1;
	m_read_buf.current = 0;
	m_read_buf.size = 0;
	return m_stream->seek(offset, seek_origin);
}

int phys_buffered_reader::tell() const
{
	int child_stream_pos = m_stream->tell();
	if( child_stream_pos >= 0 )
	{
		int offset = m_read_buf.size - m_read_buf.current;
		return child_stream_pos - offset;
	}
	return -1;
}

