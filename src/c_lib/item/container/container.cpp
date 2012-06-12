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
void ItemContainerSmelter::burn_fuel()
{
    GS_ASSERT(this->fuel_type != NULL_ITEM_TYPE);
    GS_ASSERT(this->fuel > 0.0f);
    if (this->fuel <= 0.0f) return;
    this->fuel -= this->burn_rate;
    GS_ASSERT(this->fuel >= 0.0f);
    if (this->fuel <= 0.0f)
        this->reset_fuel();
}

void ItemContainerSmelter::fill_fuel(int fuel_type)
{
    GS_ASSERT(this->fuel <= 0.0f);
    this->fuel = 1.0f;
    this->fuel_type = fuel_type;
    this->burn_rate = 1.0f / ((float)Item::get_fuel_burn_rate(fuel_type));
    GS_ASSERT(this->burn_rate > 0.0f);
    if (this->burn_rate <= 0.0f) this->burn_rate = 1.0f/30.0f;
    send_smelter_fuel(this->id);
}

void ItemContainerSmelter::reset_fuel()
{
    this->fuel_type = NULL_ITEM_TYPE;
    this->burn_rate = 1.0f/30.0f;
    if (this->fuel < 0.0f) this->fuel = 0.0f;
    if (this->fuel == 0.0f) return;
    this->fuel = 0.0f;
    send_smelter_fuel(this->id);
}

void ItemContainerSmelter::begin_smelting(int recipe_id)
{
    GS_ASSERT(recipe_id != NULL_SMELTING_RECIPE);
    GS_ASSERT(this->recipe_id == NULL_SMELTING_RECIPE);
    GS_ASSERT(this->progress <= 0.0f);
    this->recipe_id = recipe_id;
    this->progress_rate = 1.0f / ((float)Item::get_smelting_recipe_creation_time(recipe_id));
    GS_ASSERT(this->progress_rate > 0.0f);
    if (this->progress_rate <= 0.0f) this->progress_rate = 1.0f/30.0f;
    this->progress += this->progress_rate;
    send_smelter_progress(this->id);
}

void ItemContainerSmelter::tick_smelting()
{
    GS_ASSERT(this->recipe_id != NULL_SMELTING_RECIPE);
    GS_ASSERT(this->progress > 0.0f); // should have incremented once in begin_smelting()
    this->progress += this->progress_rate;
    if (this->progress > 1.0f) this->progress = 1.0f;
}

void ItemContainerSmelter::reset_smelting()
{
    this->recipe_id = NULL_SMELTING_RECIPE;
    this->progress_rate = 1.0f/30.0f;
    if (this->progress < 0.0f) this->progress = 0.0f;
    if (this->progress == 0.0f) return;
    this->progress = 0.0f;
    send_smelter_progress(this->id);
}

bool ItemContainerSmelter::can_insert_outputs(int* outputs, int* output_stacks, int n_outputs)
{
    GS_ASSERT(n_outputs > 0);
    if (n_outputs <= 0) return false;
    GS_ASSERT(n_outputs <= product_xdim*product_ydim);
    if (n_outputs > product_xdim*product_ydim) return false;

    for (int i=0; i<n_outputs; i++)
    {
        GS_ASSERT(outputs[i] != NULL_ITEM_TYPE);
        GS_ASSERT(output_stacks[i] >= 1);
        int slot = this->convert_product_slot(i);
        ItemID slot_item = this->get_item(slot);
        if (slot_item == NULL_ITEM) continue;
        int slot_item_type = Item::get_item_type(slot_item);
        if (outputs[i] != slot_item_type) return false;   // items won't stack
        if (output_stacks[i] >= Item::get_stack_space(slot_item)) return false; // no room to stack
    }
    return true;
}
#endif

}   // ItemContainer
