#include "container.hpp"

#include <item/container/_interface.hpp>
#include <item/container/_state.hpp>

#if DC_CLIENT
#include <item/container/container_ui.hpp>
#endif

#if DC_SERVER
#include <item/container/server.hpp>
#include <item/container/net/StoC.hpp>
#endif

namespace ItemContainer
{

/* Initializer */

void init_container(ItemContainerInterface* container)
{
    GS_ASSERT(container != NULL);
    if (container == NULL) return;
    switch (container->type)
    {
        case AGENT_CONTAINER:
            container->attached_to_agent = true;
            container->init(AGENT_CONTAINER_X, AGENT_CONTAINER_Y);
            break;
        case AGENT_TOOLBELT:
            container->attached_to_agent = true;
            container->init(AGENT_TOOLBELT_X, AGENT_TOOLBELT_Y);
            break;
        case AGENT_NANITE:
            container->attached_to_agent = true;
            container->init(AGENT_NANITE_X, AGENT_NANITE_Y);
            break;
        case CONTAINER_TYPE_CRAFTING_BENCH_UTILITY:
            container->attached_to_agent = false;
            container->init(CRAFTING_BENCH_UTILITY_X, CRAFTING_BENCH_UTILITY_Y);
            break;
        case CONTAINER_TYPE_CRYOFREEZER_SMALL:
            container->attached_to_agent = false;
            container->init(CRYOFREEZER_SMALL_X, CRYOFREEZER_SMALL_Y);
            break;
        case CONTAINER_TYPE_SMELTER_ONE:
            container->attached_to_agent = false;
            container->init(SMELTER_ONE_X, SMELTER_ONE_Y);
            break;
        case CONTAINER_TYPE_STORAGE_BLOCK_SMALL:
            container->attached_to_agent = false;
            container->init(STORAGE_BLOCK_SMALL_X, STORAGE_BLOCK_SMALL_Y);
            break;
        default:
            printf("%s - Unhandled container type %d\n", __FUNCTION__, container->type);
            GS_ASSERT(false);
            break;
    }
}

/* ItemContainer methods */

void ItemContainer::insert_item(int slot, ItemID item_id)
{
    GS_ASSERT(item_id != NULL_ITEM);
    GS_ASSERT(this->is_valid_slot(slot));
    //GS_ASSERT(item_id != this->get_item(slot));
    if (!this->is_valid_slot(slot)) return;
    this->slot[slot] = item_id;
    this->slot_count++;

    Item::Item* item = Item::get_item_object(item_id);
    GS_ASSERT(item != NULL);
    if (item == NULL) return;
    item->location = IL_CONTAINER;
    item->location_id = this->id;
    item->container_slot = slot;
}

void ItemContainer::remove_item(int slot)
{
    GS_ASSERT(this->is_valid_slot(slot));
    if (!this->is_valid_slot(slot)) return;

    ItemID item_id = this->slot[slot];
    GS_ASSERT(item_id != NULL_ITEM);
    if (item_id != NULL_ITEM)
    {
        Item::Item* item = Item::get_item_object(this->slot[slot]);
        GS_ASSERT(item != NULL);
        if (item != NULL)
        {
            item->location = IL_NOWHERE;
            item->container_slot = NULL_SLOT;
        }
    }

    this->slot[slot] = NULL_ITEM;
    this->slot_count--;
}

/* Cryofreezer */
void ItemContainerCryofreezer::insert_item(int slot, ItemID item_id)
{
    GS_ASSERT(item_id != NULL_ITEM);
    #if DC_SERVER
    // reset gas decay
    Item::Item* item = Item::get_item(item_id);
    GS_ASSERT(item != NULL);
    if (item != NULL) item->gas_decay = ITEM_GAS_LIFETIME;
    #endif
    ItemContainer::insert_item(slot, item_id);
}

/* Nanite */

void ItemContainerNanite::insert_item(int slot, ItemID item_id)
{
    GS_ASSERT(item_id != NULL_ITEM);
    GS_ASSERT(this->is_valid_slot(slot));
    //GS_ASSERT(item_id != this->get_item(slot));
    if (!this->is_valid_slot(slot)) return;
    this->slot[slot] = item_id;
    this->slot_count++;

    Item::Item* item = Item::get_item_object(item_id);
    GS_ASSERT(item != NULL);
    if (item != NULL)
    {
        item->location = IL_CONTAINER;
        item->location_id = this->id;
        item->container_slot = slot;
    }

    #if DC_SERVER
    if (slot == 0) this->digestion_tick = 0;
    #endif
}

void ItemContainerNanite::remove_item(int slot)
{
    GS_ASSERT(this->is_valid_slot(slot));
    if (!this->is_valid_slot(slot)) return;

    ItemID item_id = this->slot[slot];
    GS_ASSERT(item_id != NULL_ITEM);
    if (item_id != NULL_ITEM)
    {
        Item::Item* item = Item::get_item_object(this->slot[slot]);
        GS_ASSERT(item != NULL);
        if (item != NULL)
        {
            item->location = IL_NOWHERE;
            item->container_slot = NULL_SLOT;
        }
    }

    this->slot[slot] = NULL_ITEM;
    this->slot_count--;
}

#if DC_SERVER
void ItemContainerNanite::digest()
{
    // dont eat if coins are full
    ItemID coins_id = this->get_coins();
    if (coins_id != NULL_ITEM)
    {
        int coins_type = Item::get_item_type(coins_id);
        if (Item::get_stack_size(coins_id) >= Item::get_max_stack_size(coins_type)) return;
    }

    // tick digestion
    this->digestion_tick++;
    if (this->digestion_tick % NANITE_DIGESTION_RATE != 0) return;
    ItemID item_id = this->slot[0];
    if (item_id == NULL_ITEM) return;

    // decrement stack
    Item::Item* item = Item::get_item_object(item_id);
    GS_ASSERT(item != NULL);
    item->stack_size -= 1;
    
    Agent_state* a = STATE::agent_list->get(this->owner);

    // send new food state
    if (item->stack_size <= 0)
    {
        this->remove_item(0);
        if (a != NULL) send_container_remove(a->client_id, this->id, 0);
        Item::destroy_item(item_id);
    }
    else
    {
        if (a != NULL) Item::send_item_state(item->id);
    }

    // update coins
    if (coins_id == NULL_ITEM)
    {   // no coins were in coin slot, create new stack
        Item::Item* coin = Item::create_item((char*)"nanite_coin");
        GS_ASSERT(coin != NULL);
        transfer_free_item_to_container(coin->id, this->id, this->slot_max-1);
    }
    else
    {   // add to existing coin stack
        Item::Item* coins = Item::get_item_object(coins_id);
        coins->stack_size += 1;
        if (a != NULL) Item::send_item_state(coins_id);
    }
}
#endif

/* Crafting Bench */

void ItemContainerCraftingBench::insert_item(int slot, ItemID item_id)
{
    GS_ASSERT(item_id != NULL_ITEM);
    GS_ASSERT(this->is_valid_slot(slot));
    //GS_ASSERT(item_id != this->get_item(slot));
    if (!this->is_valid_slot(slot)) return;
    this->slot[slot] = item_id;
    this->slot_count++;

    Item::Item* item = Item::get_item_object(item_id);
    GS_ASSERT(item != NULL);
    if (item == NULL) return;
    item->location = IL_CONTAINER;
    item->location_id = this->id;
    item->container_slot = slot;
}

void ItemContainerCraftingBench::remove_item(int slot)
{
    GS_ASSERT(this->is_valid_slot(slot));
    if (!this->is_valid_slot(slot)) return;

    ItemID item_id = this->slot[slot];
    GS_ASSERT(item_id != NULL_ITEM);
    if (item_id != NULL_ITEM)
    {
        Item::Item* item = Item::get_item_object(this->slot[slot]);
        GS_ASSERT(item != NULL);
        if (item != NULL)
        {
            item->location = IL_NOWHERE;
            item->container_slot = NULL_SLOT;
        }
    }

    this->slot[slot] = NULL_ITEM;
    this->slot_count--;
}


/* Smelter */

void ItemContainerSmelter::insert_item(int slot, ItemID item_id)
{
    GS_ASSERT(item_id != NULL_ITEM);
    GS_ASSERT(this->is_valid_slot(slot));
    if (!this->is_valid_slot(slot)) return;

    this->slot[slot] = item_id;
    this->slot_count++;

    Item::Item* item = Item::get_item_object(item_id);
    GS_ASSERT(item != NULL);
    if (item == NULL) return;
    item->location = IL_CONTAINER;
    item->location_id = this->id;
    item->container_slot = slot;
}

void ItemContainerSmelter::remove_item(int slot)
{
    GS_ASSERT(this->is_valid_slot(slot));
    if (!this->is_valid_slot(slot)) return;

    ItemID item_id = this->slot[slot];
    GS_ASSERT(item_id != NULL_ITEM);
    if (item_id != NULL_ITEM)
    {
        Item::Item* item = Item::get_item_object(this->slot[slot]);
        GS_ASSERT(item != NULL);
        if (item != NULL)
        {
            item->location = IL_NOWHERE;
            item->container_slot = NULL_SLOT;
        }
    }

    this->slot[slot] = NULL_ITEM;
    this->slot_count--;
}

#if DC_SERVER
void ItemContainerSmelter::fill_fuel(int fuel_type)
{
    GS_ASSERT(this->fuel <= 0);
    this->fuel = 100;
    this->fuel_type = fuel_type;
    if (this->owner != NO_AGENT)
    {
        smelter_fuel_StoC msg;
        msg.container_id = this->id;
        msg.fuel = this->fuel;
        msg.sendToClient(this->owner);
    }
}

void ItemContainerSmelter::begin_smelting(int recipe_id)
{
    GS_ASSERT(recipe_id != NULL_CRAFTING_RECIPE);
    GS_ASSERT(this->recipe_id == NULL_CRAFTING_RECIPE);
    GS_ASSERT(this->progress <= 0);
    this->recipe_id = recipe_id;

    // TODO -- get inc amt and ticks between inc
    // Needs dat/properties
    
    int inc = 1;
    this->progress += inc;

    if (this->owner != NO_AGENT)
    {
        smelter_progress_StoC msg;
        msg.container_id = this->id;
        msg.progress = this->progress;
        msg.sendToClient(this->owner);
    }
}

void ItemContainerSmelter::tick_smelting()
{
    // TDO -- inc amt and ticks between inc from dat
    int inc = 1;
    this->progress += inc;
}

void ItemContainerSmelter::reset_smelting()
{
    if (this->progress != 0 && this->owner != NO_AGENT)
    {
        smelter_progress_StoC msg;
        msg.container_id = this->id;
        msg.progress = 0;
        msg.sendToClient(this->owner);
    }
    this->progress = 0;
}
#endif

}   // ItemContainer
