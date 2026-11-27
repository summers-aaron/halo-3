#ifndef INCLUDE_PHYS_FIXUP_TAG_H
#define INCLUDE_PHYS_FIXUP_TAG_H

class phys_fixup_tag {
public:
    __forceinline phys_fixup_tag() : m_fixup_finishing(0) {}
    int m_fixup_finishing;
};

#endif // INCLUDE_HAVOK_HK_FINISH_LOADED_OBJECT_FLAG_H
