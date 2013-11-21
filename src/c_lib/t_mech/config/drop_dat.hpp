/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#if DC_CLIENT
# error Do not include this file in the client
#endif

#include <item/config/item_drop_alt.hpp>

namespace t_mech
{

extern class Item::ItemDropConfig* drop_dat;

void init_drop_dat();
void teardown_drop_dat();
void load_drop_dat();

void handle_drop(const Vec3i& position, int type);

}   // t_mech
