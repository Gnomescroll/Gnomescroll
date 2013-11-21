/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "_interface.hpp"

namespace t_mob
{

void init()
{
    init_sprites();
    init_models();
}

void load_config()
{
    load_sprite_mob_config();
}

void teardown()
{
    teardown_sprites();
    teardown_models();
}

}   // t_mob
