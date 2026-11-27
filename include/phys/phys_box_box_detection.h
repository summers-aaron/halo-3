#ifndef INCLUDE_PHYS_BOX_BOX_DETECTION_H
#define INCLUDE_PHYS_BOX_BOX_DETECTION_H

// Functions defined in tu/hkinternal/hkBoxBoxCollisionDetection.cpp.

float normal_sign_from_features(unsigned short feature_bits);

// Index modulo three for the box axes (i % 3 for i in 0..8).
extern const int phys_box_box_util_mods[9];

#endif // HAVOK_HK_BOX_BOX_COLLISION_DETECTION_H
