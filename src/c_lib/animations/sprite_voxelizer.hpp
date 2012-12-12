#pragma once

#include <SDL/texture_sheet_loader.hpp>

namespace Animations
{

void init_sprite_voxelizer();
void teardown_sprite_voxelizer();

void load_sprite_voxelizer();

void prep_sprite_voxelizer();
void draw_sprite_voxelizer();

// WARNING: this function does not do fulstrum checks 
void voxelize_sprite(
    const class TextureSheetLoader::TextureSheetLoader* sheet_loader,
    int sprite_index,
    struct Vec3 origin, struct Vec3 forward, struct Vec3 right, struct Vec3 up,
    float scale);
    
}   // Animations
