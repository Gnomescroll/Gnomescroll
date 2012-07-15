#include "StoC.hpp"

#if DC_CLIENT

#include <item/_state.hpp>
#include <item/container/_interface.hpp>
#include <t_hud/_interface.hpp>
#include <item/toolbelt/_interface.hpp>

namespace Item
{

inline void item_create_StoC::handle()
{
    Item* item = item_list->get_or_create_type(type, (ItemID)id);
    GS_ASSERT(item != NULL);
    if (item == NULL) return;
    item->durability = durability;
    item->stack_size = stack_size;
}

inline void item_destroy_StoC::handle()
{
    item_list->destroy(id);
}

inline void item_state_StoC::handle()
{
    Item* item = item_list->get((ItemID)id);
    GS_ASSERT(item != NULL);
    if (item == NULL) return;
    item->stack_size = stack_size;
    item->durability = durability;

    // update UI
    ItemContainer::update_container_ui_from_state();
}

} // Item
#endif

#if DC_SERVER

namespace Item
{

// dummies
inline void item_create_StoC::handle() {}
inline void item_destroy_StoC::handle() {}
inline void item_state_StoC::handle() {}

} // Item

#endif
