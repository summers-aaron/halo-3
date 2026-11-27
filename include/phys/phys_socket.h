#ifndef INCLUDE_PHYS_SOCKET_H
#define INCLUDE_PHYS_SOCKET_H
#include "phys/phys_reader.h"
#include "phys/phys_writer.h"
class phys_socket : public phys_ref_object
{
	public:

		enum socket_events {
			NET_SOCKET_NO_EVENTS = 0, 
			NET_SOCKET_READABLE = 1, 
			NET_SOCKET_WRITABLE = 2, 
			NET_SOCKET_CONNECTED = 4, 
			NET_SOCKET_CLOSED = 8, 
			NET_SOCKET_ALL_EVENTS = 0xff 
		};

		static phys_socket* (PHYS_CALL *create)();
		
		virtual phys_bool is_stream_ok() const = 0;

		virtual void close() = 0;

		virtual int read( void* buf, int nbytes) = 0;

		virtual int write( const void* buf, int nbytes) = 0;

		virtual phys_result connect( const char* remote, int port) = 0;

		virtual phys_result listen(int port) = 0;

		virtual phys_result async_select(void* socket_notify_handle, unsigned int message, socket_events events) { return PHYS_FAILURE; }

		virtual phys_socket* poll_for_new_client() = 0;

		phys_reader& get_reader() { return m_reader; }

		phys_writer& get_writer() { return m_writer; }

		static phys_bool s_platformNetInitialized;

		static void (PHYS_CALL *s_platformNetInit)(void);

		static void (PHYS_CALL *s_platformNetQuit)(void);

	protected:

		phys_socket();

	private:

		class reader_adapter : public phys_reader
		{
			public:

				reader_adapter() : m_reader_socket(0) { }
				virtual int read( void* buf, int nbytes );
				virtual phys_bool is_stream_ok() const;

				phys_socket* m_reader_socket;
		};

		class socket_writer_adapter : public phys_writer
		{
			public:

				socket_writer_adapter() : m_reader_socket(0) { }
				virtual int write( const void* buf, int nbytes );
				virtual phys_bool is_stream_ok() const;

				phys_socket* m_reader_socket;
		};

		reader_adapter m_reader;
		socket_writer_adapter m_writer;
};

class phys_bsd_socket : public phys_socket
{
	public:
		PHYS_DECLARE_ALLOCATOR( PHYS_MEM_BASE_CLASS );

			typedef unsigned socket_fd_t;

		phys_bsd_socket(socket_fd_t s=socket_fd_t(-1));

		virtual ~phys_bsd_socket();

		virtual phys_bool is_stream_ok() const;

		virtual void close();

		virtual int read( void* buf, int nbytes);

		virtual int write( const void* buf, int nbytes);

		virtual phys_result connect(const char* server_name, int port_number);

		virtual phys_result async_select(void* socket_notify_handle, unsigned int message, socket_events events);

		phys_result listen(int port);
		phys_socket* poll_for_new_client();

	protected:

		phys_result create_socket();

		socket_fd_t m_reader_socket;
};

#endif // INCLUDE_HAVOK_HK_SOCKET_H
