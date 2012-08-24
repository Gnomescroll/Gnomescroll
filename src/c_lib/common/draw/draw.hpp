#pragma once

#if DC_SERVER
dont_include_this_file_on_server
#endif

#include <physics/vec3.hpp>

namespace Draw
{
    
void drawAxialBillboardSprite(
    struct Vec3 position,
    int texture_index, float texture_scale);
    
void drawBillboardSprite(
    struct Vec3 position,
    int texture_index, float texture_scale);
    
void drawColoredMinivox(
    struct Vec3 position, struct Vec3 forward, struct Vec3 right, struct Vec3 normal,
    Color color);

void drawTexturedMinivox(
    struct Vec3 position, struct Vec3 forward, struct Vec3 right, struct Vec3 normal,
    float tx, float ty, float sprite_width);

}   // Draw
