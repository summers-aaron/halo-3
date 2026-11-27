// hkbase/hkBsdSocket.obj: from the records 0x610588 0x610590 0x6105b0 0x6105f0 0x610650 0x6106b8 0x610720 0x610738 0x610740 0x610820 0x610880 0x6109c0 0x610c08 0x610c88 0x610e48
// @flags /TP /O2 /Oc
#include "phys/phys_base_types.h"
// @address 0x610588 ?phys_bsd_network_quit@@YAXXZ
// @address 0x610590 ?is_stream_ok@phys_bsd_socket@@UBA?AVphys_bool@@XZ
// @address 0x6105b0 ?close@phys_bsd_socket@@UAAXXZ
// @address 0x6105f0 ?create_socket@phys_bsd_socket@@IAA?AW4phys_result@@XZ
// @address 0x610650 ?read@phys_bsd_socket@@UAAHPAXH@Z
// @address 0x6106b8 ?write@phys_bsd_socket@@UAAHPBXH@Z
// @address 0x610720 ?phys_is_digit@@YA?AVphys_bool@@H@Z
// @address 0x610738 ?async_select@phys_bsd_socket@@UAA?AW4phys_result@@PAXIW4socket_events@phys_socket@@@Z
// @address 0x610740 ?phys_bsd_network_init@@YAXXZ
// @address 0x610820 ??1phys_bsd_socket@@UAA@XZ
// @address 0x610e48 ??_Gphys_bsd_socket@@UAAPAXI@Z
// @address 0x6109c0 ?listen@phys_bsd_socket@@UAA?AW4phys_result@@H@Z
#include <xtl.h>
#include <string.h>
#include "phys/phys_bsd_socket.h"
#include "phys/phys_math.h"
#include "phys/phys_object.h"
#include "phys/phys_socket.h"
#include "phys/phys_error.h"
#include "phys/phys_out_stream.h"
#include "phys/phys_thread_memory.h"
#include <stddef.h>
#include "phys/phys_err_stream.h"
#include "phys/phys_memory_class.h"
#include "phys/phys_statistics_collector.h"
#include "phys/phys_string.h"

extern "C" {
// @address 0x7ce596 g_phys_bsd_network_initialized
signed char g_phys_bsd_network_initialized;
}

void phys_bsd_network_quit()
{
}

phys_bool phys_bsd_socket::is_stream_ok() const
{
  return m_reader_socket != ~0u;
}

void phys_bsd_socket::close()
{
  if (m_reader_socket != ~0u) {
    closesocket(m_reader_socket);
    m_reader_socket = ~0u;
  }
}

phys_result phys_bsd_socket::create_socket()
{
  close();
  m_reader_socket = socket(2, 1, 0);
  if (m_reader_socket == ~0u) {
    return PHYS_FAILURE;
  } else {
    return PHYS_SUCCESS;
  }
}

int phys_bsd_socket::read(void *buffer, int size)
{
  if (m_reader_socket != ~0u) {
    int count = recv(m_reader_socket, (char *)buffer, size, 0);
    if (count > 0 && count != -1) {
      return count;
    } else {
      close();
    }
  }
  return 0;
}

int phys_bsd_socket::write(const void *buffer, int size)
{
  if (m_reader_socket != ~0u) {
    int count = send(m_reader_socket, (const char *)buffer, size, 0);
    if (count > 0 && count != -1) {
      return count;
    } else {
      close();
    }
  }
  return 0;
}

phys_bool phys_is_digit(int character)
{
  return character >= '0' && character <= '9';
}

phys_result phys_bsd_socket::async_select(void *, unsigned int, socket_events)
{
  return PHYS_FAILURE;
}

void phys_bsd_network_init(void)
{
    XNetStartupParams network_startup_parameters;
    char err_msg_buf[512];
    WSADATA wsa_startup_data;
    if (g_phys_bsd_network_initialized == 0) {
        memset(&network_startup_parameters, 0, sizeof(network_startup_parameters));
        network_startup_parameters.cfgSizeOfStruct = sizeof(network_startup_parameters);
        network_startup_parameters.cfgFlags = 1;
        XNetStartup(&network_startup_parameters);
        if (WSAStartup(0x202, &wsa_startup_data) == -1) {
            phys_err_stream err_msg_stream(err_msg_buf, 0x200);
            err_msg_stream << "(Windows)WSAStartup failed with error!";
            phys_error *error_handler = phys_singleton<phys_error>::s_instance;
            error_handler->message(phys_error::MESSAGE_ERROR, 0x321825f8, err_msg_buf,
                ".\\stream\\impl\\hkBsdSocket.cpp", 72);
        }
        g_phys_bsd_network_initialized = 1;
    }
}

phys_bsd_socket::~phys_bsd_socket()
{
  close();
}

phys_result phys_bsd_socket::listen(int port)
{
  if (create_socket() != PHYS_SUCCESS) {
    return PHYS_FAILURE;
  } else {
    sockaddr_in address;
    address.sin_family = 2;
    address.sin_port = (unsigned short)port;
    address.sin_addr.s_addr = 0;
    int allow_addr_reuse = 1;
    setsockopt(m_reader_socket, 0xffff, 4, (const char *)&allow_addr_reuse, 4);
    if (bind(m_reader_socket, (sockaddr *)&address, 16) == -1) {
      close();
      return PHYS_FAILURE;
    } else if (::listen(m_reader_socket, 2) == -1) {
      close();
      return PHYS_FAILURE;
    } else {
      phys_string host;
      XNADDR xbox_net_addr;
      unsigned int status;
      do {
        status = XNetGetTitleXnAddr(&xbox_net_addr);
      } while (status == 0);
      if (status == 1) {
        xbox_net_addr.ina.s_addr = 0;
      }
      if (xbox_net_addr.ina.s_addr != 0) {
        host.printf("%lx", xbox_net_addr.ina.s_addr);
      } else if (xbox_net_addr.inaOnline.s_addr != 0) {
        host.printf("%lx", xbox_net_addr.inaOnline.s_addr);
      } else {
        host = "unknown";
      }
      char buffer[512];
      phys_err_stream stream(buffer, 512);
      stream << "Listening on host[" << host << "] port " << port;
      phys_error *error = phys_singleton<phys_error>::s_instance;
      error->message(phys_error::MESSAGE_REPORT, -1, buffer, ".\\stream\\impl\\hkBsdSocket.cpp", 354);
      phys_result result = PHYS_SUCCESS;
      return result;
    }
  }
}

// @address 0x610c08 ??0phys_bsd_socket@@QAA@I@Z
phys_bsd_socket::phys_bsd_socket(unsigned int socket)
  : m_reader_socket(socket)
{
  if (m_reader_socket == ~0u) {
    create_socket();
  }
}


// @address 0x610880 ?connect@phys_bsd_socket@@UAA?AW4phys_result@@PBDH@Z
phys_result phys_bsd_socket::connect(const char *server, int port)
{
  sockaddr_in address;
  phys_string::mem_fill(&address, 0, 16);
  address.sin_family = 2;
  address.sin_port = (unsigned short)port;
  if (phys_is_digit(server[0])) {
    address.sin_addr.s_addr = inet_addr(server);
  } else {
    char buffer[512];
    phys_err_stream err_msg_stream(buffer, 512);
    err_msg_stream << "Hostname lookup not supported on xbox";
    phys_error *error = phys_singleton<phys_error>::s_instance;
    error->message(phys_error::MESSAGE_ERROR, 0, buffer, ".\\stream\\impl\\hkBsdSocket.cpp", 195);
  }
  if (m_reader_socket == ~0u) {
    close();
    m_reader_socket = socket(2, 1, 0);
    if (m_reader_socket == ~0u) return PHYS_FAILURE;
  }
  if (::connect(m_reader_socket, (sockaddr *)&address, 16) == -1) {
    close();
    return PHYS_FAILURE;
  }
  return PHYS_SUCCESS;
}

// @address 0x610c88 ?poll_for_new_client@phys_bsd_socket@@UAAPAVphys_socket@@XZ
phys_socket *phys_bsd_socket::poll_for_new_client()
{
  if (m_reader_socket != ~0u) {
    fd_set readable_fds;
    readable_fds.fd_count = 0;
    {
      unsigned int i;
      for (i = 0; i < readable_fds.fd_count; i++) {
        if (readable_fds.fd_array[i] == m_reader_socket) {
          break;
        }
      }
      if (i == readable_fds.fd_count) {
        if (readable_fds.fd_count < 64) {
          readable_fds.fd_array[i] = m_reader_socket;
          readable_fds.fd_count++;
        }
      }
    }
    fd_set except_set;
    except_set.fd_count = 0;
    {
      unsigned int i;
      for (i = 0; i < except_set.fd_count; i++) {
        if (except_set.fd_array[i] == m_reader_socket) {
          break;
        }
      }
      if (i == except_set.fd_count) {
        if (except_set.fd_count < 64) {
          except_set.fd_array[i] = m_reader_socket;
          except_set.fd_count++;
        }
      }
    }
    unsigned int highest_fd = m_reader_socket + 1;
    timeval timeout = {0, 0};
    int hits = select(highest_fd, &readable_fds, 0, &except_set, &timeout);
    if (hits > 0 && __WSAFDIsSet(m_reader_socket, &readable_fds) != 0) {
      sockaddr_in from;
      int fromlen = 16;
      unsigned int accepted = (unsigned int)accept(m_reader_socket, (sockaddr *)&from, &fromlen);
      phys_string recv_str;
      recv_str.printf("Socket got connection from [%lx:%d]\n", from.sin_addr.s_addr, from.sin_port);
      {
        char buffer[512];
        phys_err_stream stream(buffer, 512);
        stream << recv_str;
        phys_error *error = phys_singleton<phys_error>::s_instance;
        error->message(phys_error::MESSAGE_REPORT, -1, buffer, ".\\stream\\impl\\hkBsdSocket.cpp", 394);
      }
      if (accepted == ~0u) return 0;
      {
        unsigned int optval = 1;
        setsockopt(accepted, 6, 1, (const char *)&optval, 4);
        return new phys_bsd_socket(accepted);
      }
    }
  }
  return 0;
}
