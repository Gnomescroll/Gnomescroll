#include "_interface.hpp"

#include <c_lib/t_hud/item_grid.hpp>

namespace t_hud
{

void draw_init()
{
    init_item_grid();
}

void draw_teardown()
{

}

void draw_hud()
{
    if (!input_state.inventory) return;

    static ItemGrid g;
    g.draw(300,300);
}


}
