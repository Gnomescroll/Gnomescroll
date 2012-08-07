#pragma once

namespace t_hud
{

extern GLuint SynthesizerTexture;
extern GLuint CraftingTexture;
extern GLuint StorageBlockTexture;
extern GLuint SmelterTexture;
extern GLuint RecyclerTexture;

void init_texture();
void teardown_texture();

}   // t_hud
