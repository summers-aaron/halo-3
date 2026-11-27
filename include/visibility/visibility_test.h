#ifndef VISIBILITY_VISIBILITY_TEST_H
#define VISIBILITY_VISIBILITY_TEST_H

#include <ppcintrinsics.h>
#include <vectorintrinsics.h>

// Tests a sphere against six planes using vector operations.
long visibility_volume_test_sphere_vmx(__vector4 center, __vector4 radius,
    __vector4 plane0, __vector4 plane1, __vector4 plane2, __vector4 plane3, __vector4 plane4, __vector4 plane5);

#endif // VISIBILITY_VISIBILITY_TEST_H
