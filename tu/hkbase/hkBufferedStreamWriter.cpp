// hkbase/hkBufferedStreamWriter.obj: from the records 0x60fbe8 0x60fc78 0x60fd30 0x60fd80 0x60fdf8 0x60fe60 0x60ff20 0x60ff98 0x610018 0x6100d0 0x6101d0
// @compiler xdk6130
// @flags /TP /O2 /Oc /GF /EHs-c- /GR-
// @address 0x60fbe8 ?flush_stream_buffer@phys_buffered_writer@@IAAHXZ
// @address 0x60fc78 ?write@phys_buffered_writer@@UAAHPBXH@Z
// @address 0x60fd30 ?flush@phys_buffered_writer@@UAAXXZ
// @address 0x60fd80 ?is_stream_ok@phys_buffered_writer@@UBA?AVphys_bool@@XZ
// @address 0x60fdf8 ?seek_tell_supported@phys_buffered_writer@@UBA?AVphys_bool@@XZ
// @address 0x60fe60 ?seek@phys_buffered_writer@@UAA?AW4phys_result@@HW4seek_whence@phys_writer@@@Z
// @address 0x60ff20 ?tell@phys_buffered_writer@@UBAHXZ
// @address 0x60ff98 ??0phys_buffered_writer@@QAA@PAXHVphys_bool@@@Z
// @address 0x610018 ??0phys_buffered_writer@@QAA@PAVphys_writer@@H@Z
// @address 0x6100d0 ??1phys_buffered_writer@@UAA@XZ
// @address 0x6101d0 ??_Gphys_buffered_writer@@UAAPAXI@Z
#define PHYS_CONFIG_SIMD 1


#include "phys/phys_base.h"
#include "phys/phys_buffered_writer.h"



static const int READER_BUFFER_ALIGN = 64;
static const int READER_BLOCK_BYTES = 512;

#define READER_IS_POW2(A) (((A)&((A)-1))==0)


phys_buffered_writer::phys_buffered_writer(phys_writer* s, int buffer_bytes)
	:	m_stream(s),
		m_owns_buffer(true)

{
	if (m_stream)
	{
		m_stream->add_object_reference();
	}
	m_read_buf = phys_aligned_alloc<char>( READER_BUFFER_ALIGN, buffer_bytes, PHYS_MEM_STREAM );
	m_buf_size = 0;
	m_buf_capacity = buffer_bytes;
}

phys_buffered_writer::phys_buffered_writer(void* mem, int mem_byte_count, phys_bool mem_is_string )
	:	m_stream(0),
		m_read_buf( static_cast<char*>(mem) ),
		m_buf_size(0),
		m_buf_capacity( mem_is_string ? mem_byte_count-1 : mem_byte_count ),
		m_owns_buffer(false)
{
	if( mem_is_string )
	{
		phys_string::mem_fill( mem, 0, mem_byte_count );
	}
}

phys_buffered_writer::~phys_buffered_writer()
{
	flush();
	if( m_stream )
	{
		m_stream->object_remove_ref();
	}
	if( m_owns_buffer )
	{
		phys_aligned_free<char>(m_read_buf);
	}
}

int phys_buffered_writer::flush_stream_buffer()
{
	if( m_stream )
	{
		int nbytes = m_buf_size;
		int written_byte_cnt = 0;
		while( written_byte_cnt < nbytes )
		{
			int chunk_written = m_stream->write( m_read_buf + written_byte_cnt, nbytes - written_byte_cnt );
			written_byte_cnt += chunk_written;
			if (chunk_written == 0 )
			{
				return written_byte_cnt;
			}
		}
		m_buf_size = 0;
		return written_byte_cnt;
	}
	else
	{
		return 0;
	}
}

int phys_buffered_writer::write(const void* mem, int mem_byte_count)
{
	int stream_bytes_left = mem_byte_count;
	int spare_bytes = m_buf_capacity - m_buf_size;
	const char* src_bytes = static_cast<const char*>(mem);

	while( stream_bytes_left > spare_bytes )
	{
		phys_string::mem_copy( m_read_buf+m_buf_size, src_bytes, spare_bytes );
		src_bytes += spare_bytes;
		m_buf_size += spare_bytes;
		stream_bytes_left -= spare_bytes;

		int bytes_in_buf = m_buf_size;
		if( flush_stream_buffer() != bytes_in_buf )
		{

			return mem_byte_count - stream_bytes_left;
		}
		spare_bytes = m_buf_capacity - m_buf_size;
	}

	phys_string::mem_copy(m_read_buf + m_buf_size, src_bytes, stream_bytes_left );
	m_buf_size += stream_bytes_left;
	return mem_byte_count;
}

void phys_buffered_writer::flush()
{
	flush_stream_buffer();
	if( m_stream )
	{
		m_stream->flush();
	}
}

phys_bool phys_buffered_writer::is_stream_ok() const
{
	return m_stream
		? m_stream->is_stream_ok()
		: phys_bool(m_buf_size != m_buf_capacity);
}

phys_bool phys_buffered_writer::seek_tell_supported() const
{
	return m_stream
	       ? m_stream->seek_tell_supported()
	       : phys_bool(true);
}

phys_result phys_buffered_writer::seek(int rel_offset, seek_whence seek_origin)
{
	if( m_stream )
	{

		flush_stream_buffer();
		return m_stream->seek(rel_offset,seek_origin);
	}
	else
	{

		int pos = -1;
		switch(seek_origin)
		{
			case SEEK_FROM_START:
				pos = rel_offset;
				break;
			case SEEK_FROM_CURRENT:
				pos = m_buf_size + rel_offset;
				break;
			case AGENT_STREAM_END:
				pos = m_buf_size - rel_offset;
				break;
		}
		phys_result ok = PHYS_SUCCESS;
		if(	pos < 0 )
		{
			pos = 0;
			ok = PHYS_FAILURE;
		}
		else if( pos > m_buf_capacity )
		{
			pos = m_buf_capacity;
			ok = PHYS_FAILURE;
		}
		m_buf_size = pos;
		return ok;
	}
}

int phys_buffered_writer::tell() const
{
	int child_stream_pos = m_stream ? m_stream->tell() : 0;
	if( child_stream_pos >= 0 )
	{
		return child_stream_pos + m_buf_size;
	}
	return -1;
}

