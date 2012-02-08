#pragma once


struct MapDimension
{
    int x,y,z;
};

extern struct MapDimension map_dim;

namespace t_map
{
    
void init_t_map();
void init_for_draw();

inline int get(int x, int y, int z);
inline void set(int x, int y, int z, int value);

}

int _get(int x, int y, int z);
void _set(int x, int y, int z, int value);


int _get_highest_open_block(int x, int y, int agent_height);
int _get_highest_open_block(int x, int y);


int _get_lowest_open_block(int x, int y, int n) 
{
    return 0;
}

int _get_lowest_solid_block(int x, int y) 
{
    return 0;
}

int get_height_at(int x, int y) 
{
    return 0;
}

inline bool point_in_map(int x, int y, int z)
{
    return true;
}