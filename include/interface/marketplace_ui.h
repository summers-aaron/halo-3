#ifndef INTERFACE_MARKETPLACE_UI_H
#define INTERFACE_MARKETPLACE_UI_H

#if defined(_M_PPCBE) && !defined(__clang__)
extern "C" unsigned long __stdcall XShowMarketplaceUI_thunk(unsigned long user_index, unsigned long entry_point,
    unsigned __int64 offer_id, unsigned long content_categories);
#else
extern "C" unsigned long __stdcall XShowMarketplaceUI(unsigned long user_index, unsigned long entry_point,
    unsigned __int64 offer_id, unsigned long content_categories);
#endif

static __forceinline unsigned long marketplace_show_ui(unsigned long user_index, unsigned long entry_point,
    unsigned __int64 offer_id, unsigned long content_categories)
{
#if defined(_M_PPCBE) && !defined(__clang__)
    return XShowMarketplaceUI_thunk(user_index, entry_point, offer_id, content_categories);
#else
    return XShowMarketplaceUI(user_index, entry_point, offer_id, content_categories);
#endif
}

#endif
