#pragma once

#if DC_SERVER
dont_include_this_file_on_server
#endif

#include <physics/vec3.hpp>

namespace Draw
{

void drawAxialBillboardSprite(const Vec3& position, int texture_index,
                              float texture_scale, int sprites_wide=16);

void drawBillboardSprite(const Vec3& position, int texture_index,
                         float texture_scale);

void drawColoredMinivox(const Vec3& position, const Vec3& forward,
                        const Vec3& right, const Vec3& normal, Color color);

void drawTexturedMinivox(const Vec3& position, const Vec3& forward,
                         const Vec3& right, const Vec3& normal,
                         float tx, float ty, float sprite_width);

}   // Draw
