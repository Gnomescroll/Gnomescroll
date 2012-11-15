#pragma once

#include <t_map/common/types.hpp>

const int ssize = 256;
const int bsize = 65536;
const float RAYCAST_SAMPLING_DENSITY = 100.0f;
const int raycast_tick_max = 1024;

void _ray_cast4(float x0,float y0,float z0, float x1,float y1,float z1, float* interval, struct Vec3* v_out);

void ray_cast_interval(struct Vec3 p, struct Vec3 f, float* interval)
{
    static struct Vec3 out;
    _ray_cast4(p.x, p.y, p.z, f.x, f.y, f.z, interval, &out);
}

int* _ray_cast5(float x0,float y0,float z0, float x1,float y1,float z1, float* interval, int* collision, CubeID* tile);
int* ray_cast5_capped(float x0,float y0,float z0, float x1,float y1,float z1, float* interval, int* collision, CubeID* tile, struct Vec3* v_out);

int _ray_cast6(float x0,float y0,float z0, float _dfx,float _dfy,float _dfz, float max_l, float *distance, int* collision, int* pre_collision, CubeID* tile, int* side);

// args: position and forward vector
int* _nearest_block(float x, float y, float z, float vx, float vy, float vz, float max_distance, int z_low, int z_high);
int* _nearest_block(Vec3 position, Vec3 direction, float max_distance, int z_low, int z_high) { return _nearest_block(position.x, position.y, position.z, direction.x, direction.y, direction.z, max_distance, z_low, z_high); }
int* farthest_empty_block(float x, float y, float z, float vx, float vy, float vz, float max_distance, int z_low, int z_high);
int* farthest_empty_block(float x, float y, float z, float vx, float vy, float vz, int side[3], float max_distance, int z_low, int z_high);
int* farthest_empty_block(struct Vec3 p, struct Vec3 v, int side[3], float max_distance, int z_low, int z_high) { return farthest_empty_block(p.x, p.y, p.z, v.x, v.y, v.z, side, max_distance, z_low, z_high); }

inline float sphere_line_distance(float px, float py, float pz, float ox, float oy, float oz, float tx, float ty, float tz, float* pos, float* _rad2);

// converts int[3] filled by ray_trace6 to a cube_side id
int get_cube_side_from_side_array(int* side);
void get_side_array_from_cube_side(int side_id, int *side);

bool ray_cast_simple(float x, float y, float z, float a, float b, float c);
bool ray_cast_simple(float x, float y, float z, float a, float b, float c, float max_distance);


/* New ray tracer */

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
