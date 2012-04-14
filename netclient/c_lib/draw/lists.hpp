#pragma once

#include <c_lib/objects/components/minivox/list.hpp>

namespace Draw
{

extern ColoredMinivoxList* colored_minivox_list;
extern TexturedMinivoxList* textured_minivox_list;
extern SpriteList* sprite_list;

void init();
void teardown();

} // Draw
