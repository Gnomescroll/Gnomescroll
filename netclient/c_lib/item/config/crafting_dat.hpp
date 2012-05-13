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
    assert(_current_reagent_id < CRAFT_BENCH_INPUTS_MAX);

    int type = dat_get_item_type(item_name);

    // require specifying item,quantity at once
    for (int i=0; i<_current_reagent_id; i++)
        assert(_cr.reagent[i] != type);
    
    // insert reagents sorted by type
    if (_current_reagent_id == 0)
    {   // degenerate case
        _cr.reagent[_current_reagent_id] = type;
        _cr.reagent_count[_current_reagent_id] = quantity;
    }
    else
    {   // keep reagents sorted by type
        int i=0;
        for (; i<_current_reagent_id; i++)
        {
            if (_cr.reagent[i] < type) continue;

            // shift forward
            for (int j=_current_reagent_id; j>i; j--) _cr.reagent[j] = _cr.reagent[j-1];
            for (int j=_current_reagent_id; j>i; j--) _cr.reagent_count[j] = _cr.reagent_count[j-1];
            
            // insert
            _cr.reagent[i] = type;
            _cr.reagent_count[i] = quantity;
            break;
        }
        
        if (i == _current_reagent_id)
        {   // append to end
            _cr.reagent[_current_reagent_id] = type;
            _cr.reagent_count[_current_reagent_id] = quantity;
        }
    }

    _current_reagent_id++;
}

void end_recipe()
{
    _cr.reagent_num = _current_reagent_id;
    crafting_recipe_array[_current_crafting_id] = _cr;
    _cr.init();
    _current_crafting_id++;
    _current_reagent_id = 0;
}

}   // Item

namespace Item
{

void load_crafting_dat()
{

    def_recipe("copper_bar");
    set_reagent("copper_ore", 2);
    end_recipe();

    def_recipe("copper_shovel");
    set_reagent("copper_bar", 2);
    end_recipe();

    def_recipe("food_rock-0");
    set_reagent("regolith", 4);
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

}   // Item
