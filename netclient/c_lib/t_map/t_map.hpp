#pragma once


const int XMAX = 512;
const int YMAX = 512;
const int ZMAX = 512;

struct MapDimension
{
    int x,y,z;
};

extern struct MapDimension map_dim;

namespace t_map
{
    
void init_t_map();

#ifdef DC_CLIENT
    void init_for_draw();
#endif

inline int get(int x, int y, int z);
inline void set(int x, int y, int z, int value);

}

int _get(int x, int y, int z);
void _set(int x, int y, int z, int value);

//change to void?
int _apply_damage(int x, int y, int z, int dmg)
{
    return 0;
}

int _get_highest_open_block(int x, int y, int agent_height);
int _get_highest_open_block(int x, int y);


int _get_lowest_open_block(int x, int y, int n) 
{
    return 127;
}

int _get_lowest_open_block(int x, int y) 
{
    return 127;
}

int _get_highest_solid_block(int x, int y)
{
    return 127;
}

int _get_lowest_solid_block(int x, int y) 
{
    return 127;
}

int get_height_at(int x, int y) 
{
    return 127;
}

inline bool point_in_map(int x, int y, int z)
{
    return true;
}


int _apply_damage_broadcast(int x, int y, int z, int dmg);

void _set_broadcast(int x, int y, int z, int value) ;
void _block_broadcast(int x, int y, int z, int value) ;
void send_map_metadata(int client_id);
void send_map_metadata();
void set_map_size(int x, int y, int z);