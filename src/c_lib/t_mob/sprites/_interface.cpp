/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "_interface.hpp"

namespace t_mob
{

void init_sprites()
{
    init_sprite_mob_properties();
    init_animation_repo();
    init_sprite_mob_list();
}

void teardown_sprites()
{
    teardown_sprite_mob_list();
    teardown_sprite_mob_properties();
    teardown_animation_repo();
}

}   // t_mob
