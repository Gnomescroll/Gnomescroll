#pragma once

namespace t_map
{
    
int init_t_map();
int init_for_draw();

int get(int x, int y, int z);

void set(int x, int y, int z, int value);

}

static inline int _get(int x, int y, int z)
{
    return t_map::get(x,y,z);
}