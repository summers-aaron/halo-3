// hkvisualize/hkDisplayGeometry.obj: from the records 0x6bc5e8 0x6bc6a0
// @compiler xdk6130
// @flags /TP /O2
// @address 0x6bc6a0 ??1phys_display_geometry@@UAA@XZ
// @address 0x6bc5e8 ??_Gphys_geometry@@QAAPAXI@Z
// @address 0x618170 ??1phys_geometry@@QAA@XZ
#include "physics/havok_memory.h"
#include "phys/phys_base.h"


#include "phys/phys_display_geometry.h"
#include "phys/phys_math.h"


phys_display_geometry::~phys_display_geometry()
{
    if (m_geometry)
    {
        delete m_geometry;
        m_geometry = 0;
    }
}

