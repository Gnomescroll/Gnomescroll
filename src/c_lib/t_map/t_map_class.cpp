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
/*
    Constructors
*/
MAP_CHUNK::MAP_CHUNK(int _xpos, int _ypos)
{
    GS_ASSERT( (_xpos % TERRAIN_CHUNK_WIDTH) == 0 && (_ypos % TERRAIN_CHUNK_WIDTH) == 0 );
    #if DC_CLIENT
    needs_update = false;
    #endif
    xpos = _xpos;
    ypos = _ypos;
    version = 0;

    chunk_index = (ypos / TERRAIN_CHUNK_WIDTH)*(MAP_CHUNK_XDIM) + (xpos / TERRAIN_CHUNK_WIDTH);
    chunk_item_container.chunk_index = chunk_index;
    memset(e, 0, TERRAIN_CHUNK_WIDTH*TERRAIN_CHUNK_WIDTH*TERRAIN_MAP_HEIGHT*sizeof(struct MAP_ELEMENT) );
}


Terrain_map::Terrain_map(int _xdim, int _ydim)
{
    this->xdim = _xdim;
    this->ydim = _xdim;

    chunk = new MAP_CHUNK*[xchunk_dim*ychunk_dim];

    for(int i=0; i<xchunk_dim*ychunk_dim; i++) 
        chunk[i] = NULL;

    #if DC_SERVER
    for(int i=0; i<xchunk_dim; i++)
    for(int j=0; j<ychunk_dim; j++)
        load_chunk(i,j);
    #endif

    #if DC_CLIENT
    this->height_changed = false;
    for (int i=0; i<MAP_WIDTH*MAP_HEIGHT; i++) column_heights[i] = 0;
    for (int i=0; i<MAP_CHUNK_YDIM*MAP_CHUNK_XDIM; i++) chunk_heights_status[i] = CHUNK_HEIGHT_UNSET;
    #endif
}

Terrain_map::~Terrain_map()
{
    if (this->chunk != NULL)
    {
        for(int i=0; i < xchunk_dim*ychunk_dim; i++)
            if(this->chunk[i] != NULL) delete this->chunk[i];
        delete[] this->chunk;
    }
}

/*
Height Cache
*/

void MAP_CHUNK::refresh_height_cache()
{
    for(int i=0; i<TERRAIN_CHUNK_WIDTH; i++)
    for(int j=0; j<TERRAIN_CHUNK_WIDTH; j++)
    for(int k=ZMAX-1; k>=0; k--)
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


CubeType MAP_CHUNK::get_block(int x, int y, int z)
{
    return (CubeType)e[ (z<<8)+((y&15)<<4)+(x&15) ].block;
}

struct MAP_ELEMENT MAP_CHUNK::get_element(int x, int y, int z)
{
    return e[ (z<<8)+((y&15)<<4)+(x&15) ];
}

struct MAP_ELEMENT Terrain_map::get_element(int x, int y, int z)
{
    #if T_MAP_GET_OPTIMIZED
    // don't assert here -- some methods that use this will naturally pass in -1
    // and its intended 
    //IF_ASSERT((z & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0) return NULL_MAP_ELEMENT;
    if ((z & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0) return NULL_MAP_ELEMENT;
    x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    y &= TERRAIN_MAP_WIDTH_BIT_MASK2;

    class MAP_CHUNK* c;

    c = chunk[ MAP_CHUNK_XDIM*(y >> 4) + (x >> 4) ];
    if(c == NULL) return NULL_MAP_ELEMENT;
    return c->e[ (z<<8)+((y&15)<<4)+(x&15) ];

    #else    
    IF_ASSERT((z & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0) return NULL_MAP_ELEMENT;
    x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    y &= TERRAIN_MAP_WIDTH_BIT_MASK2;

    class MAP_CHUNK* c;
    {
        int xchunk = (x >> 4);
        int ychunk = (y >> 4);
        c = chunk[ MAP_CHUNK_XDIM*ychunk + xchunk ];
        if( c == NULL ) return NULL_MAP_ELEMENT;
    }

    int xi = x & 15; //bit mask
    int yi = y & 15; //bit mask

    return c->e[TERRAIN_CHUNK_WIDTH*TERRAIN_CHUNK_WIDTH*z+ TERRAIN_CHUNK_WIDTH*yi + xi];
    #endif
}

#if DC_CLIENT
void Terrain_map::set_update(int x, int y)
{
    //printf("set update: %i %i \n", x,y);

    x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    y &= TERRAIN_MAP_WIDTH_BIT_MASK2;

    int index = MAP_CHUNK_XDIM*(y >> 4) + (x >> 4);
    GS_ASSERT(index >= 0 && index < 32*32);
    
    class MAP_CHUNK* c = chunk[ MAP_CHUNK_XDIM*(y >> 4) + (x >> 4) ];

    GS_ASSERT(c != NULL);
    if( c != NULL ) 
        c->needs_update = true;
}
#endif

/*
Set Methods 
*/


void MAP_CHUNK::set_element(int x, int y, int z, struct MAP_ELEMENT element)
{
    e[ (z<<8)+((y&15)<<4)+(x&15) ] = element;

/*
    Causes horrible error on client
*/
//#if DC_CLIENT
//    this->needs_update = true;
//#endif
}


void Terrain_map::set_element(int x, int y, int z, struct MAP_ELEMENT element)
{
    IF_ASSERT((z & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0) return;
    x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    y &= TERRAIN_MAP_WIDTH_BIT_MASK2;

    class MAP_CHUNK* c;

    int xchunk = (x >> 4);
    int ychunk = (y >> 4);

    c = chunk[ MAP_CHUNK_XDIM*ychunk + xchunk ];

    #if DC_CLIENT
    GS_ASSERT(c != NULL)
    if (c == NULL) return;
    #endif

    int xi = x & 15; //bit mask
    int yi = y & 15; //bit mask

    c->e[TERRAIN_CHUNK_WIDTH*TERRAIN_CHUNK_WIDTH*z+ TERRAIN_CHUNK_WIDTH*yi + xi] = element;

    #if DC_CLIENT
    c->needs_update = true; 

    if((x & 15) == 0)  set_update(x-1,y);
    if((x & 15) == 15) set_update(x+1,y);
    if((y & 15) == 0)  set_update(x,y-1);
    if((y & 15) == 15) set_update(x,y+1);
    #endif
}

int Terrain_map::get_damage(int x, int y, int z)
{
    IF_ASSERT((z & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0) return -2;
    x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    y &= TERRAIN_MAP_WIDTH_BIT_MASK2;

    class MAP_CHUNK* c;
    {
        int xchunk = (x >> 4);
        int ychunk = (y >> 4);

        c = chunk[ MAP_CHUNK_XDIM*ychunk + xchunk ];

        if( c == NULL ) return -3;
    }

    int xi = x & 15; //bit mask
    int yi = y & 15; //bit mask
    struct MAP_ELEMENT* e =  &c->e[TERRAIN_CHUNK_WIDTH*TERRAIN_CHUNK_WIDTH*z+ TERRAIN_CHUNK_WIDTH*yi + xi];
    
    return e->damage;
}

// return negative on error, 0 on destruction of underlying block, or total damage so far on block 
int Terrain_map::apply_damage(int x, int y, int z, int dmg, CubeType* cube_type)
{
    if (dmg <= 0) return -4;

    IF_ASSERT((z & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0) return -2;
    x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    y &= TERRAIN_MAP_WIDTH_BIT_MASK2;

    class MAP_CHUNK* c;
    {
        int xchunk = (x >> 4);
        int ychunk = (y >> 4);

        c = chunk[ MAP_CHUNK_XDIM*ychunk + xchunk ];

        if( c == NULL ) return -3;
    }

    int xi = x & 15; //bit mask
    int yi = y & 15; //bit mask

    struct MAP_ELEMENT* e =  &c->e[TERRAIN_CHUNK_WIDTH*TERRAIN_CHUNK_WIDTH*z+ TERRAIN_CHUNK_WIDTH*yi + xi];
    
    *cube_type = (CubeType)e->block;

    if(e->block == 0) return -1;
    int maxdmg = maxDamage((CubeType)e->block);
    if (maxdmg >= INVINCIBLE_CUBE_DAMAGE) return -5;
    if(e->damage + dmg >= maxdmg) 
    {
        e->damage = maxdmg;

        //moved to set function
/*
        // destroy block
        *e = NULL_MAP_ELEMENT;   
        _envlight_update2(x,y,z); //light update

        #if DC_SERVER
        if(isItemContainer(*cube_type))
            destroy_item_container_block(x,y,z);
        if (isExplosive(*cube_type))
            handle_explosive_block(x,y,z);
        #endif

        #if DC_CLIENT
        c->needs_update = true; 

        if((x & 15) == 0)  set_update(x-1,y);
        if((x & 15) == 15) set_update(x+1,y);
        if((y & 15) == 0)  set_update(x,y-1);
        if((y & 15) == 15) set_update(x,y+1);
        #endif

        #if DC_SERVER
        t_mech::handle_block_removal(x,y,z);
        #endif
*/
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
    MAP_CHUNK* m = this->chunk[cx + MAP_CHUNK_XDIM*cy];
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
            this->column_heights[x + y*MAP_WIDTH] = h;
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
    if(chunk[ MAP_CHUNK_XDIM*CY + CX ] != NULL)
        chunk[ MAP_CHUNK_XDIM*CY + CX ]->needs_update = true;

    CX = (cx+CW1) % CW;
    CY = cy;
    if(chunk[ MAP_CHUNK_XDIM*CY + CX ] != NULL)
        chunk[ MAP_CHUNK_XDIM*CY + CX ]->needs_update = true;

    CX = cx;
    CY = (cy+1) % CW;
    if(chunk[ MAP_CHUNK_XDIM*CY + CX ] != NULL)
        chunk[ MAP_CHUNK_XDIM*CY + CX ]->needs_update = true;

    CX = cx;
    CY = (cy+CW1) % CW;
    if(chunk[ MAP_CHUNK_XDIM*CY + CX ] != NULL)
        chunk[ MAP_CHUNK_XDIM*CY + CX ]->needs_update = true;

    //DEBUG
    GS_ASSERT( ((cy+CW1) % CW) == ((cy-1) & MASK) );
    /*
        Update Lighting
    */

    update_skylight(cx, cy);
    update_skylight2(cx,cy);    //update skylight
    
    init_update_envlight(cx, cy);   //init env_light values for blocks in chunk
    update_envlight(cx,cy);
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
    return this->column_heights[x + y*MAP_WIDTH];
}

/*
DEPRECATE
*/
void Terrain_map::update_heights(int x, int y, int z, CubeType cube_type)
{
    z += 1; // heights are not 0 indexed;
    int new_h = -1;
    int h = this->column_heights[x + y*MAP_WIDTH];
    if (cube_type != EMPTY_CUBE)
    {   // setting higher block
        if (z > h)
            new_h = z;
    }
    else
    {
        if (z >= h) // deleting top block
            new_h = get_highest_solid_block(x,y,z) + 1;
    }

    if (new_h == -1) return; // no change in height

    this->column_heights[x + y*MAP_WIDTH] = new_h;

    int cx = x / TERRAIN_CHUNK_WIDTH;   // truncate
    int cy = y / TERRAIN_CHUNK_WIDTH;
    this->chunk_heights_status[cx + cy*MAP_CHUNK_XDIM] = CHUNK_HEIGHT_CHANGED;
    this->height_changed = true;
}
#endif

/*
Block Methodss
*/
CubeType Terrain_map::get_block(int x, int y, int z)
{
    IF_ASSERT((z & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0) return EMPTY_CUBE;

    x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    y &= TERRAIN_MAP_WIDTH_BIT_MASK2;

    class MAP_CHUNK* c = chunk[ MAP_CHUNK_XDIM*(y >> 4) + (x >> 4) ];
    if (c == NULL) return EMPTY_CUBE;
    return (CubeType)c->e[ (z<<8)+((y&15)<<4)+(x&15) ].block;
}

void Terrain_map::set_block(int x, int y, int z, CubeType cube_type)
{
    IF_ASSERT((z & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0) return;

    x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    y &= TERRAIN_MAP_WIDTH_BIT_MASK2;

    struct MAP_ELEMENT element = {{{(unsigned char)cube_type, 0, fast_cube_attributes[cube_type].light_value,0}}};
    
    set_element(x,y,z, element);

    #if DC_CLIENT
    update_heights(x,y,z, cube_type);
    #endif
}

void Terrain_map::set_block_fast(int x, int y, int z, CubeType cube_type)
{
    IF_ASSERT((z & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0) return;

    x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    y &= TERRAIN_MAP_WIDTH_BIT_MASK2;

    struct MAP_ELEMENT element = {{{(unsigned char)cube_type, 0, fast_cube_attributes[cube_type].light_value,0}}};
    set_element(x,y,z, element);
}


#if DC_CLIENT
bool Terrain_map::chunk_loaded(int x, int y, int z)
{
    IF_ASSERT((z & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0) return false;

    x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    y &= TERRAIN_MAP_WIDTH_BIT_MASK2;

    class MAP_CHUNK* c = chunk[ MAP_CHUNK_XDIM*(y >> 4) + (x >> 4) ];
    return (c != NULL);
}
#endif


/*
Height Map
*/

inline int Terrain_map::get_height(int x, int y)
{
    x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    y &= TERRAIN_MAP_WIDTH_BIT_MASK2;

    class MAP_CHUNK* c = chunk[ MAP_CHUNK_XDIM*(y >> 4) + (x >> 4) ];
    IF_ASSERT(c == NULL) return 0;
    
    return c->height_cache[ ((y&(TERRAIN_CHUNK_WIDTH-1))<<4) + (x&(TERRAIN_CHUNK_WIDTH-1)) ];
}


//only entry point for loading chunks
void Terrain_map::load_chunk(int i, int j)
{
    IF_ASSERT(i < 0 || i >= xchunk_dim || j < 0 || j >= ychunk_dim) return;
    GS_ASSERT(this->chunk[xchunk_dim*j + i ] == NULL);
    this->chunk[xchunk_dim*j+i] = new MAP_CHUNK(TERRAIN_CHUNK_WIDTH*i, TERRAIN_CHUNK_WIDTH*j);
}      


//only entry point for unloading chunks
void Terrain_map::unload_chunk(int i, int j)
{
    IF_ASSERT(this->chunk[ xchunk_dim*j + i ] == NULL) return;
    delete this->chunk[xchunk_dim*j + i];
    this->chunk[xchunk_dim*j + i] = NULL;
}

}   // t_map
