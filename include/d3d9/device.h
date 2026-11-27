#ifndef D3D9_DEVICE_H
#define D3D9_DEVICE_H

#include <xtl.h>
#include <stddef.h>

namespace D3D
{
// The private device extends the SDK's public register/ring state.
// Query state at 0x2f08..0x2f51 is read and written by query.obj. The
// intervening private device state is still opaque, as in the original unit.
class CDevice : public D3DDevice
{
public:
    unsigned char private_device_state[0x488];
    unsigned __int64 pending_queries;
    unsigned char queued_queries[64];
    unsigned char query_read_index;
    unsigned char query_write_index;

    unsigned long *KickOff();
};
}

extern "C" unsigned long *d3d_device_issue_occlusion_query(D3D::CDevice *device, int index, int flag);

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char d3d9_public_device_size[(sizeof(void *) != 4 || sizeof(D3DDevice) == 0x2a80) ? 1 : -1];
typedef char d3d9_query_register_offset[(sizeof(void *) != 4 || offsetof(D3DDevice, m_TessellatorPacket.VizQuery) == 0x29b0) ? 1 : -1];
typedef char d3d9_query_pending_offset[(sizeof(void *) != 4 || offsetof(D3D::CDevice, pending_queries) == 0x2f08) ? 1 : -1];
typedef char d3d9_query_write_offset[(sizeof(void *) != 4 || offsetof(D3D::CDevice, query_write_index) == 0x2f51) ? 1 : -1];
#endif

#endif // D3D9_DEVICE_H
