#pragma once

namespace t_hud
{

//extern SDL_Surface* ItemSheetSurface;
extern unsigned int ItemSheetTexture;

extern SDL_Surface* ItemGridSlotSurface;
extern unsigned int ItemGridSlotTexture;

extern unsigned int NaniteTexture;

extern unsigned int CraftingTexture;

void init_texture();
void teardown_texture();

}
