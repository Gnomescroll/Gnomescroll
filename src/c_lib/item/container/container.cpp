#include "container.hpp"

#include <sound/sound.hpp>

#include <item/container/_interface.hpp>
#include <item/container/_state.hpp>
#include <item/container/config/_interface.hpp>

#if DC_CLIENT
# include <item/container/container_ui.hpp>
#endif

#if DC_SERVER
# include <item/container/server.hpp>
# include <item/container/net/StoC.hpp>
# if GS_SERIALIZER
#  include <serializer/uuid.hpp>
# endif
#endif

namespace ItemContainer
{
    
/* Initializer */

void init_container(ItemContainerInterface* container)
{
    GS_ASSERT(container != NULL);
    if (container == NULL) return;
    
    GS_ASSERT(container->type != NULL_CONTAINER_TYPE);
    class ContainerAttributes* attr = get_attr(container->type);
    GS_ASSERT(attr != NULL);
    if (attr == NULL) return;
    GS_ASSERT(attr->loaded);
    
    container->attached_to_agent = attr->attached_to_agent;
    container->set_alt_parameters(attr->alt_xdim, attr->alt_ydim);
    container->init(attr->xdim, attr->ydim);
}

/* Interface */

int ItemContainerInterface::insert_item(int slot, ItemID item_id)  // virtual
{
    IF_ASSERT(!isValid(item_id)) return NULL_SLOT;
    IF_ASSERT(!this->is_valid_slot(slot)) return NULL_SLOT;
    IF_ASSERT(this->slot[slot] != NULL_ITEM) return NULL_SLOT;
    
    Item::Item* item = Item::get_item(item_id);
    IF_ASSERT(item == NULL) return slot;

    item->location = IL_CONTAINER;
    item->location_id = this->id;
    item->container_slot = slot;

    #if DC_SERVER && GS_SERIALIZER
    if (Options::serializer)
    {   // create a uuid now
        if (uuid_is_null(item->uuid))
            uuid_generate(item->uuid);
    }
    #endif

    this->slot[slot] = item_id;
    this->slot_count++;

    return slot;
}

void ItemContainerInterface::remove_item(int slot)  // virtual
{
    IF_ASSERT(!this->is_valid_slot(slot)) return;

    ItemID item_id = this->slot[slot];
    GS_ASSERT(item_id != NULL_ITEM);
    if (item_id != NULL_ITEM)
    {
        Item::Item* item = Item::get_item(this->slot[slot]);
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

/* Hand */

int ItemContainerHand::insert_item(ItemID item_id)
{
    int slot = ItemContainerInterface::insert_item(0, item_id);
    if (slot == NULL_SLOT) return NULL_SLOT;
    class Item::Item* item = Item::get_item(item_id);
    GS_ASSERT(item != NULL);
    if (item == NULL) return NULL_SLOT;
    item->location = IL_HAND;
    item->location_id = this->owner;
    return slot;
}


/* Energy Tanks */

int ItemContainerEnergyTanks::insert_item(int slot, ItemID item_id)
{
    int item_type = Item::get_item_type(item_id);
    GS_ASSERT(item_type == this->energy_tank_type);
    return ItemContainerInterface::insert_item(slot, item_id);
}

/* Cryofreezer */

int ItemContainerCryofreezer::insert_item(int slot, ItemID item_id)
{
    GS_ASSERT(item_id != NULL_ITEM);
    #if DC_SERVER
    // reset gas decay
    Item::Item* item = Item::get_item(item_id);
    GS_ASSERT(item != NULL);
    if (item != NULL) item->gas_decay = Item::get_gas_lifetime(item->type);
    #endif
    return ItemContainer::insert_item(slot, item_id);
}

/* Smelter */

bool ItemContainerSmelter::can_produce_output()
{
    int recipe_id;
    Item::SmeltingRecipe* recipe;
    return this->can_produce_output(&recipe, &recipe_id);
}

bool ItemContainerSmelter::can_produce_output(Item::SmeltingRecipe** pRecipe, int* pRecipe_id)
{
    class Item::SmeltingRecipe* recipe = Item::get_selected_smelting_recipe(this->id);
    int recipe_id = NULL_SMELTING_RECIPE;
    if (recipe != NULL && recipe->available)
    {
        recipe_id = recipe->id;
        GS_ASSERT(recipe_id != NULL_SMELTING_RECIPE);
    }
    *pRecipe = recipe;
    *pRecipe_id = recipe_id;
    return (recipe != NULL && recipe->available && recipe->id != NULL_SMELTING_RECIPE
          && this->can_insert_outputs(recipe->output, recipe->output_stack, recipe->output_num));
}

bool ItemContainerSmelter::can_insert_outputs(int* outputs, int* output_stacks, int n_outputs)
{
    GS_ASSERT(n_outputs > 0);
    if (n_outputs <= 0) return false;
    GS_ASSERT(n_outputs <= alt_xdim*alt_ydim);
    if (n_outputs > alt_xdim*alt_ydim) return false;

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

#if DC_SERVER
void ItemContainerSmelter::burn_fuel()
{
    GS_ASSERT(this->fuel_type != NULL_ITEM_TYPE);
    GS_ASSERT(this->fuel > 0.0f);
    if (this->fuel <= 0.0f) return;
    this->fuel -= this->burn_rate;
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

    if (!this->on)
    {
        int b[3];
        bool found = t_map::get_container_location(this->id, b);
        GS_ASSERT(found);
        if (!found) return;
        struct Vec3 p = vec3_init(b[0], b[1], b[2]);
        p = vec3_add(p, vec3_init(0.5f, 0.5f, 0.5f));
        Sound::broadcast_play_3d_sound("smelter_on", p);

        t_map::smelter_on(this->id);

        this->on = true;
    }
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

    // send smelter off only if we dont expect the recipe to burn another one
    if (this->fuel <= 0.0f || !this->can_produce_output())
    {
        this->on = false;
        t_map::smelter_off(this->id);
    }
}
#endif

}   // ItemContainer
