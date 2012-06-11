#pragma once

namespace t_hud
{

//extern SDL_Surface* ItemGridSlotSurface;
//extern GLuint ItemGridSlotTexture;

extern GLuint NaniteTexture;
extern GLuint CraftingTexture;
extern GLuint StorageBlockTexture;
extern GLuint SmelterTexture;

void init_texture();
void teardown_texture();

}
