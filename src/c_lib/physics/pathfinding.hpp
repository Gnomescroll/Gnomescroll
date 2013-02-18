#pragma once

#include <t_map/common/types.hpp>

namespace Path
{

struct MapPos* get_path(const struct MapPos& start, const struct MapPos& end, size_t& len);

void print_path(const struct MapPos* path, size_t len);

}   // Path
