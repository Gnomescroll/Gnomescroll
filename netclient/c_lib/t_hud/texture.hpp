#pragma once

namespace t_hud

{

extern SDL_Surface* ItemSheetSurface;
extern unsigned int ItemSheetTexture;

extern SDL_Surface* ItemGridSlotSurface;
extern unsigned int ItemGridSlotTexture;

void init_texture();
void tear_down_texture();

}