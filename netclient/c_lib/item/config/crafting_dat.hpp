#include <item/config/_util.hpp>

#include <item/common/struct.hpp>
//#include <t_map/t_properties.hpp>

namespace Item
{

    int _current_crafting_id = 0;
    int _current_reagent_id = 0;

    class CraftingRecipe _cr;

    void def_recipe(const char* item_name)
    {
        _cr.output = dat_get_item_type(item_name);
    }


    void set_reagent(const char* item_name, int quantity)
    {
        _cr.reagant[_current_reagent_id] = dat_get_item_type(item_name);
        _cr.reagant_count[_current_reagent_id] = quantity;
        _current_reagent_id++;
    }

    void end_recipe()
    {
        _cr.reagant_num = _current_reagent_id;
        crafting_recipe_array[_current_crafting_id] = _cr;
        _cr.init();
        _current_crafting_id++;
        _current_reagent_id = 0;
    }

    void end_crafting_dat()
    {

    }

}

namespace Item
{




void load_crafting_dat()
{

    def_recipe("copper_bar");
    set_reagent("copper_ore", 1);
    end_recipe();

    def_recipe("copper_shovel");
    set_reagent("copper_bar", 1);
    set_reagent("copper_bar", 1);
    end_recipe();

    def_recipe("food_rock-0");
    set_reagent("regolith", 1);
    set_reagent("regolith", 1);
    set_reagent("regolith", 1);
    set_reagent("regolith", 1);
    end_recipe();

    def_recipe("mining_laser");
    set_reagent("copper_bar", 1);
    set_reagent("blue_cystal", 1);
    end_recipe();

    def_recipe("laser_rifle");
    set_reagent("copper_bar", 1);
    set_reagent("quartz_crystal", 1);
    end_recipe();



}

}