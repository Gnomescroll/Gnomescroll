#include "t_map.hpp"

#include "t_vbo.hpp"
#include "t_map_class.hpp"

#ifdef DC_CLIENT
    #include "cache.hpp"
#endif

struct MapDimension map_dim;

namespace t_map
{

Terrain_map* main_map;

void init_t_map()
{
    main_map = new Terrain_map(512, 512); //512 by 512 map
}

#ifdef DC_CLIENT
    void init_for_draw()
    {
        init_cache();
        init_t_vbo();
    }
#endif

int get(int x, int y, int z)
{
    return main_map->get_block(x,y,z);
}

void set(int x, int y, int z, int value)
{
    main_map->set_block(x,y,z,value);
}

}
 

int _get(int x, int y, int z)
{
    return t_map::main_map->get_block(x,y,z);
}

void _set(int x, int y, int z, int value)
{
    t_map::main_map->set_block(x,y,z,value);
}


/*
    Move to utility functions
*/
int _get_highest_open_block(int x, int y)
{
    return 127;
}

int _get_highest_open_block(int x, int y, int agent_height)
{
    return 127;
}