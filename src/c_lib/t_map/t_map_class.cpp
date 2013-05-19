#include "t_map_class.hpp"

#include <t_map/t_map.hpp>
#include <t_map/t_properties.hpp>
#include <t_map/_interface.hpp>
#include <t_map/common/constants.hpp>
#include <physics/quadrant.hpp>
#include <t_mech/_interface.hpp>
#include <t_map/lighting.hpp>

namespace t_map
{

MapChunk::MapChunk(int _xpos, int _ypos) :
    xpos(_xpos), ypos(_ypos), version(0)
{
    GS_ASSERT((_xpos % TERRAIN_CHUNK_WIDTH) == 0 &&
              (_ypos % TERRAIN_CHUNK_WIDTH) == 0);
    #if DC_CLIENT
    this->needs_update = false;
    #endif

    this->chunk_index = (ypos / TERRAIN_CHUNK_WIDTH)*(MAP_CHUNK_XDIM) + (xpos / TERRAIN_CHUNK_WIDTH);
    this->chunk_item_container.chunk_index = chunk_index;
    memset(this->height_cache, 0, TERRAIN_CHUNK_WIDTH*TERRAIN_CHUNK_WIDTH*sizeof(struct MapElement));
    memset(e, 0, TERRAIN_CHUNK_WIDTH*TERRAIN_CHUNK_WIDTH*TERRAIN_MAP_HEIGHT*sizeof(struct MapElement));
}


Terrain_map::Terrain_map(int _xdim, int _ydim)
{
    this->xdim = _xdim;
    this->ydim = _xdim;

    chunk = new MapChunk*[xchunk_dim*ychunk_dim];

    for (int i=0; i<xchunk_dim*ychunk_dim; i++)
        chunk[i] = NULL;

    #if DC_SERVER
    for (int i=0; i<xchunk_dim; i++)
    for (int j=0; j<ychunk_dim; j++)
        load_chunk(i,j);
    #endif

    #if DC_CLIENT
    this->height_changed = false;
    this->column_heights = (unsigned char*)calloc(map_dim.x * map_dim.y, sizeof(*this->column_heights));
    for (int i=0; i<map_dim.x*map_dim.y; i++) column_heights[i] = 0;
    for (int i=0; i<MAP_CHUNK_YDIM*MAP_CHUNK_XDIM; i++) chunk_heights_status[i] = CHUNK_HEIGHT_UNSET;
    #endif
}

Terrain_map::~Terrain_map()
{
    if (this->chunk != NULL)
    {
        for (int i=0; i < xchunk_dim*ychunk_dim; i++)
            delete this->chunk[i];
    }
    delete[] this->chunk;
    #if DC_CLIENT
    free(this->column_heights);
    #endif
}

/*
Height Cache
*/

void MapChunk::refresh_height_cache()
{
    for (int i=0; i<TERRAIN_CHUNK_WIDTH; i++)
    for (int j=0; j<TERRAIN_CHUNK_WIDTH; j++)
    for (int k=map_dim.z-1; k>=0; k--)
    {
        if (isSolid((CubeType)e[(k<<8)+(j<<4)+i].block))
        {
            this->height_cache[TERRAIN_CHUNK_WIDTH*j+i] = j+1; //first block above the firs solid block
            break;
        }
    }
}

/*
Get Methods
*/


ALWAYS_INLINE CubeType MapChunk::get_block(const Vec3i& position)
{
    return (CubeType)e[this->get_element_index(position)].block;
}

ALWAYS_INLINE CubeType MapChunk::get_block(int x, int y, int z)
{
    return (CubeType)e[this->get_element_index(x, y, z)].block;
}

ALWAYS_INLINE struct MapElement MapChunk::get_element(const Vec3i& position)
{
    return e[this->get_element_index(position)];
}

ALWAYS_INLINE struct MapElement MapChunk::get_element(int x, int y, int z)
{
    return e[this->get_element_index(x, y, z)];
}

struct MapElement Terrain_map::get_element(const Vec3i& position)
{
    if (!is_valid_z(position)) return NULL_MAP_ELEMENT; // don't assert
    Vec3i p = translate_position(position);

    #if T_MAP_GET_OPTIMIZED
    class MapChunk* c = this->get_chunk(p);
    if (c == NULL)
        return NULL_MAP_ELEMENT;
    return c->get_element(p);
    #else
    int xchunk = (p.x >> 4);
    int ychunk = (p.y >> 4);
    class MapChunk* c = chunk[MAP_CHUNK_XDIM * ychunk + xchunk];
    if (c == NULL)
        return NULL_MAP_ELEMENT;

    int xi = p.x & 15; //bit mask
    int yi = p.y & 15; //bit mask

    return c->e[TERRAIN_CHUNK_WIDTH*TERRAIN_CHUNK_WIDTH*z+ TERRAIN_CHUNK_WIDTH*yi + xi];
    #endif
}

inline struct MapElement Terrain_map::get_element(int x, int y, int z)
{
    return this->get_element(vec3i_init(x, y, z));
}


#if DC_CLIENT
void Terrain_map::set_update(int x, int y)
{
    x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    y &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    class MapChunk* c = chunk[ MAP_CHUNK_XDIM*(y >> 4) + (x >> 4) ];

    //GS_ASSERT(c != NULL);
    if (c != NULL)
        c->needs_update = true;
}
#endif

/*
Set Methods
*/


void Terrain_map::set_element(const Vec3i& position, struct MapElement element)
{
    IF_ASSERT(!is_valid_z(position)) return;
    Vec3i p = translate_position(position);

    int xchunk = (p.x >> 4);
    int ychunk = (p.y >> 4);
    class MapChunk* c = chunk[MAP_CHUNK_XDIM*ychunk + xchunk];

    #if DC_CLIENT
    IF_ASSERT(c == NULL) return;
    #endif

    int xi = p.x & 15; //bit mask
    int yi = p.y & 15; //bit mask

    const int index = TERRAIN_CHUNK_WIDTH*TERRAIN_CHUNK_WIDTH*p.z+ TERRAIN_CHUNK_WIDTH*yi + xi;
    struct MapElement old_element = c->e[index];
    c->e[index] = element;

    if(old_element.block != element.block) //remove this check to annoy everyone
    {
    /*
        Handle Special for Removed Blocks
    */

        if(fast_cube_properties[old_element.block].special)
        {
            if(fast_cube_properties[old_element.block].radioactive)
            {
                main_map->radiation_block_list.remove(vec3i_init(p));
            }
        #if DC_SERVER
            //item container
            else if (fast_cube_properties[old_element.block].item_container)
            {
                c->chunk_item_container.remove(p);
            }
        #endif
        }
    /*
        Handle special for Added Blocks
    */
        if(fast_cube_properties[element.block].special)
        {
            if(fast_cube_properties[element.block].radioactive)
            {
                main_map->radiation_block_list.add(vec3i_init(p));
            }
        }
    }
    //map updates
    #if DC_CLIENT
    c->needs_update = true;

    if ((p.x & 15) == 0)  set_update(p.x - 1, p.y);
    if ((p.x & 15) == 15) set_update(p.x + 1, p.y);
    if ((p.y & 15) == 0)  set_update(p.x, p.y - 1);
    if ((p.y & 15) == 15) set_update(p.x, p.y + 1);
    #endif


}

int Terrain_map::get_damage(const Vec3i& position)
{
    IF_ASSERT(!is_valid_z(position)) return -2;
    Vec3i p = translate_position(position);

    class MapChunk* c;
    {
        int xchunk = (p.x >> 4);
        int ychunk = (p.y >> 4);

        c = chunk[ MAP_CHUNK_XDIM*ychunk + xchunk];

        if (c == NULL) return -3;
    }

    int xi = p.x & 15; //bit mask
    int yi = p.y & 15; //bit mask
    struct MapElement* e = &c->e[TERRAIN_CHUNK_WIDTH*TERRAIN_CHUNK_WIDTH*p.z+ TERRAIN_CHUNK_WIDTH*yi + xi];

    return e->damage;
}

// return negative on error, 0 on destruction of underlying block, or total damage so far on block
int Terrain_map::apply_damage(const Vec3i& position, int dmg, CubeType* cube_type)
{
    if (dmg <= 0) return -4;
    IF_ASSERT(!is_valid_z(position)) return -2;
    Vec3i p = translate_position(position);

    class MapChunk* c;
    int xchunk = (p.x >> 4);
    int ychunk = (p.y >> 4);
    c = chunk[ MAP_CHUNK_XDIM*ychunk + xchunk ];
    if (c == NULL) return -3;

    int xi = p.x & 15; //bit mask
    int yi = p.y & 15; //bit mask

    int chunk_index = TERRAIN_CHUNK_WIDTH*TERRAIN_CHUNK_WIDTH*p.z + TERRAIN_CHUNK_WIDTH*yi + xi;
    struct MapElement* e =  &c->e[chunk_index];
    *cube_type = (CubeType)e->block;

    if (e->block == 0) return -1;
    int maxdmg = maxDamage((CubeType)e->block);
    if (maxdmg >= INVINCIBLE_CUBE_DAMAGE) return -5;
    if (e->damage + dmg >= maxdmg)
    {
        e->damage = maxdmg;
        return 0;   //tells it to set to 0
    }
    else
    {
        e->damage += dmg;
        return e->damage;
    }
}


#if DC_CLIENT

void Terrain_map::chunk_received(int cx, int cy)
{   // update chunk/column heights based on this chunk
    int highest = -1;
    int h;
    int xoff = cx * TERRAIN_CHUNK_WIDTH;
    int yoff = cy * TERRAIN_CHUNK_WIDTH;
    MapChunk* m = this->chunk[cx + MAP_CHUNK_XDIM*cy];
    int x,y;

    for (int i=0; i<TERRAIN_CHUNK_WIDTH; i++)
        for (int j=0; j<TERRAIN_CHUNK_WIDTH; j++)
        {
            x = xoff+i;
            y = yoff+j;

            for (h=map_dim.z-1; h>=0; h--)
                if (isSolid(m->get_block(x,y,h))) break;
            h += 1;

            if (h > highest) highest = h;
            this->column_heights[x + y*map_dim.x] = h;
        }

    this->chunk_heights_status[cx + cy*MAP_CHUNK_XDIM] = CHUNK_HEIGHT_CHANGED;
    this->height_changed = true;

    const static int MASK = (512/16)-1; //chunk width
    const static int CW = 32;
    const static int CW1 = 32-1; //add 32-1 instead of subtracting 1

    int CX;
    int CY;

    CX = (cx+1) % CW;
    CY = cy;
    if (chunk[ MAP_CHUNK_XDIM*CY + CX ] != NULL)
        chunk[ MAP_CHUNK_XDIM*CY + CX ]->needs_update = true;

    CX = (cx+CW1) % CW;
    CY = cy;
    if (chunk[ MAP_CHUNK_XDIM*CY + CX ] != NULL)
        chunk[ MAP_CHUNK_XDIM*CY + CX ]->needs_update = true;

    CX = cx;
    CY = (cy+1) % CW;
    if (chunk[ MAP_CHUNK_XDIM*CY + CX ] != NULL)
        chunk[ MAP_CHUNK_XDIM*CY + CX ]->needs_update = true;

    CX = cx;
    CY = (cy+CW1) % CW;
    if (chunk[ MAP_CHUNK_XDIM*CY + CX ] != NULL)
        chunk[ MAP_CHUNK_XDIM*CY + CX ]->needs_update = true;

    //DEBUG
    GS_ASSERT(((cy+CW1) % CW) == ((cy-1) & MASK));
    /*
        Init Update Lighting
    */

    //init_update_sunlight(cx, cy);   //init env_light values for blocks in chunk
    //init_update_envlight(cx, cy);   //init env_light values for blocks in chunk
}

/*
DEPRECATE
*/

void Terrain_map::reset_heights_read()
{   // call when heights are done being read
    this->height_changed = false;
    // only toggle CHANGED chunks, we dont want to switch UNSET->UNCHANGED
    for (int i=0; i<MAP_CHUNK_YDIM*MAP_CHUNK_XDIM; i++)
        if (chunk_heights_status[i] == CHUNK_HEIGHT_CHANGED)
            chunk_heights_status[i] = CHUNK_HEIGHT_UNCHANGED;
}

/*
DEPRECATE
*/
inline unsigned char Terrain_map::get_cached_height(int x, int y)
{
    x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    y &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    return this->column_heights[x + y*map_dim.x];
}

/*
DEPRECATE
*/
void Terrain_map::update_heights(const Vec3i& position, CubeType cube_type)
{
    Vec3i p = translate_position(position);
    p.z += 1; // heights are not 0 indexed;
    int new_h = -1;
    int h = this->column_heights[p.x + p.y*map_dim.x];
    if (cube_type != EMPTY_CUBE)
    {   // setting higher block
        if (p.z > h)
            new_h = p.z;
    }
    else
    {
        if (p.z >= h) // deleting top block
            new_h = get_highest_solid_block(p) + 1;
    }

    if (new_h == -1) return; // no change in height

    this->column_heights[p.x + p.y * map_dim.x] = new_h;

    int cx = p.x / TERRAIN_CHUNK_WIDTH;   // truncate
    int cy = p.y / TERRAIN_CHUNK_WIDTH;
    this->chunk_heights_status[cx + cy * MAP_CHUNK_XDIM] = CHUNK_HEIGHT_CHANGED;
    this->height_changed = true;
}
#endif

/*
Block Methodss
*/
CubeType Terrain_map::get_block(const Vec3i& position)
{
    IF_ASSERT(!is_valid_z(position)) return EMPTY_CUBE;
    Vec3i p = translate_position(position);

    class MapChunk* c = this->get_chunk(p);
    if (c == NULL)
        return EMPTY_CUBE;
    return (CubeType)c->get_element(p).block;
}

void Terrain_map::set_block(const Vec3i& position, CubeType cube_type)
{
    IF_ASSERT(!is_valid_z(position)) return;
    Vec3i p = translate_position(position);
    struct MapElement element = {{{
        (unsigned char)cube_type,
        0,
        (unsigned char)(fast_cube_attributes[cube_type].light_value << 4),
        0
    }}};
    set_element(p, element);
    #if DC_CLIENT
    update_heights(p, cube_type);
    #endif
}

void Terrain_map::set_block_fast(const Vec3i& position, CubeType cube_type)
{
    IF_ASSERT(!is_valid_z(position)) return;
    struct MapElement element = {{{
        (unsigned char)cube_type,
        0,
        (unsigned char)(fast_cube_attributes[cube_type].light_value << 4),
        0
    }}};
    set_element(position, element);
}

#if DC_CLIENT
bool Terrain_map::chunk_loaded(const Vec3i& position)
{
    IF_ASSERT(!is_valid_z(position)) return false;
    return (this->get_chunk(translate_position(position)) != NULL);
}
#endif


/*
Height Map
*/

inline int Terrain_map::get_height(int x, int y)
{
    x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    y &= TERRAIN_MAP_WIDTH_BIT_MASK2;

    class MapChunk* c = chunk[ MAP_CHUNK_XDIM*(y >> 4) + (x >> 4) ];
    IF_ASSERT(c == NULL) return 0;

    return c->height_cache[ ((y&(TERRAIN_CHUNK_WIDTH-1))<<4) + (x&(TERRAIN_CHUNK_WIDTH-1)) ];
}


//only entry point for loading chunks
void Terrain_map::load_chunk(int i, int j)
{
    IF_ASSERT(i < 0 || i >= xchunk_dim || j < 0 || j >= ychunk_dim) return;
    GS_ASSERT(this->chunk[xchunk_dim*j + i ] == NULL);
    this->chunk[xchunk_dim*j+i] = new MapChunk(TERRAIN_CHUNK_WIDTH*i, TERRAIN_CHUNK_WIDTH*j);
}


//only entry point for unloading chunks
void Terrain_map::unload_chunk(int i, int j)
{
    IF_ASSERT(this->chunk[ xchunk_dim*j + i ] == NULL) return;
    delete this->chunk[xchunk_dim*j + i];
    this->chunk[xchunk_dim*j + i] = NULL;
}

}   // t_map
