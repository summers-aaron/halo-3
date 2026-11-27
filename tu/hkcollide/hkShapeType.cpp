// hkcollide/hkShapeType.obj: from the records 0x627868 0x6279a0
// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0x627868 ?phys_register_alt_shape_types@@YAXPAVphys_dispatcher@@@Z
// @address 0x6279a0 ?phys_get_shape_type_name@@YAPBDW4phys_shape_type@@@Z

#include "phys/phys_collide.h"
#include "phys/phys_shape_type.h"

#include "phys/phys_dispatcher.h"
#include "phys/phys_collision_proxy.h"
#include "phys/phys_collectors.h"
#include "phys/phys_agent.h"
#include "phys/phys_collision_input.h"

void phys_register_alt_shape_types(phys_dispatcher *dispatcher)
{
    dispatcher->register_alt_shape_type(PHYS_SHAPE_SPHERE, PHYS_SHAPE_CONVEX);
    dispatcher->register_alt_shape_type(PHYS_SHAPE_TRIANGLE, PHYS_SHAPE_CONVEX);
    dispatcher->register_alt_shape_type(PHYS_SHAPE_BOX, PHYS_SHAPE_CONVEX);
    dispatcher->register_alt_shape_type(PHYS_SHAPE_CAPSULE, PHYS_SHAPE_CONVEX);
    dispatcher->register_alt_shape_type(PHYS_SHAPE_CYLINDER, PHYS_SHAPE_CONVEX);
    dispatcher->register_alt_shape_type(PHYS_SHAPE_CONVEX_HULL, PHYS_SHAPE_CONVEX);
    dispatcher->register_alt_shape_type(PHYS_SHAPE_PACKED_HULL, PHYS_SHAPE_CONVEX);
    dispatcher->register_alt_shape_type(PHYS_SHAPE_CONVEX_OFFSET, PHYS_SHAPE_CONVEX);
    dispatcher->register_alt_shape_type(PHYS_SHAPE_CONVEX_XFORM, PHYS_SHAPE_CONVEX);
    dispatcher->register_alt_shape_type(PHYS_SHAPE_CONVEX_PIECE, PHYS_SHAPE_CONVEX);
    dispatcher->register_alt_shape_type(PHYS_SHAPE_TRIANGLE_COMPOUND, PHYS_SHAPE_COMPOUND);
    dispatcher->register_alt_shape_type(PHYS_SHAPE_LIST, PHYS_SHAPE_COMPOUND);
    dispatcher->register_alt_shape_type(PHYS_SHAPE_BVH_TREE, PHYS_SHAPE_BVH);
    dispatcher->register_alt_shape_type(PHYS_SHAPE_BVH_EMBEDDED, PHYS_SHAPE_BVH);
    dispatcher->register_alt_shape_type(PHYS_SHAPE_CONVEX, PHYS_SHAPE_SPHERE_REP);
    dispatcher->register_alt_shape_type(PHYS_SHAPE_PLANE, PHYS_SHAPE_HEIGHTFIELD);
    dispatcher->register_alt_shape_type(PHYS_SHAPE_SAMPLED_HEIGHTFIELD, PHYS_SHAPE_HEIGHTFIELD);
}

const char *phys_get_shape_type_name(phys_shape_type type)
{
    switch (type) {
    case PHYS_SHAPE_ALL: return "HK_SHAPE_ALL";
    case PHYS_SHAPE_CONVEX: return "HK_SHAPE_CONVEX";
    case PHYS_SHAPE_COMPOUND: return "HK_SHAPE_COLLECTION";
    case PHYS_SHAPE_BVH: return "HK_SHAPE_BV_TREE";
    case PHYS_SHAPE_SPHERE: return "HK_SHAPE_SPHERE";
    case PHYS_SHAPE_TRIANGLE: return "HK_SHAPE_TRIANGLE";
    case PHYS_SHAPE_BOX: return "HK_SHAPE_BOX";
    case PHYS_SHAPE_CAPSULE: return "HK_SHAPE_CAPSULE";
    case PHYS_SHAPE_CYLINDER: return "HK_SHAPE_CYLINDER";
    case PHYS_SHAPE_CONVEX_HULL: return "HK_SHAPE_CONVEX_VERTICES";
    case PHYS_SHAPE_PACKED_HULL: return "HK_SHAPE_PACKED_CONVEX_VERTICES";
    case PHYS_SHAPE_CONVEX_PIECE: return "HK_SHAPE_CONVEX_PIECE";
    case PHYS_SHAPE_MULTI_SPHERE: return "HK_SHAPE_MULTI_SPHERE";
    case PHYS_SHAPE_LIST: return "HK_SHAPE_LIST";
    case PHYS_SHAPE_BVH_EMBEDDED: return "HK_SHAPE_MOPP_EMBEDDED";
    case PHYS_SHAPE_CONVEX_LIST: return "HK_SHAPE_CONVEX_LIST";
    case PHYS_SHAPE_TRIANGLE_COMPOUND: return "HK_SHAPE_TRIANGLE_COLLECTION";
    case PHYS_SHAPE_MULTI_RAY: return "HK_SHAPE_MULTI_RAY";
    case PHYS_SHAPE_HEIGHTFIELD: return "HK_SHAPE_HEIGHT_FIELD";
    case PHYS_SHAPE_SAMPLED_HEIGHTFIELD: return "HK_SHAPE_SAMPLED_HEIGHT_FIELD";
    case PHYS_SHAPE_SPHERE_REP: return "HK_SHAPE_SPHERE_REP";
    case PHYS_SHAPE_TRI_PATCH: return "HK_SHAPE_TRI_PATCH";
    case PHYS_SHAPE_BOUNDING: return "HK_SHAPE_BV";
    case PHYS_SHAPE_PLANE: return "HK_SHAPE_PLANE";
    case PHYS_SHAPE_BVH_TREE: return "HK_SHAPE_MOPP";
    case PHYS_SHAPE_TRANSFORM: return "HK_SHAPE_TRANSFORM";
    case PHYS_SHAPE_CONVEX_OFFSET: return "HK_SHAPE_CONVEX_TRANSLATE";
    case PHYS_SHAPE_CONVEX_XFORM: return "HK_SHAPE_CONVEX_TRANSFORM";
    case PHYS_SHAPE_VOLUME_CALLBACK: return "HK_SHAPE_PHANTOM_CALLBACK";
    case PHYS_SHAPE_USER0: return "HK_SHAPE_USER0";
    case PHYS_SHAPE_USER1: return "HK_SHAPE_USER1";
    case PHYS_SHAPE_USER2: return "HK_SHAPE_USER2";
    default: return "unknown";
    }
}

