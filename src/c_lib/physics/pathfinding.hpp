#pragma once

#include <t_map/common/types.hpp>

namespace Path
{

inline struct MapPos* get_path_2d(const struct MapPos& start, const struct MapPos& end, size_t& len);
inline struct MapPos* get_path_3d_air(const struct MapPos& start, const struct MapPos& end, size_t& len);
inline struct MapPos* get_path_3d_surface(const struct MapPos& start, const struct MapPos& end, size_t& len);
inline struct MapPos* get_path(const struct MapPos& start, const struct MapPos& end, size_t& len);

void print_path(const struct MapPos* path, size_t len);

#if DC_CLIENT
void draw_path(const struct MapPos* path, size_t len);
#endif

}   // Path
