#pragma once


/*
	Deprecate the Quad Cache
*/
	
namespace t_map
{

struct Vertex* quad_cache = NULL;
struct Vertex* quad_cache_comptability = NULL;

void init_cache();

}