#pragma once

#include <t_map/common/types.hpp>

inline float sphere_line_distance(float px, float py, float pz, float ox, float oy, float oz, float tx, float ty, float tz, float* pos, float* _rad2);

int get_cube_side_from_side_array(int side[3]);
void get_side_array_from_cube_side(int side_id, int side[3]);

class RaytraceData
{
    public:
        int side;
        float interval; // % travelled
        int collision_point[3];

    RaytraceData() : side(0), interval(0.0f)
    {
        memset(this->collision_point, 0, sizeof(this->collision_point));
    }

    inline const CubeID get_cube_id();
    inline void set_collision_point(int x, int y, int z);
    const void get_pre_collision_point(int pre_collision_point[3]);

    const struct Vec3 collision_normal()
    {
        int sides[3];
        this->get_side_array(sides);
        return vec3_init(sides[0], sides[1], sides[2]);
    }

    const void get_side_array(int arr[3])
    {
        get_side_array_from_cube_side(this->side, arr);
    }
};

// returns true if collided with any block
bool raytrace_terrain(struct Vec3 start, struct Vec3 end, struct RaytraceData* data);
bool raytrace_terrain(struct Vec3 start, struct Vec3 direction, float length, struct RaytraceData* data);
bool raytrace_terrain(struct Vec3 start, struct Vec3 end) { return raytrace_terrain(start, end, NULL); }
bool raytrace_terrain(struct Vec3 start, struct Vec3 direction, float length)  { return raytrace_terrain(start, direction, length, NULL); }
