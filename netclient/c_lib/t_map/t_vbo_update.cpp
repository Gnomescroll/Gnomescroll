#include "t_vbo_update.hpp"

#include <c_lib/t_map/glsl/structs.hpp>
#include <c_lib/t_map/glsl/cache.hpp>
#include <c_lib/t_map/glsl/hash_functions.hpp>

#include "t_map_class.hpp"
#include "t_vbo_update.hpp"

#include "t_properties.hpp"

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

const static int_fast8_t s_array[18] = {
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

    return isOccludes( t_map::get(x,y,z) );
}

static inline int _is_occluded_transparent(int x,int y,int z, int side_num, int _tile_id) 
{
    int i;
    i = 3*side_num;
    x += s_array[i+0];
    y += s_array[i+1];
    z += s_array[i+2];

    int tile_id =  t_map::get(x,y,z) ;
    if(tile_id == _tile_id) return 1;
    return isActive(tile_id);
}

#include <c_lib/physics/color_matrix.hpp>


static inline void _set_quad_local_ambient_occlusion(struct Vertex* v_list, int offset, int x, int y, int z, int side)
{
    int i;
    int index;
    int CX[8];
    for(i=0; i<8; i++) 
    {
        index = side*8*3+i*3;
        CX[i] = isOccludes( t_map::get(x+CI[index+0],y+CI[index+1],z+CI[index+2]));
    }

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

}

//#3D525E

const int _pallet_num = 5;
const char _pallet[ 3*(_pallet_num+1) ] = 
{
    0xa0, 0xa0,0xa0,
    0x3d, 0x52,0x5e,
    0x57, 0x6e,0x62,
    0x6d,0x8e, 0x86,
    0x3d,0x52,0x5e,
    0x94,0xb2,0xbb,
};


static inline void _set_quad_color(struct Vertex* v_list, int offset, int x, int y, int z, int side)
{
    int index = 3*((hash_function4(x, y, z) % _pallet_num)+1) ;

    struct ColorElement _ce;

    _ce.r = _pallet[index+0];
    _ce.g = _pallet[index+1];
    _ce.b = _pallet[index+2];
    _ce.a = 0;
    
    for(int i=0 ;i <4; i++)
    {
        v_list[offset+i].r = _ce.r;
        v_list[offset+i].g = _ce.g;
        v_list[offset+i].b = _ce.b;
    }
}

static inline void add_quad2(struct Vertex* v_list, int offset, int x, int y, int z, int side, int tile_id) 
{
    memcpy(&v_list[offset], &quad_cache[tile_id*6*4+4*side], 4*sizeof(struct Vertex)); //id*6*4+4*side+vert_num

    int _x = x & 15;
    int _y = y & 15;

    for(int i=0; i<4;i++) 
    {
        v_list[offset+i].x += _x;
        v_list[offset+i].y += _y;
        v_list[offset+i].z += z;
    }
    _set_quad_local_ambient_occlusion(v_list, offset, x, y, z, side);
    _set_quad_color(v_list, offset, x, y, z, side);
}

static const int VERTEX_SLACK = 128;

//int update_column_VBO(struct vm_column* column) {

void Vbo_map::update_vbo(int i, int j)
{
    int _x, _y, _z;

    class MAP_CHUNK* chunk = map->chunk[j*xchunk_dim + i];  //map chunk
    class Map_vbo* vbo = vbo_array[j*xchunk_dim + i];       //vbo for storing resulting vertices

    if(chunk == NULL) printf("chunk null\n");
    if(vbo == NULL) printf("vbo null\n");

    int vertex_count[2] = {0, 0};

    static struct Vertex* vlist_scratch_0 = (struct Vertex*) malloc(16*16*(TERRAIN_MAP_HEIGHT/2)*4*sizeof(struct Vertex));
    static struct Vertex* vlist_scratch_1 = (struct Vertex*) malloc(16*16*(TERRAIN_MAP_HEIGHT/2)*4*sizeof(struct Vertex));

    /*
        1> Use internals for the chunk instead of map get
        2> Use z indices
        3> memcpy rows
    */
    
    /*
        Now that quads have been counted, malloc memory
        -malloc is slow, so over allocated and reuse if possible 
    */

    for(_z = 0; _z < TERRAIN_MAP_HEIGHT; _z++) {

        for(_x = chunk->xpos; _x < chunk->xpos +16 ; _x++) {
        for(_y = chunk->ypos; _y < chunk->ypos +16 ; _y++) {

            int tile_id = map->get_block(_x,_y,_z);

            if( !isActive(tile_id) ) continue;

            if( !isTransparent(tile_id) )
            { 
                for(int side_num=0; side_num<6; side_num++) {
                    if(! _is_occluded(_x,_y,_z,side_num)) 
                    {
                        add_quad2( vlist_scratch_0, vertex_count[0], _x,_y,_z,side_num,tile_id);
                        vertex_count[0] += 4;
                    }
                }
            } 
            else
            {
                //active block that does not occlude
                for(int side_num=0; side_num<6; side_num++) 
                {
                    if(! _is_occluded_transparent(_x,_y,_z,side_num, tile_id)) 
                    {
                        add_quad2( vlist_scratch_1, vertex_count[1],_x,_y,_z,side_num,tile_id);
                        vertex_count[1] += 4;
                    }
                }
            }

        }}
    }

    int vnum = vertex_count[0] + vertex_count[1];

    vbo->_v_num[0] = vertex_count[0];
    vbo->_v_num[1] = vertex_count[1];

    vbo->_v_offset[0] = 0;
    vbo->_v_offset[1] = vertex_count[0];



    if(vnum == 0) 
    {
        vbo->vnum = 0;
        if(vbo->vbo_id != 0) glDeleteBuffers(1, &vbo->vbo_id);
        return;
    } 
    else 
    {
        vbo->vnum = vnum; //total vertices, size of VBO
        if( vnum > vbo->vnum_max ) vbo->resize(vnum);
    }

    /*
        Memcpy from buffer into vlist
    */
    memcpy(vbo->v_list, vlist_scratch_0, vertex_count[0]*sizeof(struct Vertex));
    memcpy(vbo->v_list+vertex_count[0], vlist_scratch_1, vertex_count[1]*sizeof(struct Vertex));

    //push to graphics card
    if(vbo->vbo_id == 0)  glGenBuffers(1, &vbo->vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, vbo->vbo_id);
    glBufferData(GL_ARRAY_BUFFER, vbo->vnum*sizeof(struct Vertex), NULL, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, vbo->vnum*sizeof(struct Vertex), vbo->v_list, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //printf("id= %i vnun= %i \n", vbo->vbo_id, vbo->vnum);
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