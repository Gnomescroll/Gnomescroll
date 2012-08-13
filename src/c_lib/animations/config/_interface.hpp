#pragma once

#include <animations/config/_state.hpp>

namespace Animations
{

void init_config();
void teardown_config();

class AnimationData* get_animation_data(int animation_id);
anim_callback get_animation_callback(int animation_id);
anim_callback get_animation_callback(const char* name);
int get_animation_id(const char* name);

}   // Animations
