#ifndef INCLUDE_PHYS_MESH_MATERIAL_H
#define INCLUDE_PHYS_MESH_MATERIAL_H

// One mesh material: the collision filter info of the triangles that use it. Materials sit in a strided
// table (hkMoppEmbeddedShape::m_materialBase / m_materialStriding).
class phys_mesh_material {
public:
    unsigned int m_filter_info;
};

#endif
