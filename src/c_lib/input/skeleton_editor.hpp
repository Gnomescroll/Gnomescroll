#pragma once

namespace SkeletonEditor
{

void key_down_handler(SDL_Event* event);
void key_up_handler(SDL_Event* event);
void mouse_button_down_handler(SDL_Event* event);
void mouse_button_up_handler(SDL_Event* event);
void key_state_handler(Uint8 *keystate, int numkeys);

void save();

}   // SkeletonEditor
