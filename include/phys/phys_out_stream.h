#ifndef INCLUDE_PHYS_OUT_STREAM_H
#define INCLUDE_PHYS_OUT_STREAM_H
#include "phys/phys_writer.h"
class phys_out_stream;
class phys_writer;
class phys_string;
template<typename T> class phys_array;

namespace phys_out_stream_manip
{

	typedef phys_out_stream& (PHYS_CALL *function)(phys_out_stream&);

	phys_out_stream& PHYS_CALL endl(phys_out_stream& os);

	phys_out_stream& PHYS_CALL flush(phys_out_stream& os);
}

class phys_out_stream : public phys_ref_object
{
	public:

		PHYS_DECLARE_ALLOCATOR(PHYS_MEM_STREAM);

		explicit phys_out_stream(phys_writer* sr);

		explicit phys_out_stream(const char* filename);

		phys_out_stream(void* mem, int mem_byte_count, phys_bool is_string_stream=false);

		explicit phys_out_stream( phys_array<char>& buf );

		~phys_out_stream();

		phys_bool is_stream_ok() const;

		phys_out_stream& operator<< (const void* p);

		phys_out_stream& operator<< (phys_bool b);

		phys_out_stream& operator<< (char c);

		phys_out_stream& operator<< (signed char c);

		phys_out_stream& operator<< (unsigned char c);

		phys_out_stream& operator<< (const char* s);

		phys_out_stream& operator<< (const signed char* s);

		phys_out_stream& operator<< (const unsigned char* s);

		phys_out_stream& operator<< (short s);

		phys_out_stream& operator<< (unsigned short s);

		phys_out_stream& operator<< (int i);

		phys_out_stream& operator<< (unsigned int u);

		phys_out_stream& operator<< (float f);

		phys_out_stream& operator<< (double d);

		phys_out_stream& operator<< (__int64 i);

		phys_out_stream& operator<< (unsigned __int64 u);

		phys_out_stream& operator<< (const phys_string& str);

		int write( const char* buf, int nbytes);

		void printf( const char *format_str, ...);

		void flush();

		phys_out_stream& operator<< (phys_out_stream_manip::function f);

		phys_writer* get_stream_writer();

		void set_stream_writer(phys_writer* new_writer);

	protected:

		phys_writer*	m_writer;
};

typedef phys_out_stream phys_out_file_stream;

#define stream_endl phys_out_stream_manip::endl
#define stream_flush phys_out_stream_manip::flush

__forceinline phys_writer*	phys_out_stream::get_stream_writer()
{
	return m_writer;
}

__forceinline phys_out_stream& phys_out_stream::operator<< (signed char c)
{
	return (*this) << static_cast<char>(c);
}

__forceinline phys_out_stream& phys_out_stream::operator<< (unsigned char c)
{
	return (*this) << static_cast<char>(c);
}

__forceinline phys_out_stream& phys_out_stream::operator<< (const signed char* s)
{
	return (*this) << reinterpret_cast<const char*>(s);
}

__forceinline phys_out_stream& phys_out_stream::operator<< (const unsigned char* s)
{
	return (*this) << reinterpret_cast<const char*>(s);
}

__forceinline phys_out_stream& phys_out_stream::operator<< (double d)
{
	return (*this) << static_cast<float>(d);
}

__forceinline phys_out_stream& phys_out_stream::operator<< (phys_out_stream_manip::function m)
{
	return (*m)(*this);
}
#endif // INCLUDE_HAVOK_HK_OSTREAM_H
