#pragma once

namespace HudContainer
{

extern GLuint SynthesizerTexture;
extern GLuint CraftingTexture;
extern GLuint StorageBlockTexture;
extern GLuint SmelterTexture;
extern GLuint CrusherTexture;
extern GLuint EquipmentTexture;

void init_texture();
void teardown_texture();

}   // HudContainer
