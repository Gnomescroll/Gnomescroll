#include "container_ui.hpp"

#if DC_SERVER
dont_include_this_file_in_server
#endif

namespace ItemContainer
{

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

} // Item

