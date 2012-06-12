#include "container_ui.hpp"

#if DC_SERVER
dont_include_this_file_in_server
#endif

namespace ItemContainer
{

void ItemContainerSmelterUI::tick_fuel()
{
    if (fuel <= 0.0f) return;
    if (this->get_fuel() != NULL_ITEM_TYPE)
    {
        // only set it if the type hasnt gone blank
        // this way its saved even after the last one is remove
        // will need to send the type in a packet later, once multiple fuels are added TODO
        this->burn_rate = 1.0f / ((float)Item::get_fuel_burn_rate(this->get_fuel()));
    }
    GS_ASSERT(this->burn_rate > 0.0f);
    if (this->burn_rate <= 0.0f) this->burn_rate = 1.0f/30.0f;
    fuel -= this->burn_rate;
    if (fuel > 1.0f) fuel = 1.0f;
    if (fuel <= 0.0f)
    {
        fuel = 0.0f;
        this->reset_fuel();
    }
}

void ItemContainerSmelterUI::reset_fuel()
{
    if (fuel < 0.0f) fuel = 0.0f;
    if (fuel <= 0.0f) return;
    this->fuel = 0.0f;
    this->burn_rate = 1.0f/30.0f;
}

void ItemContainerSmelterUI::tick_progress()
{
    if (progress == 0.0f) return;

    class Item::SmeltingRecipe* recipe = Item::get_selected_smelting_recipe(this->id);
    GS_ASSERT(recipe != NULL);
    if (recipe == NULL) return;
    float progress_rate = 1.0f / ((float)Item::get_smelting_recipe_creation_time(recipe->id));
    GS_ASSERT(progress_rate > 0.0f);
    if (progress_rate <= 0.0f) progress_rate = 1.0f/30.0f;
    progress += progress_rate;
    if (progress > 1.0f) progress = 1.0f;
    if (progress < 0.0f) progress = 0.0f;
}

} // Item

