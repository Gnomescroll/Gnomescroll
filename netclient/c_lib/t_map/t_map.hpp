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

const int MAP_WIDTH = 512;
const int MAP_HEIGHT = 512;
   
const int MAP_CHUNK_WIDTH = MAP_WIDTH/16;
const int MAP_CHUNK_HEIGHT = MAP_CHUNK_HEIGHT/16;
 
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
int _apply_damage(int x, int y, int z, int dmg);

int _apply_damage_broadcast(int x, int y, int z, int dmg);

void _set_broadcast(int x, int y, int z, int value)
{
    printf("Warning: deprecated function _set_broadcast\n");
}

void _block_broadcast(int x, int y, int z, int value)
{
    printf("Warning: deprecated function _set_broadcast\n");
}

void send_map_metadata(int client_id);  //Deprecate
//void send_map_metadata();   //Deprecate
void set_map_size(int x, int y, int z); //Deprecate

#include "t_properties.hpp"

int _get_highest_open_block(int x, int y, int agent_height);
int _get_highest_open_block(int x, int y);
int _get_lowest_open_block(int x, int y, int n) ;
int _get_lowest_open_block(int x, int y) ;
int _get_highest_solid_block(int x, int y);
int _get_lowest_solid_block(int x, int y);
bool point_in_map(int x, int y, int z);

int get_height_at(int x, int y)
{
    return 64;
}

int _get_highest_open_block(int x, int y, int n) {
    if (n < 1) {
        printf("WARNING: _get_highest_open_block :: called with n < 1\n");
        return -1;
    }
    if (n==1) return _get_highest_solid_block(x,y) + 1;

    int open=n;
    int block;
    int i;

    for (i=ZMAX-1; i>=0; i--) {
        block = _get(x,y,i);
        if (!isSolid(block)) {
            open++;
        } else {
            if (open >= n) {
                return i+1;
            }
            open = 0;
        }
    }
    if (open >= n) return 0;
    return -1;
}

int _get_highest_open_block(int x, int y) 
{
    return _get_highest_open_block(x,y,1);
}

int _get_highest_solid_block(int x, int y) {

    int i;
    for (i=ZMAX-1; i>=0; i--) {
        if (isSolid(_get(x,y,i))) {
            break;
        }
    }
    return  i;
}

int _get_lowest_open_block(int x, int y, int n) {
    if (n < 1) {
        printf("WARNING: _get_lowest_open_block :: called with n < 1\n");
        return -1;
    }

    int i;
    int block;
    int open=0;
    for (i=0; i<ZMAX; i++) {
        block = _get(x,y,i);
        if (isSolid(block)) {
            open = 0;
        } else {
            open++;
        }
        if (open >= n) return i-open+1;
    }

    return i;
}

int _get_lowest_solid_block(int x, int y) {

    int i;
    for (i=0; i < ZMAX; i++) {
        if (isSolid(_get(x,y,i))) {
            break;
        }
    }
    if (i >= ZMAX) i = -1;  // failure
    return i;
}

inline bool point_in_map(int x, int y, int z)
{
    if (x<0 || x>=map_dim.x || y<0 || y>=map_dim.y || z<0 || z>map_dim.z)
        return false;
    return true;
}
