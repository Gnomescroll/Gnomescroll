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

int get(int x, int y, int z);

void set(int x, int y, int z, int value);

}


int _get_highest_open_block(int x, int y, int agent_height);
int _get_highest_open_block(int x, int y);

static inline int _get(int x, int y, int z)
{
    return t_map::get(x,y,z);
}

static inline void _set(int x, int y, int z, int value)
{
    t_map::set(x,y,z, value);
}