#include "container.hpp"

#include <sound/sound.hpp>
#include <item/container/_interface.hpp>
#include <item/container/_state.hpp>
#include <item/container/config/_interface.hpp>
#include <agent/attributes.hpp>

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

ItemContainerInterface* create_item_container_interface(int type, int id)
{
    containerCreate create_fn = get_container_create_function((ItemContainerType)type);
    IF_ASSERT(create_fn == NULL) return NULL;
    ItemContainerInterface* container = create_fn((ItemContainerType)type, (ItemContainerID)id);
    IF_ASSERT(container == NULL) return NULL;

    class ContainerAttribute* attr = get_attr(container->type);
    IF_ASSERT(attr == NULL)
    {
        destroy_container(container->id);
        return NULL;
    }

    container->attached_to_agent = attr->attached_to_agent;
    container->set_alt_parameters(attr->alt_xdim, attr->alt_ydim);
    container->init(attr->xdim, attr->ydim);

    return container;
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
    IF_ASSERT(item == NULL) return NULL_SLOT;
    item->location = IL_HAND;
    item->location_id = this->owner;
    return slot;
}


/* Energy Tanks */

int ItemContainerEnergyTanks::insert_item(int slot, ItemID item_id)
{
    ItemType item_type = Item::get_item_type(item_id);
    GS_ASSERT(item_type == this->energy_tank_type);
    return ItemContainerInterface::insert_item(slot, item_id);
}

#if DC_SERVER
int ItemContainerEnergyTanks::consume_energy_tanks(int damage)
{   // applies damage to energy tanks,
    // returning amount of damage not consumed
    if (damage <= 0) return damage;
    while (damage > 0)
    {   // consume durability from the weakest energy tanks first
        if (this->slot_count <= 0) break;
        int lowest_dur = MAX_DURABILITY + 1;
        int lowest_slot = NULL_SLOT;
        for (int i=0; i<this->slot_max; i++)
        {
            if (this->slot[i] == NULL_ITEM) continue;
            int dur = Item::get_item_durability(this->slot[i]);
            if (dur <= lowest_dur)
            {
                lowest_dur = dur;
                lowest_slot = i;
            }
        }
        int consume_dur = GS_MIN(damage, lowest_dur);
        int remains = Item::consume_durability(this->slot[lowest_slot], consume_dur, true);
        if (remains)
            Item::send_item_state(this->slot[lowest_slot]);
        damage -= consume_dur;
    }

    return damage;
}
#endif


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

bool ItemContainerSmelter::can_insert_outputs(ItemType* outputs, int* output_stacks, int n_outputs)
{
    IF_ASSERT(n_outputs <= 0) return false;
    IF_ASSERT(n_outputs > alt_xdim*alt_ydim) return false;

    for (int i=0; i<n_outputs; i++)
    {
        GS_ASSERT(outputs[i] != NULL_ITEM_TYPE);
        GS_ASSERT(output_stacks[i] >= 1);
        int slot = this->convert_product_slot(i);
        ItemID slot_item = this->get_item(slot);
        if (slot_item == NULL_ITEM) continue;
        ItemType slot_item_type = Item::get_item_type(slot_item);
        if (outputs[i] != slot_item_type) return false;   // items won't stack
        if (output_stacks[i] >= Item::get_stack_space(slot_item)) return false; // no room to stack
    }
    return true;
}

#if DC_SERVER
void ItemContainerSmelter::burn_fuel()
{
    GS_ASSERT(this->fuel_type != NULL_ITEM_TYPE);
    IF_ASSERT(this->fuel <= 0.0f) return;
    this->fuel -= this->burn_rate;
    if (this->fuel <= 0.0f)
        this->reset_fuel();
}

void ItemContainerSmelter::fill_fuel(ItemType fuel_type)
{
    GS_ASSERT(this->fuel <= 0.0f);
    this->fuel = 1.0f;
    this->fuel_type = fuel_type;
    this->burn_rate = 1.0f / ((float)Item::get_fuel_burn_rate(fuel_type));
    IF_ASSERT(this->burn_rate <= 0.0f) this->burn_rate = 1.0f/30.0f;
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
    IF_ASSERT(this->progress_rate <= 0.0f) this->progress_rate = 1.0f/30.0f;
    this->progress += this->progress_rate;
    send_smelter_progress(this->id);

    if (!this->on)
    {
        int b[3];
        bool found = t_map::get_container_location(this->id, b);
        IF_ASSERT(!found) return;
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

/* Equipment */

bool ItemContainerEquipment::can_insert_item(int slot, ItemID item_id)
{
    if (!ItemContainerInterface::can_insert_item(slot, item_id))
        return false;
    if (Item::get_item_group(item_id) != IG_EQUIPMENT)
        return false;
    if (Item::get_item_equipment_type(item_id) != this->slot_equipment_types[slot])
        return false;
    return true;
}

int ItemContainerEquipment::insert_item(int slot, ItemID item_id)
{
    GS_ASSERT(Item::get_item_group(item_id) == IG_EQUIPMENT);
    GS_ASSERT(Item::get_item_equipment_type(item_id) == this->get_slot_equipment_type(slot));
    return ItemContainerInterface::insert_item(slot, item_id);
}

}   // ItemContainer
