#include "t_vbo_update.hpp"

#include <t_map/glsl/structs.hpp>
#include <t_map/glsl/cache.hpp>
#include <t_map/glsl/hash_functions.hpp>
#include <t_map/glsl/palette.hpp>
#include <t_map/t_properties.hpp>
#include <t_map/t_map_class.hpp>

#ifdef __MSVC__
# define PUSHED_OPTIMIZATIONS
# pragma optimize( "gt", on )
#endif

#if __GNUC__ && (!__clang__ || (__has_attribute(push_options) && __has_attribute(optimize)))
# define PUSHED_OPTIMIZATIONS
# pragma GCC push_options
//# pragma GCC optimization_level 3
//# pragma GCC optimize 3
# pragma GCC optimize ("O3")
#endif

namespace t_map
{

void init_pallete();

struct SIDE_BUFFER
{
    unsigned short x;
    unsigned short y;
    unsigned short z;
    unsigned short side;
    struct MAP_ELEMENT element;
};

const int SIDE_BUFFER_ARRAY_SIZE = 7;
//int SIDE_BUFFER_INDEX[SIDE_BUFFER_ARRAY_SIZE];   //index
//struct SIDE_BUFFER* SIDE_BUFFER_ARRAY[SIDE_BUFFER_ARRAY_SIZE];
int* SIDE_BUFFER_INDEX = NULL;   //index
struct SIDE_BUFFER** SIDE_BUFFER_ARRAY = NULL;

struct Vertex* vlist_scratch_0 = NULL;
struct Vertex* vlist_scratch_1 = NULL;

//float light_lookup[16]; //how fast light decays
float light_lookup[16]; //lookup in light CLUT textuer

void t_vbo_update_init()
{
    vlist_scratch_0 = (struct Vertex*)calloc(TERRAIN_CHUNK_WIDTH*TERRAIN_CHUNK_WIDTH*(TERRAIN_MAP_HEIGHT/2)*4, sizeof(struct Vertex));
    vlist_scratch_1 = (struct Vertex*)calloc(TERRAIN_CHUNK_WIDTH*TERRAIN_CHUNK_WIDTH*(TERRAIN_MAP_HEIGHT/2)*4, sizeof(struct Vertex));

    SIDE_BUFFER_ARRAY = (struct SIDE_BUFFER**)calloc(SIDE_BUFFER_ARRAY_SIZE, sizeof(struct SIDE_BUFFER*));
    SIDE_BUFFER_INDEX = (int*)calloc(SIDE_BUFFER_ARRAY_SIZE, sizeof(int));

    const int b_size = (1+(TERRAIN_CHUNK_WIDTH*TERRAIN_CHUNK_WIDTH*TERRAIN_MAP_HEIGHT/2));
    for(int i=0; i<SIDE_BUFFER_ARRAY_SIZE; i++) SIDE_BUFFER_ARRAY[i] = (struct SIDE_BUFFER*) calloc(b_size, sizeof(struct SIDE_BUFFER));

    init_pallete();

/*
    for(int i=0; i<16; i++)
    {
        light_lookup[i] = 0.10 + 0.90f* ((float)(i))/15.0;
    }
*/
    for(int i=0; i<16; i++)
    {
        //light_lookup[i] = (1.0/32.0)+((float)(i))/ ((float) (16-1)); //texel aligned
        light_lookup[i] = (1.0/32.0)+ ((float)(i)) / ((float) (16)); //texel aligned
        GS_ASSERT_ABORT(light_lookup[i] > 0.0 && light_lookup[i] < 1.0);
    }
}

void t_vbo_update_end()
{
    free(vlist_scratch_0);
    free(vlist_scratch_1);

    if (SIDE_BUFFER_ARRAY != NULL)
    {
        for(int i=0; i<SIDE_BUFFER_ARRAY_SIZE; i++)
            if (SIDE_BUFFER_ARRAY[i] != NULL)
                free(SIDE_BUFFER_ARRAY[i]);
        free(SIDE_BUFFER_ARRAY);
    }

    if (SIDE_BUFFER_INDEX != NULL)
        free(SIDE_BUFFER_INDEX);
}

//cache line optimization; minimize size
static const int_fast8_t CI[6*8*3] = {1, 1, 1, 0, 1, 1, -1, 1, 1, -1, 0, 1, -1, -1, 1, 0, -1, 1, 1, -1, 1, 1, 0, 1,
-1, 1, -1, 0, 1, -1, 1, 1, -1, 1, 0, -1, 1, -1, -1, 0, -1, -1, -1, -1, -1, -1, 0, -1,
1, -1, 1, 1, -1, 0, 1, -1, -1, 1, 0, -1, 1, 1, -1, 1, 1, 0, 1, 1, 1, 1, 0, 1,
-1, 1, 1, -1, 1, 0, -1, 1, -1, -1, 0, -1, -1, -1, -1, -1, -1, 0, -1, -1, 1, -1, 0, 1,
1, 1, 1, 1, 1, 0, 1, 1, -1, 0, 1, -1, -1, 1, -1, -1, 1, 0, -1, 1, 1, 0, 1, 1,
-1, -1, 1, -1, -1, 0, -1, -1, -1, 0, -1, -1, 1, -1, -1, 1, -1, 0, 1, -1, 1, 0, -1, 1 };

/*
    will be 1 if is adjacent to any side
    will be 2 only if both sides are occluded
*/

//const static int occ_array[3] = { 255, 177, 100 };

//STATIC_INLINE_OPTIMIZED
ALWAYS_INLINE int calcAdj(int side_1, int side_2, int corner) 
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

//this is doing a get, but can use within chunk lookup
ALWAYS_INLINE
int _is_occluded(int x,int y,int z, int side_num)
{
    int i = 3*side_num;
    return isOccludes(x+s_array[i+0],y+s_array[i+1],z+s_array[i+2]);
}

ALWAYS_INLINE
int get_lighting(int x,int y,int z, int side)
{
    int i = 3*side;
    return main_map->get_element(x+s_array[i+0],y+s_array[i+1],z+s_array[i+2]).light;
}

inline int _is_occluded_transparent(int x,int y,int z, int side_num, CubeType _cube_type) 
{
    int i = 3*side_num;
    x += s_array[i+0];
    y += s_array[i+1];
    z += s_array[i+2];

    CubeType cube_type =  t_map::get(x,y,z);
    if (cube_type == _cube_type) return 1;
    return isActive(cube_type);
}

#define AO_DEBUG 0

ALWAYS_INLINE
void _set_quad_local_ambient_occlusion(struct Vertex* v_list, int offset, int x, int y, int z, int side)
{
    int CX[8];
    for(int i=0; i<8; i++) 
    {
        int index = side*8*3+i*3;
        CX[i] = isOccludes(x+CI[index+0],y+CI[index+1],z+CI[index+2]);
    }

#if AO_DEBUG
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
#else

    AOElement _ao;

    _ao.ao[0] = calcAdj(CX[7], CX[1], CX[0]);
    _ao.ao[1] = calcAdj(CX[5], CX[7], CX[6]);
    _ao.ao[2] = calcAdj(CX[1], CX[3], CX[2]);
    _ao.ao[3] = calcAdj(CX[3], CX[5], CX[4]);

    v_list[offset+0].AO = _ao.AO;
    v_list[offset+1].AO = _ao.AO;
    v_list[offset+2].AO = _ao.AO;
    v_list[offset+3].AO = _ao.AO;
#endif

}

ALWAYS_INLINE
void _set_quad_local_ambient_occlusion_compatibility(struct VertexBackup* v_list, int offset, int x, int y, int z, int side)
{
    int i;
    int index;
    int CX[8];
    for(i=0; i<8; i++) 
    {
        index = side*8*3+i*3;
        CX[i] = isOccludes(x+CI[index+0],y+CI[index+1],z+CI[index+2]);
    }

    AOElement _ao;

    _ao.ao[0] = calcAdj(CX[7], CX[1], CX[0]);
    _ao.ao[1] = calcAdj(CX[5], CX[7], CX[6]);
    _ao.ao[2] = calcAdj(CX[1], CX[3], CX[2]);
    _ao.ao[3] = calcAdj(CX[3], CX[5], CX[4]);

    v_list[offset+0].AO = _ao.AO;
    v_list[offset+1].AO = _ao.AO;
    v_list[offset+2].AO = _ao.AO;
    v_list[offset+3].AO = _ao.AO;

}

//#3D525E

/*
const int _pallet_num = 6;
const char _pallet[ 3*(_pallet_num) ] = 
{
    0xa0, 0xa0,0xa0,
    0x3d, 0x52,0x5e,
    0x57, 0x6e,0x62,
    0x6d,0x8e, 0x86,
    0x3d,0x52,0x5e,
    0x94,0xb2,0xbb,
};
*/

const int _pallet_num = 1;
const char _pallet[ 3*(_pallet_num) ] = 
{
    (char)0xFF, (char)0xFF, (char)0xFF
/*
    0x3d, 0x52,0x5e,
    0xa0, 0xa0,0xa0,
*/  
    //0x00, 0x32,0x64,
    //0xFF, 0xFF,0xFF,
    //0xa0, 0x64,0x32,

    //0xFF,0xFF,0xFF,
    //0x57, 0x6e,0x62,
    //0x6d,0x8e, 0x86,
    //0x94,0xb2,0xbb,
};

unsigned char _palletn[ 3*(_pallet_num) ];


void init_pallete()
{
    for(int i=0; i<_pallet_num; i++)
    {
        float r = _pallet[3*i+0];
        float g = _pallet[3*i+1];
        float b = _pallet[3*i+2];

        float avg = ((r+g+b) / 3.0f);
        avg /= 255.0f;

        _palletn[3*i+0] = (unsigned char) (r / avg);
        _palletn[3*i+1] = (unsigned char) (g / avg);
        _palletn[3*i+2] = (unsigned char) (b / avg);
    }
}
 
#if 0
static inline void _set_quad_color_default(struct Vertex* v_list, int offset, int x, int y, int z, int side)
{
    int index = 3*(hash_function4(x, y, z) % _pallet_num) ;

    struct ColorElement _ce;

    _ce.r = _palletn[index+0];
    _ce.g = _palletn[index+1];
    _ce.b = _palletn[index+2];
    _ce.a = 0;

    v_list[offset+0].color = _ce.color;
    v_list[offset+1].color = _ce.color;
    v_list[offset+2].color = _ce.color;
    v_list[offset+3].color = _ce.color;
}


static inline void _set_quad_color_flat(struct Vertex* v_list, int offset, int x, int y, int z, int side)
{
    
    for(int i=0 ;i <4; i++)
    {
        v_list[offset+i].color = 0XFFFFFFFF;
    }
}

static inline void _set_quad_color_perlin(struct Vertex* v_list, int offset, int x, int y, int z, int side)
{

    int index[4];
    //4th element is side+vertex index
    index[0] = 3*(hash_function_perlin(x, y, z, 3*(4*side+0) ) % _pallet_num) ;
    index[1] = 3*(hash_function_perlin(x, y, z, 3*(4*side+1) ) % _pallet_num) ;
    index[2] = 3*(hash_function_perlin(x, y, z, 3*(4*side+2) ) % _pallet_num) ;
    index[3] = 3*(hash_function_perlin(x, y, z, 3*(4*side+3) ) % _pallet_num) ;

    for(int i=0;i <4; i++)
    {
        v_list[offset+i].r = _palletn[index[i]+0];
        v_list[offset+i].g = _palletn[index[i]+1];
        v_list[offset+i].b = _palletn[index[i]+2];
    }

}
#endif

static const unsigned char _0 = 0;
static const unsigned char _1 = 1;

const struct TextureElement texElementArray[4] =
{
    {{{_0,_0,0,0}}},
    {{{_0,_1,0,0}}},
    {{{_1,_1,0,0}}},
    {{{_1,_0,0,0}}}
};


static const struct PositionElement _v_index[4*6] = 
{
    {{{1,1,1,0}}} , {{{0,1,1,0}}} , {{{0,0,1,0}}} , {{{1,0,1,0}}} , //top
    {{{0,1,0,0}}} , {{{1,1,0,0}}} , {{{1,0,0,0}}} , {{{0,0,0,0}}} , //bottom
    {{{1,0,1,0}}} , {{{1,0,0,0}}} , {{{1,1,0,0}}} , {{{1,1,1,0}}} , //north
    {{{0,1,1,0}}} , {{{0,1,0,0}}} , {{{0,0,0,0}}} , {{{0,0,1,0}}} , //south
    {{{1,1,1,0}}} , {{{1,1,0,0}}} , {{{0,1,0,0}}} , {{{0,1,1,0}}} , //west
    {{{0,0,1,0}}} , {{{0,0,0,0}}} , {{{1,0,0,0}}} , {{{1,0,1,0}}}  //east
};

/*
static const struct PositionElement _v_index2[3*6] = 
{
    {{{1,1,1,0}}} , {{{0,1,1,0}}} , {{{0,0,1,0}}} , {{{1,0,1,0}}} , //top
    {{{0,1,0,0}}} , {{{1,1,0,0}}} , {{{1,0,0,0}}} , {{{0,0,0,0}}} , //bottom
    {{{1,0,1,0}}} , {{{1,0,0,0}}} , {{{1,1,0,0}}} , {{{1,1,1,0}}} , //north
    {{{0,1,1,0}}} , {{{0,1,0,0}}} , {{{0,0,0,0}}} , {{{0,0,1,0}}} , //south
    {{{1,1,1,0}}} , {{{1,1,0,0}}} , {{{0,1,0,0}}} , {{{0,1,1,0}}} , //west
    {{{0,0,1,0}}} , {{{0,0,0,0}}} , {{{1,0,0,0}}} , {{{1,0,1,0}}}  //east
};
*/

/*
    Test this!
*/



static const unsigned char _02 = 0;
static const unsigned char _12 = 255;

const struct TextureElement texElementArray2[4] =
{
    {{{_02,_02,0,0}}},
    {{{_02,_12,0,0}}},
    {{{_12,_12,0,0}}},
    {{{_12,_02,0,0}}}
};

static const int VERTEX_SLACK = 32;
static int vertex_max = 0;

//int update_column_VBO(struct vm_column* column) {

#define USE_QUAD_CACHE 0

//ALWAYS_INLINE static void push_quad1(struct Vertex* v_list, int offset, int x, int y, int z, int side, struct MAP_ELEMENT element) __attribute((always_inline));

//OPTIMIZED

//ALWAYS_INLINE
void push_quad1(struct Vertex* v_list, int offset, int x, int y, int z, int side, struct MAP_ELEMENT element) 
{
    int tile_id = element.block;
#if USE_QUAD_CACHE
    memcpy(&v_list[offset], &quad_cache[tile_id*6*4+4*side], 4*sizeof(struct Vertex)); //id*6*4+4*side+vert_num
#else
    //unsigned char tile_tex = (unsigned char) cube_side_texture_array[6*tile_id+side];

    //lower 4 bits are texture palette, upper 4 bits are color palette
    //int _p = element.palette|0x0f;
    //_p = 0;
    
    //unsigned char tile_tex = cube_texture_palette[ 6*( cube_texture_palette_lookup[tile_id] + (element.palette&0x0f) ) + side ];
    unsigned char tile_tex = cube_texture_palette[ 6*( cube_texture_palette_lookup[tile_id] + element.palette ) + side ];


    /*
        DEBUGGING CODE, remove soon
    */
    #if 0
    //int id1 = cube_texture_palette_lookup[tile_id];
    int id2 = cube_texture_palette_lookup[tile_id] + element.palette;
    if(element.palette >= cube_texture_palette_lookup_max[tile_id] )
    {
        //printf("0: block_id= %i id1= %i id2= %i \t", tile_id,id1,id2);
        //printf("base_index= %i pallete_index= %i pallete_lookup_value= %i pallete_lookup_max= %i \n", id1, id2, cube_texture_palette_lookup[tile_id]+element.palette, cube_texture_palette_lookup_max[tile_id]);
    }

    if(id2 < cube_texture_palette_lookup[tile_id] )
    {
        //printf("1: block_id= %i id1= %i id2= %i \t", tile_id,id1,id2);
        //printf("base_index= %i pallete_index= %i pallete_lookup_value= %i pallete_lookup_max= %i \n", id1, id2, cube_texture_palette_lookup[tile_id]+element.palette, cube_texture_palette_lookup_max[tile_id]);
    }
    #endif

    struct TextureElement tex;
    tex.tex = 0;
    tex.tz = tile_tex;

    v_list[offset+0].tex = texElementArray[0].tex | tex.tex;
    v_list[offset+1].tex = texElementArray[1].tex | tex.tex;
    v_list[offset+2].tex = texElementArray[2].tex | tex.tex;
    v_list[offset+3].tex = texElementArray[3].tex | tex.tex;

    v_list[offset+0].pos = _v_index[4*side+0].pos;
    v_list[offset+1].pos = _v_index[4*side+1].pos;
    v_list[offset+2].pos = _v_index[4*side+2].pos;
    v_list[offset+3].pos = _v_index[4*side+3].pos;
#endif

    int _lv = get_lighting(x,y,z,side);

/*
    int _sky_light = _lv & 0x0f;
    int _env_light = (_lv << 4) & 0x0f;

    float sky_light = light_lookup[_sky_light ];
    float env_light = light_lookup[_env_light ];
*/

    float sky_light = light_lookup[ _lv & 0x0f ];
    float env_light = light_lookup[ (_lv >> 4) & 0x0f ];

    v_list[offset+0].lighting[0] = sky_light;
    v_list[offset+1].lighting[0] = sky_light;
    v_list[offset+2].lighting[0] = sky_light;
    v_list[offset+3].lighting[0] = sky_light;

    v_list[offset+0].lighting[1] = env_light;
    v_list[offset+1].lighting[1] = env_light;
    v_list[offset+2].lighting[1] = env_light;
    v_list[offset+3].lighting[1] = env_light;

    {
        int _x = x & 15;
        int _y = y & 15;

        for(int i=0; i<4;i++) 
        {
            v_list[offset+i].x += _x;
            v_list[offset+i].y += _y;
            v_list[offset+i].z += z;
        }
    }
    //_set_quad_local_ambient_occlusion(v_list, offset, x, y, z, side);
    //_set_quad_color(v_list, offset, x, y, z, side);

    for(int i=0 ;i <4; i++)
    {
        v_list[offset+i].color = 0xffffffff;
    }

    //_set_quad_color_flat(v_list, offset, x, y, z, side);
}


void generate_vertex_list(struct Vertex* vlist)
{
    int offset = 0;

    for(int side=0; side<SIDE_BUFFER_ARRAY_SIZE; side++)
    for(int j=0; j<SIDE_BUFFER_INDEX[side]; j++)
    {
        struct SIDE_BUFFER sb = SIDE_BUFFER_ARRAY[side][j];
        struct MAP_ELEMENT element = sb.element;
        push_quad1(vlist, offset, sb.x, sb.y, sb.z, side, element);
        offset += 4;
    }
}

void generate_quad_ao_values(struct Vertex* vlist)
{
    int offset = 0;

    for(int side=0; side<SIDE_BUFFER_ARRAY_SIZE; side++)
    for(int j=0; j<SIDE_BUFFER_INDEX[side]; j++)
    {
        //struct SIDE_BUFFER sb = SIDE_BUFFER_ARRAY[side][j];

        int x = SIDE_BUFFER_ARRAY[side][j].x;
        int y = SIDE_BUFFER_ARRAY[side][j].y;
        int z = SIDE_BUFFER_ARRAY[side][j].z;

        _set_quad_local_ambient_occlusion(vlist, offset, x, y, z, side);

        offset += 4;
    }
}


/*
    BUFFER
*/
//static inline void push_buffer1(unsigned short side, unsigned short x, unsigned short y, unsigned short z, struct MAP_ELEMENT element)  __attribute((always_inline));
//static inline void push_buffer2(unsigned short side , unsigned short x, unsigned short y, unsigned short z, struct MAP_ELEMENT element)  __attribute((always_inline));


//for solid blocks
ALWAYS_INLINE OPTIMIZED
void push_buffer1(unsigned short side, unsigned short x, unsigned short y, unsigned short z, struct MAP_ELEMENT element)
{
    struct SIDE_BUFFER* sb = &SIDE_BUFFER_ARRAY[side][SIDE_BUFFER_INDEX[side]];
    sb->x = x;
    sb->y = y;
    sb->z = z;
    sb->element = element;
    SIDE_BUFFER_INDEX[side]++;
}


//for transparent blocks
ALWAYS_INLINE OPTIMIZED
void push_buffer2(unsigned short side, unsigned short x, unsigned short y, unsigned short z, struct MAP_ELEMENT element)
{
    struct SIDE_BUFFER* sb = &SIDE_BUFFER_ARRAY[6][SIDE_BUFFER_INDEX[side]];
    sb->x = x;
    sb->y = y;
    sb->z = z;
    sb->side = side;
    sb->element = element;
    SIDE_BUFFER_INDEX[6]++;
}


/*
    Two optimizations: incremental updates and by sidesgoogl

*/

/*
    int vertex_num[6];
    int vertex_num_array[6][16];   //for each column
*/

OPTIMIZED
void set_vertex_buffers(class MAP_CHUNK* chunk, class Map_vbo* vbo)
{

    int varray[6][9];

    for(int zi0 = 0; zi0 < 128/16; zi0++) 
    {

        for(int side=0; side<6; side++)
            varray[side][zi0] = 4*SIDE_BUFFER_INDEX[side];

        for(int zi1 = 0; zi1 < 16; zi1++) 
        {

            const int _z = 16*zi0 + zi1;

            for(int x = 0; x<16; x++) {
            for(int y = 0; y<16; y++) {

                int _x = x + chunk->xpos;
                int _y = y + chunk->ypos;

                struct MAP_ELEMENT element = chunk->get_element(_x,_y,_z); //faster
                CubeType cube_type = (CubeType)element.block;

                if( !isActive(cube_type) ) continue;

                if( !isTransparent(cube_type) )
                {
                    //for each side
                    //for(int i=0; i<6; i++) if(! _is_occluded(_x,_y,_z,i)) push_buffer1(i, _x,_y,_z, element);
                    //this is faster than a loop!
                    if(! _is_occluded(_x,_y,_z,0)) push_buffer1(0, _x,_y,_z, element);
                    if(! _is_occluded(_x,_y,_z,1)) push_buffer1(1, _x,_y,_z, element);
                    if(! _is_occluded(_x,_y,_z,2)) push_buffer1(2, _x,_y,_z, element);
                    if(! _is_occluded(_x,_y,_z,3)) push_buffer1(3, _x,_y,_z, element);
                    if(! _is_occluded(_x,_y,_z,4)) push_buffer1(4, _x,_y,_z, element);
                    if(! _is_occluded(_x,_y,_z,5)) push_buffer1(5, _x,_y,_z, element);
                
                } 
                else
                {
                    //active block that does not occlude
                    for(int side_num=0; side_num<6; side_num++) 
                    {
                        if(!_is_occluded_transparent(_x,_y,_z,side_num, (CubeType)cube_type))
                            push_buffer2(side_num, _x,_y,_z, element);
                    }
                }

            }}
        }

        //for(int i=0; i<6; i++) vbo->vertex_num_array[i][zi0] = 4*SIDE_BUFFER_INDEX[i];
    }

        for(int side=0; side<6; side++)
            varray[side][8] = 4*SIDE_BUFFER_INDEX[side];


        //set index for each side
        for(int side=0; side<6; side++)
        {
            vbo->vertex_offset[side] = 0;
            for(int j=0; j<side; j++) 
                vbo->vertex_offset[side] += 4*SIDE_BUFFER_INDEX[j];
        }

        //set voff array

        for(int side=0; side<6; side++)
        {
            for(int j=0; j<9; j++) 
                vbo->voff_array[side][j] = vbo->vertex_offset[side] + varray[side][j];
        }
        //set vertex number
        for(int i=0; i<6; i++) vbo->vertex_num[i] = 4*SIDE_BUFFER_INDEX[i];


/*
    for(int i=0; i<6; i++) vbo->vertex_num[i] = 4*SIDE_BUFFER_INDEX[i];

    for(int i=0; i<6; i++)
    {
        vbo->vertex_offset[i] = 0;
        for(int j=0; j<i; j++) vbo->vertex_offset[i] += 4*SIDE_BUFFER_INDEX[j];
    }



    for(int side=0; side<6; side++)
    {
        int vnum[8+1];
        vnum[0] = vbo->vertex_num_array[side][0];
        vnum[8] = vbo->vertex_num[side] - vbo->vertex_num_array[side][7];
        for(int i=1; i<8; i++)
            vnum[i] = vbo->vertex_num_array[side][i] - vbo->vertex_num_array[side][i-1];


        for(int i=0; i<=8; i++)
             vbo->voff_array[side][i] = vbo->vertex_offset[side];

        for(int i=0; i<=8; i++)
        for(int j=0; j<=i; j++)
        {
            vbo->voff_array[side][i] += vnum[j];
        }
    }
*/



}

void Vbo_map::update_vbo(int i, int j)
{

    update_skylight(i,j); //update skylights
    update_skylight2(i,j);
    update_envlight(i,j);

    class MAP_CHUNK* chunk = map->chunk[j*MAP_CHUNK_XDIM + i];  //map chunk
    class Map_vbo* vbo = vbo_array[j*MAP_CHUNK_XDIM + i];       //vbo for storing resulting vertices

    for(int i=0; i<SIDE_BUFFER_ARRAY_SIZE; i++) SIDE_BUFFER_INDEX[i] = 0;

    if(chunk == NULL) printf("Error: Vbo_map::update_vbo, chunk null; impossible\n");
    if(vbo == NULL) printf("Error: Vbo_map::update_vbo, vbo null; impossible\n");

    int vertex_count[2] = {0, 0};


    set_vertex_buffers(chunk, vbo); //set the vertex 

    for(int i=0; i<6; i++) vertex_count[0] += 4*SIDE_BUFFER_INDEX[i];
    vertex_count[1] = 4*SIDE_BUFFER_INDEX[6];

    int vnum = vertex_count[0] + vertex_count[1];

    vertex_max = vnum;
    vbo->_v_num[0] = vertex_count[0];
    vbo->_v_num[1] = vertex_count[1];

    vbo->_v_offset[0] = 0;
    vbo->_v_offset[1] = vertex_count[0];

    if(vnum == 0) 
    {
        vbo->vnum = 0;
        if(vbo->vbo_id != 0) glDeleteBuffers(1, &vbo->vbo_id); vbo->vbo_id = 0;
        return;
    } 
    else 
    {
        vbo->vnum = vnum; //total vertices, size of VBO
        // if( vnum > vbo->vnum_max ) vbo->resize(vnum);
    }


    //struct Vertex* vlist = (struct Vertex*) malloc(vnum*sizeof(struct Vertex));
    struct Vertex* vlist = new Vertex[vnum];

    generate_vertex_list(vlist);
    generate_quad_ao_values(vlist);

    if(vbo->v_list != NULL)
        delete[] vbo->v_list; //free old memory
    vbo->v_list = vlist;

    if(vbo->vbo_id == 0)  glGenBuffers(1, &vbo->vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, vbo->vbo_id);
    glBufferData(GL_ARRAY_BUFFER, vbo->vnum*sizeof(struct Vertex), NULL, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, vbo->vnum*sizeof(struct Vertex), vbo->v_list, GL_STATIC_DRAW);
}

 
#define USE_QUAD_CACHE_COMPATIBABILITY 0

static void push_quad_compatibility(struct VertexBackup* v_list, int offset, int x, int y, int z, int side, struct MAP_ELEMENT element) 
{

    int tile_id = element.block;

#if USE_QUAD_CACHE_COMPATIBABILITY
    memcpy(&v_list[offset], &quad_cache_compatibility[tile_id*6*4 +4*side], 4*sizeof(struct Vertex)); //id*6*4+4*side+vert_num
#else

    v_list[offset+0].tex = texElementArray2[0].tex;
    v_list[offset+1].tex = texElementArray2[1].tex;
    v_list[offset+2].tex = texElementArray2[2].tex;
    v_list[offset+3].tex = texElementArray2[3].tex;

    //int iz = 0;
    //int iw = 0;

    //int tile_tex = (unsigned char) cube_side_texture_array[6*tile_id+side];
    int tile_tex = cube_texture_palette[ 6*( cube_texture_palette_lookup[tile_id] + element.palette ) + side ];

    float tx_min = (tile_tex % 16)*0.0625f;
    float ty_min = (tile_tex / 16)*0.0625f;

    float tx_max = (tile_tex % 16)*0.0625f + 0.0625f;
    float ty_max = (tile_tex / 16)*0.0625f + 0.0625f;

    v_list[offset+0].tx0 = tx_max;
    v_list[offset+0].ty0 = ty_min;

    v_list[offset+1].tx0 = tx_max;
    v_list[offset+1].ty0 = ty_max;

    v_list[offset+2].tx0 = tx_min;
    v_list[offset+2].ty0 = ty_max;
    
    v_list[offset+3].tx0 = tx_min;
    v_list[offset+3].ty0 = ty_min;

    for(int i=0; i<4; i++)
    {
        v_list[offset+i].x =  _v_index[4*side+i].x;
        v_list[offset+i].y =  _v_index[4*side+i].y;
        v_list[offset+i].z =  _v_index[4*side+i].z;
    }

#endif

    float light = light_lookup[get_lighting(x,y,z,side)];

    v_list[offset+0].lighting[0] = light;
    v_list[offset+1].lighting[0] = light;
    v_list[offset+2].lighting[0] = light;
    v_list[offset+3].lighting[0] = light;
    
    {
        int _x = x & 15;
        int _y = y & 15;

        for(int i=0; i<4;i++) 
        {
            v_list[offset+i].x += _x;
            v_list[offset+i].y += _y;
            v_list[offset+i].z += z;
        }
    }

    for(int i=0 ;i <4; i++)
    {
        v_list[offset+i].color = 0xffffffff;
    }

}


void generate_vertex_list_compatibility(struct VertexBackup* vlist)
{
    int offset = 0;

    for(int side=0; side<SIDE_BUFFER_ARRAY_SIZE; side++)
    for(int j=0; j<SIDE_BUFFER_INDEX[side]; j++)
    {
        struct SIDE_BUFFER sb = SIDE_BUFFER_ARRAY[side][j];

        int x = sb.x;
        int y = sb.y;
        int z = sb.z;

        struct MAP_ELEMENT element = sb.element;

        push_quad_compatibility(vlist, offset, x,y,z, side, element);

        offset += 4;
    }

}

void generate_quad_ao_values_compatibility(struct VertexBackup* vlist)
{
    int offset = 0;

    for(int side=0; side<SIDE_BUFFER_ARRAY_SIZE; side++)
    for(int j=0; j<SIDE_BUFFER_INDEX[side]; j++)
    {
        //struct SIDE_BUFFER sb = SIDE_BUFFER_ARRAY[side][j];

        int x = SIDE_BUFFER_ARRAY[side][j].x;
        int y = SIDE_BUFFER_ARRAY[side][j].y;
        int z = SIDE_BUFFER_ARRAY[side][j].z;

        _set_quad_local_ambient_occlusion_compatibility(vlist, offset, x, y, z, side);

        offset += 4;
    }

}

void Vbo_map::update_vbo_compatibility(int i, int j)
{

    update_skylight(i,j); //update skylights
    update_skylight2(i,j);
    update_envlight(i,j);

    class MAP_CHUNK* chunk = map->chunk[j*MAP_CHUNK_XDIM + i];  //map chunk
    class Map_vbo* vbo = vbo_array[j*MAP_CHUNK_XDIM + i];       //vbo for storing resulting vertices

    for(int i=0; i<SIDE_BUFFER_ARRAY_SIZE; i++) SIDE_BUFFER_INDEX[i] = 0;

    if(chunk == NULL) printf("Error: Vbo_map::update_vbo, chunk null; impossible\n");
    if(vbo == NULL) printf("Error: Vbo_map::update_vbo, vbo null; impossible\n");

    int vertex_count[2] = {0, 0};

    set_vertex_buffers(chunk, vbo); //set the vertex 

    for(int i=0; i<6; i++) vertex_count[0] += 4*SIDE_BUFFER_INDEX[i];
    vertex_count[1] = 4*SIDE_BUFFER_INDEX[6];

    int vnum = vertex_count[0] + vertex_count[1];

    vertex_max = vnum;
    vbo->_v_num[0] = vertex_count[0];
    vbo->_v_num[1] = vertex_count[1];

    vbo->_v_offset[0] = 0;
    vbo->_v_offset[1] = vertex_count[0];

    if(vnum == 0) 
    {
        vbo->vnum = 0;
        if(vbo->vbo_id != 0) glDeleteBuffers(1, &vbo->vbo_id); vbo->vbo_id = 0;
        return;
    } 
    else 
    {
        vbo->vnum = vnum; //total vertices, size of VBO
    }


    //struct Vertex* vlist = (struct Vertex*) malloc(vnum*sizeof(struct Vertex));
    struct VertexBackup* vlist = new VertexBackup[vnum];

    generate_vertex_list_compatibility(vlist);
    generate_quad_ao_values_compatibility(vlist);

    if(vbo->v_list_backup != NULL)
        delete[] vbo->v_list_backup; //free old memory
    vbo->v_list_backup = vlist;

    if(vbo->vbo_id == 0)  glGenBuffers(1, &vbo->vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, vbo->vbo_id);
    glBufferData(GL_ARRAY_BUFFER, vbo->vnum*sizeof(struct VertexBackup), NULL, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, vbo->vnum*sizeof(struct VertexBackup), vbo->v_list_backup, GL_STATIC_DRAW);
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


}   // t_map


#ifdef PUSHED_OPTIMIZATIONS
# ifdef __MSVC__
#  pragma optimize("", off)
# endif
# if __GNUC__
#  pragma GCC pop_options
# endif
# undef PUSHED_OPTIMIZATIONS
#endif
