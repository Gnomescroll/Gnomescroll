/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once


/*
    Deprecate the Quad Cache
*/

namespace t_map
{

struct Vertex* quad_cache = NULL;
struct Vertex* quad_cache_compatibility = NULL;

void init_cache();
void teardown_cache();

}
