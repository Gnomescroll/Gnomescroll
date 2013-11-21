/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#include <animations/config/_state.hpp>

ALWAYS_INLINE bool isValid(AnimationType type)
{
    return (type >= 0 && type < MAX_ANIMATIONS);
}

namespace Animations
{

void init_config();
void teardown_config();

class AnimationProperty* get_animation_data(AnimationType animation_type);
anim_callback get_animation_callback(AnimationType animation_type);
anim_callback get_animation_callback(const char* name);
AnimationType get_animation_type(const char* name);

}   // Animations
