#ifndef INCLUDE_PHYS_BUFFERED_READER_H
#define INCLUDE_PHYS_BUFFERED_READER_H

// hkBufferedStreamReader, defined in tu/hkbase/hkBufferedStreamReader.cpp.

#include "phys/phys_reader.h"

class phys_buffered_reader : public phys_reader
{
	public:

		phys_buffered_reader(phys_reader* s, int buffer_bytes=4096);

		~phys_buffered_reader();

		virtual int read(void* buf, int nbytes);

		virtual int skip(int nbytes);

		virtual phys_bool is_stream_ok() const;

		virtual phys_bool mark_supported() const;

		virtual phys_result set_stream_mark(int mark_limit);

		virtual phys_result rewind_to_mark();
		
		virtual phys_bool seek_tell_supported() const;

		virtual phys_result seek( int offset, seek_whence seek_origin);

		virtual int tell() const;

	protected:

		phys_reader* m_stream; 

		struct Buffer
		{
			PHYS_DECLARE_NONVIRTUAL_ALLOCATOR( PHYS_MEM_BASE_CLASS, phys_buffered_reader::Buffer );

			Buffer(int cap);
			~Buffer();
			char* begin;
			int current; 
			int size; 
			int capacity; 
			int mark_pos; 
			int mark_limit; 
		};

		Buffer m_read_buf;

	protected:

		void prepare_buffer_refill();
			
		virtual phys_result refill_buffer();
};

#endif // HAVOK_HK_BUFFERED_STREAM_READER_H
