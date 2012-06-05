#pragma once

#if DC_SERVER
dont_include_this_file_on_server
#endif

#include <physics/vec3.hpp>

void drawAxialBillboardSprite(Vec3 position, int texture_index, float texture_scale);
void drawBillboardSprite(Vec3 position, int texture_index, float texture_scale);
void drawColoredMinivox(Vec3 position, Vec3 forward, Vec3 right, Vec3 normal, Color color);
void drawTexturedMinivox(Vec3 position, Vec3 forward, Vec3 right, Vec3 normal, int sprite_index, float tx, float ty, float pixel_margin);
