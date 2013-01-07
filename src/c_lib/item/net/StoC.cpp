#include "StoC.hpp"

#if DC_CLIENT

#include <item/_state.hpp>
#include <item/container/_interface.hpp>
#include <hud/container/_interface.hpp>
#include <item/toolbelt/_interface.hpp>

namespace Item
{

inline void item_create_StoC::handle()
{
    Item* item = get_item((ItemID)this->id);
    IF_ASSERT(item != NULL) destroy_item((ItemID)this->id);
    item = create_item(this->type, (ItemID)this->id);
    IF_ASSERT(item == NULL) return;
    item->type = type;
    item->durability = durability;
    item->stack_size = stack_size;
}

inline void item_destroy_StoC::handle()
{
    destroy_item((ItemID)id);
}

inline void item_state_StoC::handle()
{
    Item* item = item_list->get((ItemID)id);
    IF_ASSERT(item == NULL) return;
    item->stack_size = stack_size;
    item->durability = durability;

    // update UI
    ItemContainer::update_container_ui_from_state();
}

inline void item_charges_StoC::handle()
{
    Item* item = item_list->get((ItemID)id);
    IF_ASSERT(item == NULL) return;
    item->charges = charges;
    printf("Item %d has %d charges\n", item->id, charges);
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
inline void item_charges_StoC::handle() {}

} // Item

#endif
