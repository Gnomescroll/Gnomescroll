#include "t_map_class.hpp"

namespace t_map
{
    



    struct MAP_ELEMENT Terrain_map::get_element(int x, int y, int z)
    {
        int xindex = x | 16; //bit mask
        int yindex = y | 16; //bit mask

        int xchunk = (x >> 4);
        int ychunk = (y >> 4);

        return n
    }

    void Terrain_map::set_element(int x, int y, int z, int value)
    {
        
    }

    int Terrain_map::get_block(int x, int y, int z)
    {
        return get_element(x,y,z).block;
    }

    void Terrain_map::set_block(int x, int y, int z, int value)
    {
        set_element(x,y,z, value);
    }

}