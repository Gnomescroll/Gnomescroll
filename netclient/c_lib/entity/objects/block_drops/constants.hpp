#pragma once

#include <common/common.hpp>

namespace Objects
{

// pickup
const float BLOCK_DROP_PICKUP_RADIUS = 1.5f;

//health
const float BLOCK_DROP_TTL = 30 * 12;

// physics
const float BLOCK_DROP_MASS = 1.0f;
const float BLOCK_DROP_DAMP = 0.35f;

// voxel size
const float BLOCK_DROP_SIZE = 0.25f;

// voxel animation rotation
//const float BLOCK_DROP_THETA_ROTATION_SPEED = 0.02f;
const float BLOCK_DROP_THETA_ROTATION_SPEED = 0.00f;
const float BLOCK_DROP_PHI_ROTATION_SPEED = 0.0f;

// colored voxel
const struct Color DIRT_BLOCK_DROP_COLOR = { 150, 98, 0 };
const struct Color STONE_BLOCK_DROP_COLOR = { 140, 140, 140 };

// textured voxel
const float BLOCK_DROP_TEXTURE_PIXEL_WIDTH = 4;
// textured voxels for block drops are looked up from t_map/glsl/texture.hpp at runtime

} // Objects
