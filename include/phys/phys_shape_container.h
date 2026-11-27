#ifndef INCLUDE_PHYS_SHAPE_CONTAINER_H
#define INCLUDE_PHYS_SHAPE_CONTAINER_H

#include "cseries/crt_xdk.h"
#include "phys/phys_fixup_tag.h"
#include "phys/phys_shape_key.h"
class phys_shape;
typedef __declspec(align(16)) char phys_shape_buffer[512];

class phys_shape_container {
public:
    virtual __forceinline ~phys_shape_container() {}
    virtual int get_child_shape_count() const;
    virtual phys_shape_key get_first_key() const = 0;
    virtual phys_shape_key get_next_key(phys_shape_key old_key) const = 0;
    virtual unsigned int get_collision_filter_info(phys_shape_key key) const;
    virtual const phys_shape *get_child_shape(phys_shape_key key, phys_shape_buffer &buffer) const = 0;
    // March 0x827e9c78; collections can disable welding in their override.
    virtual __forceinline bool edge_weld_enabled() const { return true; }
};

// Method set and bodies follow the March 2007 debug build (marchbody class hkSingleShapeContainer): the
// constructor and destructor take and drop the child's reference without a null test; getChildShape is out of line.
class phys_single_shape_container : public phys_shape_container {
public:
    __forceinline void *operator new(size_t, void *p) { return p; }
    __forceinline void operator delete(void *) {}

    inline phys_single_shape_container(const phys_shape *s);
    __forceinline phys_single_shape_container(phys_fixup_tag, const phys_shape *shape) : m_single_child_shape(shape) {}
    __forceinline phys_single_shape_container(phys_fixup_tag) {}
    virtual ~phys_single_shape_container();
    virtual int get_child_shape_count() const { return 1; }
    virtual phys_shape_key get_first_key() const { return 0; }
    virtual phys_shape_key get_next_key(phys_shape_key) const { return PHYS_INVALID_SHAPE_KEY; }
    virtual const phys_shape *get_child_shape(phys_shape_key key, phys_shape_buffer &buffer) const;

    __forceinline const phys_shape *get_single_child() const { return m_single_child_shape; }
    __forceinline const phys_shape *operator->() const { return m_single_child_shape; }

protected:
    const phys_shape *m_single_child_shape;
};

#endif
