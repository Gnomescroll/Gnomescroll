#pragma once

namespace t_map
{

inline int get_highest_open_block(int x, int y, int clearance);
inline int get_highest_open_block(int x, int y);
inline int get_lowest_open_block(int x, int y, int clearance);
inline int get_highest_solid_block(int x, int y);
inline int get_highest_solid_block(int x, int y, int z);
inline int get_lowest_solid_block(int x, int y);
inline int get_solid_block_below(int x, int y, int z);
inline int get_open_block_below(int x, int y, int z);

inline size_t get_nearest_surface_blocks(const struct Vec3i& pos, int clearance,
                                        int* out, size_t outlen);
inline int get_nearest_surface_block(const struct Vec3i& pos);
inline int get_nearest_surface_block(const struct Vec3i& pos, int clearance);
inline int get_nearest_surface_block(int x, int y, int z);
inline int get_nearest_surface_block(int x, int y, int z, int clearance);

// _below returns <= pos.z, _above returns > pos.z
inline int get_nearest_surface_block_below(const struct Vec3i& pos);
inline int get_nearest_surface_block_below(const struct Vec3i& pos, int clearance);
inline int get_nearest_surface_block_below(const struct Vec3i& pos, int clearance, int max);
inline int get_nearest_surface_block_above(const struct Vec3i& pos);
inline int get_nearest_surface_block_above(const struct Vec3i& pos, int clearance);
inline int get_nearest_surface_block_above(const struct Vec3i& pos, int clearance, int max);

inline bool is_surface_block(int x, int y, int z);
inline bool is_surface_block(const struct Vec3i& pos);

ALWAYS_INLINE bool position_is_loaded(int x, int y);
bool block_can_be_placed(int x, int y, int z, CubeType cube_type);

}   // t_map
