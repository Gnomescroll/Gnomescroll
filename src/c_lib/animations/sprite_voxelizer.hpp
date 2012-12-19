#pragma once

#include <SDL/texture_sheet_loader.hpp>
#include <physics/mat3.hpp>

namespace Animations
{

void init_sprite_voxelizer();
void teardown_sprite_voxelizer();
void load_sprite_voxelizer();

void draw_voxelized_sprite(int sprite_id, const struct Mat4& rotation_matrix);

bool draw_voxelized_sprite_gl_begin();
void draw_voxelized_sprite_gl_end();

}   // Animations
