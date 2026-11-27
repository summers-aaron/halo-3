#ifndef INCLUDE_PHYS_KDOP26_H
#define INCLUDE_PHYS_KDOP26_H

class phys_kdop26 {
public:
    __forceinline float get_low_bound(int i) const { return (&m_lo_x)[i * 2]; }
    __forceinline float get_high_bound(int i) const { return (&m_hi_x)[i * 2]; }
    __forceinline void set_dop_low(int i, float l) { (&m_lo_x)[i << 1] = l; }
    __forceinline void set_dop_high(int i, float h) { (&m_hi_x)[i << 1] = h; }
    enum dop_axis { KDOP_AXIS_X = 0, KDOP_AXIS_Y = 1, KDOP_AXIS_Z = 2, KDOP_AXIS_YZ = 3, KDOP_AXIS_YMZ = 4, KDOP_AXIS_XZ = 5,
        KDOP_AXIS_XMZ = 6, KDOP_AXIS_XY = 7, KDOP_AXIS_XMY = 8, KDOP_AXIS_XYZ = 9, KDOP_AXIS_XYMZ = 10, KDOP_AXIS_XMYZ = 11, KDOP_AXIS_XMYMZ = 12 };
    float m_lo_x, m_hi_x;
    float m_lo_y, m_hi_y;
    float m_lo_z, m_hi_z;
    float m_lo_yz, m_hi_yz;
    float m_lo_y_neg_z, m_hi_ymz;
    float m_lo_xz, m_hi_xz;
    float m_lo_x_neg_z, m_hi_xmz;
    float m_lo_xy, m_hi_xy;
    float m_lo_x_neg_y, m_hi_xmy;
    float m_lo_xyz, m_hi_xyz;
    float m_lo_xy_neg_z, m_hi_xymz;
    float m_lo_x_neg_y_z, m_hi_xmyz;
    float m_lo_x_neg_y_neg_z, m_hi_xmymz;
};

typedef char havok_size_hk26Dop[(sizeof(phys_kdop26) == 104) ? 1 : -1];

#endif // HAVOK_HK_26_DOP_H
