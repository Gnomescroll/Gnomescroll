#include "container_ui.hpp"

#if DC_SERVER
# error Do not include this file in the server
#endif

namespace ItemContainer
{

struct SlotMetadata copy_item_metadata(ItemID item_id)
{
    Item::Item* item = Item::get_item(item_id);
    IF_ASSERT(item == NULL) return NULL_SLOT_METADATA;
    struct SlotMetadata metadata;
    metadata.type = item->type;
    metadata.stack_size = item->stack_size;
    metadata.durability = item->durability;
    metadata.charges = item->charges;
    return metadata;
}

void ItemContainerSmelterUI::tick_fuel()
{
    if (fuel <= 0.0f) return;

    float burn_rate;
    if (this->fuel_type == NULL_ITEM_TYPE) burn_rate = 1.0f/30.0f;
    else burn_rate = 1.0f / ((float)Item::get_fuel_burn_rate(this->fuel_type));
    IF_ASSERT(burn_rate <= 0.0f) burn_rate = 1.0f/30.0f;
    fuel -= burn_rate;
    if (fuel > 1.0f) fuel = 1.0f;
    if (fuel < 0.0f) fuel = 0.0f;
}

void ItemContainerSmelterUI::tick_progress()
{
    if (progress == 0.0f) return;

    class Item::SmeltingRecipe* recipe = Item::get_selected_smelting_recipe(this->id);
    IF_ASSERT(recipe == NULL) return;
    float progress_rate = 1.0f / ((float)Item::get_smelting_recipe_creation_time(recipe->id));
    IF_ASSERT(progress_rate <= 0.0f) progress_rate = 1.0f/30.0f;
    progress += progress_rate;
    if (progress > 1.0f) progress = 1.0f;
    if (progress < 0.0f) progress = 0.0f;
}

/* Equipment */

bool ItemContainerEquipmentUI::can_insert_item(int slot, ItemType item_type)
{
    if (!ItemContainerUIInterface::can_insert_item(slot, item_type))
        return false;
    if (Item::get_item_group_for_type(item_type) != IG_EQUIPMENT)
        return false;
    if (Item::get_item_equipment_type(item_type) != this->slot_equipment_types[slot])
        return false;
    return true;
}

void ItemContainerEquipmentUI::insert_item(int slot, struct SlotMetadata metadata)
{
    GS_ASSERT(Item::get_item_group_for_type(metadata.type) == IG_EQUIPMENT);
    GS_ASSERT(Item::get_item_equipment_type(metadata.type) == this->get_slot_equipment_type(slot));
    ItemContainerUIInterface::insert_item(slot, metadata);
}

} // Item

