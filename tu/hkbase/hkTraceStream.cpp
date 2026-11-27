// hkbase/hkTraceStream.obj: from the records 0x6ffe00 0x6ffe08 0x6fff08 0x6fff78 0x70b598
// @compiler xdk6130
// @flags /TP /O2 /Oc /GF /EHs-c- /GR-
// @address 0x70b598 ??__Ephys_singleton_reg_trace_stream@@YAXXZ
// @address 0x6ffe00 ?skip_printf@phys_trace_stream@@QAAXPBD0ZZ
// @address 0x6ffe08 ?printf@phys_trace_stream@@QAAXPBD0ZZ
// @address 0x6fff08 ?phys_trace_stream_create@@YAPAVphys_ref_object@@XZ
// @address 0x6fff78 ??_Gphys_trace_stream@@UAAPAXI@Z
#define PHYS_CONFIG_SIMD 1

#include "phys/phys_base.h"

#include "phys/phys_trace_stream.h"
#include <stdarg.h>

phys_ref_object* PHYS_CALL phys_trace_stream_create() { return new phys_trace_stream(); }
// @address 0x7cf598 ?s_instance@?$phys_singleton@Vphys_trace_stream@@@@2PAVphys_trace_stream@@A
template <> phys_trace_stream *phys_singleton<phys_trace_stream>::s_instance = 0;
// @address 0x72c418 phys_singleton_reg_trace_stream
static phys_singleton_init_node phys_singleton_reg_trace_stream(phys_trace_stream_create, (void **)&phys_trace_stream::s_instance);

static const int PHYS_TRACE_STREAM_MAX_STRING_SIZE = 1024;

void phys_trace_stream::printf( const char* title, const char* format_str, ... )
{
	
	{
		for (int i = 0; i < m_trace_titles.getSize(); i++)
		{
			if (0 == phys_string::str_cmp(m_trace_titles[i].m_value, title))
			{
				
				return;
			}
		}
	}

	m_counter++;
	if ( m_counter == 2051)
	{
		
		m_counter *= 1;
	}

	if ( m_stream )
	{
		char buf[PHYS_TRACE_STREAM_MAX_STRING_SIZE];
		
		va_list args; 
		va_start(args, format_str);

		if ( format_str[0] == '#')
		{
			phys_string::sprintf( buf, "%4i\t\t", m_counter );
			m_stream->write(buf, phys_string::str_len( buf ) );
			phys_string::vsnprintf(buf, PHYS_TRACE_STREAM_MAX_STRING_SIZE, format_str+1, args);
		}
		else
		{
			phys_string::vsnprintf(buf, PHYS_TRACE_STREAM_MAX_STRING_SIZE, format_str, args);
		}
		va_end(args);
		m_stream->write(buf, phys_string::str_len( buf ) );
	}
}

void phys_trace_stream::skip_printf( const char* title, const char* format_str, ... )
{
}


