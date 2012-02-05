#include "t_properties.hpp"

namespace t_map
{

struct cubeProperties cube_list[max_cubes];
short cube_side_texture_array[max_cubes*6];

void set_cube_side_texture(int id, int side, int tex_id) 
{
    if(tex_id < 0 ) return;
    if(tex_id > MAX_TEXTURES)
    {
        printf("_set_cube_side_texture: error, tex id would exceed MAX_TEXTURES: id= %i, side=%i, tex_id= %i\n", id, side, tex_id);
        return;
    }
    if(6*id + side > max_cubes*6)
    {
        printf("_set_cube_side_texture: error, would overflow array: id= %i, side=%i, tex_id= %i\n", id, side, tex_id);
        return;
    }
    cube_side_texture_array[6*id +side] = tex_id;
}

int _get_cube_side_texture(int id, int side) { return cube_side_texture_array[6*id +side]; }
struct cubeProperties* _get_cube(int id)  { return &cube_list[id]; }

}