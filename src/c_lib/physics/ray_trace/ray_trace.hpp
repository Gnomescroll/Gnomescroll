#pragma once

const int ssize = 256;
const int bsize = 65536;
const float RAYCAST_SAMPLING_DENSITY = 100.0f;
const int raycast_tick_max = 1024;

//int* _ray_cast4(float x0,float y0,float z0, float x1,float y1,float z1, float* interval);
void _ray_cast4(float x0,float y0,float z0, float x1,float y1,float z1, float* interval, struct Vec3* v_out);

int* _ray_cast5(float x0,float y0,float z0, float x1,float y1,float z1, float* interval, int* collision, int* tile);
//int* _ray_cast5_capped(float x0,float y0,float z0, float x1,float y1,float z1, float* interval, int* collision, int* tile);
int* _ray_cast5_capped(float x0,float y0,float z0, float x1,float y1,float z1, float* interval, int* collision, int* tile, struct Vec3* v_out);

int _ray_cast6(float x0,float y0,float z0, float _dfx,float _dfy,float _dfz, float max_l, float *distance, int* collision, int* pre_collision, int* tile, int* side);

// args: position and forward vector
int* _nearest_block(float x, float y, float z, float vx, float vy, float vz, float max_distance, int z_low, int z_high);
int* _nearest_block(Vec3 position, Vec3 direction, float max_distance, int z_low, int z_high) { return _nearest_block(position.x, position.y, position.z, direction.x, direction.y, direction.z, max_distance, z_low, z_high); }
int* _farthest_empty_block(float x, float y, float z, float vx, float vy, float vz, float max_distance, int z_low, int z_high);
int* _farthest_empty_block(float x, float y, float z, float vx, float vy, float vz, int side[3], float max_distance, int z_low, int z_high);
int* _farthest_empty_block(Vec3 p, Vec3 v, int side[3], float max_distance, int z_low, int z_high) { return _farthest_empty_block(p.x, p.y, p.z, v.x, v.y, v.z, side, max_distance, z_low, z_high); }

inline float sphere_line_distance(float px, float py, float pz, float ox, float oy, float oz, float tx, float ty, float tz, float* pos, float* _rad2);

// converts int[3] filled by ray_trace6 to a cube_side id
int get_cube_side_from_side_array(int* side);
void get_side_array_from_cube_side(int cube_id);

bool ray_cast_simple(float x, float y, float z, float a, float b, float c);
bool ray_cast_simple(float x, float y, float z, float a, float b, float c, float max_distance);
