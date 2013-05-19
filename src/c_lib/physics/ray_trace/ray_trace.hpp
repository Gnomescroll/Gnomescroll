#pragma once

#include <t_map/common/types.hpp>

inline float sphere_line_distance(float px, float py, float pz, float ox, float oy, float oz, float tx, float ty, float tz, float pos[3], float* _rad2);
inline float sphere_line_distance(struct Vec3 p, struct Vec3 o, struct Vec3 t, struct Vec3* out, float* _rad2);

int get_cube_side_from_sides(const Vec3i& sides);
Vec3i get_sides_from_cube_side(int side_id);

class RaytraceData
{
    public:
        int side;
        float interval; // % travelled
        Vec3i collision_point;

    RaytraceData() :
        side(0), interval(0.0f), collision_point(vec3i_init(0))
    {
    }

    inline CubeType get_cube_type();
    inline void set_collision_point(const Vec3i& position);
    Vec3i get_pre_collision_point();

    struct Vec3 collision_normal()
    {
        return vec3_init(this->get_sides());
    }

    Vec3i get_sides()
    {
        return get_sides_from_cube_side(this->side);
    }
};

// returns true if collided with any block
bool raytrace_terrain(const Vec3& start, const Vec3& end, class RaytraceData* data);
bool raytrace_terrain(const Vec3& start, const Vec3& direction, float length, class RaytraceData* data);

bool raytrace_terrain(const Vec3& start, const Vec3& end)
{
    return raytrace_terrain(start, end, NULL);
}

bool raytrace_terrain(const Vec3& start, const Vec3& direction, float length)
{
    return raytrace_terrain(start, direction, length, NULL);
}
