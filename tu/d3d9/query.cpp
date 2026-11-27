// d3d9/query.obj: from the records 0x18f138
// @flags /TP /O2 /Oc
#include <ppcintrinsics.h>
#include "d3d9/device.h"

// @address 0x18f138 d3d_device_issue_occlusion_query
unsigned long *d3d_device_issue_occlusion_query(D3D::CDevice *device, int index, int flag)
{
    unsigned long *p;
    unsigned long *result;
    unsigned long long bit;
    unsigned long long *pending;
    int word = (((flag << 6) & 64) | (index & 63)) << 1 | 1;
    unsigned char n = device->query_write_index + 1;
    device->query_write_index = n;
    device->queued_queries[n] = (flag << 6) | index;
    p = device->m_pRing;
    if (p > device->m_pRingGuarantee)
        p = device->KickOff();
    device->m_TessellatorPacket.VizQuery.dword = word;
    __storewordupdate(0x2293, 4, p);
    __storewordupdate(word, 4, p);
    result = p;
    bit = 1ULL << (unsigned int)index;
    device->m_Pending.m_Mask[3] &= ~(1ULL << 35);
    pending = &device->pending_queries;
    if ((*pending & bit) == 0) {
        __storewordupdate(0xC0002300, 4, p);
        __storewordupdate(index & 63, 4, p);
        *pending |= bit;
    }
    device->m_pRing = p;
    return result;
}
