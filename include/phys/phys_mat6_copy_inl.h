#ifndef INCLUDE_PHYS_MAT6_COPY_INL_H
#define INCLUDE_PHYS_MAT6_COPY_INL_H
#include "phys/phys_mat6_types.h"
inline phys_mat6::phys_mat6(const phys_mat6 &o)
{
    m_blocks[0][0].m_column0 = o.m_blocks[0][0].m_column0;
    m_blocks[0][0].m_column1 = o.m_blocks[0][0].m_column1;
    m_blocks[0][0].m_column2 = o.m_blocks[0][0].m_column2;
    m_blocks[0][1].m_column0 = o.m_blocks[0][1].m_column0;
    m_blocks[0][1].m_column1 = o.m_blocks[0][1].m_column1;
    m_blocks[0][1].m_column2 = o.m_blocks[0][1].m_column2;
    m_blocks[1][0].m_column0 = o.m_blocks[1][0].m_column0;
    m_blocks[1][0].m_column1 = o.m_blocks[1][0].m_column1;
    m_blocks[1][0].m_column2 = o.m_blocks[1][0].m_column2;
    m_blocks[1][1].m_column0 = o.m_blocks[1][1].m_column0;
    m_blocks[1][1].m_column1 = o.m_blocks[1][1].m_column1;
    m_blocks[1][1].m_column2 = o.m_blocks[1][1].m_column2;
}
#endif
