#pragma once

#include <physics/vec3i.hpp>
#include <t_map/common/types.hpp>

namespace Path
{

void init();
void teardown();

struct Vec3i* get_path_2d(const struct Vec3i& start, const struct Vec3i& end, size_t& len);
struct Vec3i* get_path_3d_air(const struct Vec3i& start, const struct Vec3i& end, size_t& len);
struct Vec3i* get_path_3d_surface(const struct Vec3i& start, const struct Vec3i& end, size_t& len);
struct Vec3i* get_path_3d_jump(const struct Vec3i& start, const struct Vec3i& end, size_t& len);

void print_path(const struct Vec3i* path, size_t len);

#if DC_CLIENT
void draw_path(const struct Vec3i* path, size_t len);
#endif

}   // Path
