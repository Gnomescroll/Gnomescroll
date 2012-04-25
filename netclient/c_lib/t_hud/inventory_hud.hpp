#pragma once

#include <c_lib/t_hud/_interface.hpp>

#include <c_lib/t_hud/texture.hpp>

#include <c_lib/SDL/SDL_functions.h>

namespace t_hud
{

extern InventoryUI* agent_inventory;

void init_agent_inventory_ui();

void draw_agent_inventory();


}
