#include "t_vbo_update.hpp"

#include "common_data.hpp"
#include "common_function.hpp"
#include "cache.hpp"

namespace t_map
{

//cache line optimization; minimize size
static const int_fast8_t CI[6*8*3] = {1, 1, 1, 0, 1, 1, -1, 1, 1, -1, 0, 1, -1, -1, 1, 0, -1, 1, 1, -1, 1, 1, 0, 1,
-1, 1, -1, 0, 1, -1, 1, 1, -1, 1, 0, -1, 1, -1, -1, 0, -1, -1, -1, -1, -1, -1, 0, -1,
1, -1, 1, 1, -1, 0, 1, -1, -1, 1, 0, -1, 1, 1, -1, 1, 1, 0, 1, 1, 1, 1, 0, 1,
-1, 1, 1, -1, 1, 0, -1, 1, -1, -1, 0, -1, -1, -1, -1, -1, -1, 0, -1, -1, 1, -1, 0, 1,
1, 1, 1, 1, 1, 0, 1, 1, -1, 0, 1, -1, -1, 1, -1, -1, 1, 0, -1, 1, 1, 0, 1, 1,
-1, -1, 1, -1, -1, 0, -1, -1, -1, 0, -1, -1, 1, -1, -1, 1, -1, 0, 1, -1, 1, 0, -1, 1 };


static inline int calcAdj(int side_1, int side_2, int corner)  __attribute((always_inline));
static inline int _is_occluded(int x,int y,int z, int side_num) __attribute((always_inline));
static inline int _is_occluded_transparent(int x,int y,int z, int side_num, int _tile_id) __attribute((always_inline));
static inline void add_quad2(struct Vertex* v_list, int offset, int x, int y, int z, int side, int tile_id)  __attribute((always_inline));

/*
will be 1 if is adjacent to any side
will be 2 only if both sides are occluded
*/

//const static int occ_array[3] = { 255, 177, 100 };


static inline int calcAdj(int side_1, int side_2, int corner) 
{
    const static int occ_array[3] = { 255, 128, 64 };
    int occ = (side_1 | side_2 | corner) + (side_1 & side_2);
    return occ_array[occ];
}

const static int s_array[18] = {
        0,0,1,  //top
        0,0,-1, //bottom
        1,0,0,  //north
        -1,0,0, //south
        0,1,0,  //west
        0,-1,0, //east
        };

static inline int _is_occluded(int x,int y,int z, int side_num)
{
    int i = 3*side_num;
    x += s_array[i+0];
    y += s_array[i+1];
    z += s_array[i+2];

    return isOccludes(_get(x,y,z));
}

static inline int _is_occluded_transparent(int x,int y,int z, int side_num, int _tile_id) {
    int i;
    i = 3*side_num;
    x += s_array[i+0];
    y += s_array[i+1];
    z += s_array[i+2];

    int tile_id = _get(x,y,z);
    if(tile_id == _tile_id) return 1;
    return isActive(tile_id);
}

//#3D525E

const int _pallet_num = 5;
const char _pallet[3*(_pallet_num+1)] = 
{
    0xa0, 0xa0,0xa0,
    0x3d, 0x52,0x5e,
    0x57, 0x6e,0x62,
    0x6d,0x8e, 0x86,
    0x3d,0x52,0x5e,
    0x94,0xb2,0xbb,
};

#include <c_lib/physics/color_matrix.hpp>


static inline void _set_quad_local_ambient_occlusion(struct Vertex* v_list, int offset, int x, int y, int z, int side)
{
    int i;
    int index;
    int CX[8];
    for(i=0; i<8; i++) 
    {
        index = side*8*3+i*3;
        CX[i] = isOccludes(_get(x+CI[index+0],y+CI[index+1],z+CI[index+2]));
    }

    {
        int occ1, occ2, occ3, occ4;

        occ1 = calcAdj(CX[7], CX[1], CX[0]);
        occ2 = calcAdj(CX[5], CX[7], CX[6]);
        occ3 = calcAdj(CX[1], CX[3], CX[2]);
        occ4 = calcAdj(CX[3], CX[5], CX[4]);

        AOElement _ao;
        _ao.ao[0] = occ1;
        _ao.ao[1] = occ2;
        _ao.ao[2] = occ3;
        _ao.ao[3] = occ4;

        v_list[offset+0].AO = _ao.AO;
        v_list[offset+1].AO = _ao.AO;
        v_list[offset+2].AO = _ao.AO;
        v_list[offset+3].AO = _ao.AO;

        int index = 3*((hash_function4(x, y, z) % _pallet_num)+1) ;

        struct ColorElement _ce;

        _ce.r = _pallet[index+0];
        _ce.g = _pallet[index+1];
        _ce.b = _pallet[index+2];
        _ce.a = 0;

        v_list[offset+0].color = _ce.color;
        v_list[offset+1].color = _ce.color;
        v_list[offset+2].color = _ce.color;
        v_list[offset+3].color = _ce.color;
    }
}



static inline void add_quad2(struct Vertex* v_list, int offset, int x, int y, int z, int side, int tile_id) {
    int i;

    memcpy(&v_list[offset], &quad_cache[tile_id*6*4+4*side], 4*sizeof(struct Vertex)); //id*6*4+4*side+vert_num

    float _x = x;
    float _y = y;
    float _z = z;
    for(i=0; i<=3;i++) {
        v_list[offset+i].x += _x;
        v_list[offset+i].y += _y;
        v_list[offset+i].z += _z;
    }
    _set_quad_local_ambient_occlusion(v_list, offset, x, y, z, side);
}

static const int VERTEX_SLACK = 128;

int update_column_VBO(struct vm_column* column) {
#if 0
    int tile_id, side_num;
    int _x, _y, _z;

    struct VBO* vbo = &column->vbo;
    struct vm_chunk* chunk;
    int i;

    int cube_vertex_count[4] = {0, 0, 0, 0};
    int vertex_count = 0; //clear counter

    //first pass, count quads
    int active;
    int transparency;
    for(i = 0; i < vm_column_max; i++) {
        if(column->chunk[i] == NULL) { continue; }
        //printf("chunk= %i\n", i);
        chunk = column->chunk[i];
        //chunk->vbo_needs_update = 0;
        //printf("1,2,3 = %i, %i, %i \n", 8*chunk->x_off, 8*chunk->y_off, 8*chunk->z_off);
        for(_x = 8*chunk->x_off; _x < 8*chunk->x_off +8 ; _x++) {
        for(_y = 8*chunk->y_off; _y < 8*chunk->y_off +8 ; _y++) {
        for(_z = 8*chunk->z_off; _z < 8*chunk->z_off +8 ; _z++) {
            tile_id = _get(_x,_y,_z);
            //printf("test %i, %i, %i tile= %i\n", _x,_y,_z,tile_id );
            active = isActive(tile_id);
            if(active == 0) continue;

            transparency = isTransparent(tile_id);
            if(active == 1)  
            {
                if(transparency == 0)
                { 
                    for(side_num=0; side_num<6; side_num++) {
                        if(! _is_occluded(_x,_y,_z,side_num)) {
                            //add_quad(_x,_y,_z,side_num,tile_id);
                            vertex_count += 4;
                            cube_vertex_count[0] += 4;
                        }
                    }
                } 
                else
                {
                    //active block that does not occlude
                    for(side_num=0; side_num<6; side_num++) 
                    {
                        if(! _is_occluded_transparent(_x,_y,_z,side_num, tile_id)) 
                        {
                            //add_quad(_x,_y,_z,side_num,tile_id);
                            vertex_count += 4;
                            cube_vertex_count[transparency] += 4;
                        }
                    }
                }
                continue;    
            }

        }}}
    }

    vbo->_v_num[0] = cube_vertex_count[0];
    vbo->_v_num[1] = cube_vertex_count[1];
    vbo->_v_num[2] = cube_vertex_count[2];
    vbo->_v_num[3] = cube_vertex_count[3];

    vbo->_v_offset[0] = 0;
    vbo->_v_offset[1] = cube_vertex_count[0];
    vbo->_v_offset[2] = cube_vertex_count[0]+cube_vertex_count[1];
    vbo->_v_offset[3] = cube_vertex_count[0]+cube_vertex_count[1]+cube_vertex_count[2];

    /*
        Now that quads have been counted, malloc memory
        -malloc is slow, so over allocated and reuse if possible 
    */

    if(vertex_count == 0) {
        vbo->v_num = 0;

        set_flag(column, VBO_loaded, 0);
        set_flag(column, VBO_needs_update, 0);
        set_flag(column, VBO_has_blocks, 0);
        return 0;
    } else {
        set_flag(column, VBO_loaded, 1);
        set_flag(column, VBO_needs_update, 0);
        set_flag(column, VBO_has_blocks, 1);


        vbo->v_num = vertex_count; //total vertices, size of VBO

        if(vertex_count > vbo->v_list_max_size) {
            if(vbo->v_list != NULL) {
                free(vbo->v_list);
                vbo->v_list = NULL;
            }
            vbo->v_list_max_size = VERTEX_SLACK+vertex_count;
            vbo->v_list = (struct Vertex*)malloc(vbo->v_list_max_size*sizeof(struct Vertex)); 
        }
    }

    int _cube_vertex_count[4];
    _cube_vertex_count[0] = vbo->_v_offset[0];
    _cube_vertex_count[1] = vbo->_v_offset[1];
    _cube_vertex_count[2] = vbo->_v_offset[2];
    _cube_vertex_count[3] = vbo->_v_offset[3];

    struct Vertex* v_list2 = vbo->v_list;

    int infinite_texture;
    for(i = 0; i < vm_column_max; i++) {
        if(column->chunk[i] == NULL) { continue; }
        chunk = column->chunk[i];
        for(_x = 8*chunk->x_off; _x < 8*chunk->x_off +8 ; _x++) {
        for(_y = 8*chunk->y_off; _y < 8*chunk->y_off +8 ; _y++) {
        for(_z = 8*chunk->z_off; _z < 8*chunk->z_off +8 ; _z++) {
            tile_id = _get(_x,_y,_z);
            active = isActive(tile_id);
            infinite_texture = getInfiniteTexture(tile_id);
            if(active == 0) continue;
            transparency = isTransparent(tile_id);
            if(active == 1)
            {
                if(transparency == 0)
                { 
                    for(side_num=0; side_num<6; side_num++) {
                        if(! _is_occluded(_x,_y,_z,side_num)) {
                            //printf("count= %i\n",_cube_vertex_count[0]);
                            add_quad2(v_list2, _cube_vertex_count[0], _x,_y,_z,side_num,tile_id);
                            _cube_vertex_count[0] += 4;
                        }
                    }
                } 
                else
                {
                    //active block that does not occlude
                    for(side_num=0; side_num<6; side_num++) 
                    {
                        if(! _is_occluded_transparent(_x,_y,_z,side_num, tile_id)) 
                        {
                            add_quad2(v_list2, _cube_vertex_count[transparency],_x,_y,_z,side_num,tile_id);
                            _cube_vertex_count[transparency] += 4;
                        }
                    }
                }
                continue;
            }
        }}}
    }
    //push to graphics card
    if(vbo->VBO_id == 0)  glGenBuffers(1, &vbo->VBO_id);
    glBindBuffer(GL_ARRAY_BUFFER, vbo->VBO_id);
    glBufferData(GL_ARRAY_BUFFER, vbo->v_list_max_size*sizeof(struct Vertex), NULL, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, vbo->v_list_max_size*sizeof(struct Vertex), vbo->v_list, GL_STATIC_DRAW);
#endif

    return 0;
}


int update_chunks() {
#if 0 
    struct vm_map* m;
    struct vm_column* c;
    int i,j;
    m = _get_map();

    _garbage_collection_counter++;
    if(_garbage_collection_counter >= garbage_collection_frequency) {
        _garbage_collection_counter = 0;
        for(i=0; i<vm_map_dim; i++) {
        for(j=0; j<vm_map_dim;j++) {
            c = &m->column[j*vm_map_dim+i];
            //if VBO is farther than 10 viewing units, delete it
            if (flag_is_true(c, VBO_loaded) && !chunk_render_check(c->x_off, c->y_off, 15)) {
                //printf("unloaded VBO: %i, %i \n", c->x_off, c->y_off);
                delete_vbo(&c->vbo);
                set_flag(c, VBO_loaded, 0);
            }
        }}
    }
    //VBO construction loop

    for(i=0; i<vm_map_dim; i++) 
    for(j=0; j<vm_map_dim;j++) 
    {
        c = &m->column[j*vm_map_dim+i];
        //update or create VBO for chunks within 10 units of viewing distance
        if(chunk_render_check(c->x_off, c->y_off, 10)) 
        {
            if(flag_is_true(c, VBO_has_blocks) && ( flag_is_true(c, VBO_needs_update) || flag_is_false(c, VBO_loaded))) 
            {
                    update_column_VBO(c);
                    return 0;
            }
        }
    }
#endif
    return 0;
}

}