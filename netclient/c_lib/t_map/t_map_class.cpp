#include "t_map_class.hpp"

#include "t_map.hpp"

#include "t_properties.hpp"
#include <c_lib/t_map/common/map_element.hpp>

namespace t_map
{

    static const MAP_ELEMENT NO_MAP_ELEMENT = {{{0}}};
    const int TERRAIN_MAP_HEIGHT_BIT_MASK = ~(TERRAIN_MAP_HEIGHT-1);
    const int TERRAIN_MAP_WIDTH_BIT_MASK = ~(512-1); //assumes map size of 512

    /*
        Constructors
    */
    MAP_CHUNK::MAP_CHUNK(int _xpos, int _ypos)
    {

        #ifdef DC_CLIENT
            needs_update = false;
        #endif
        xpos = _xpos;
        ypos = _ypos;
        //for(int i=0; i<16*16*TERRAIN_MAP_HEIGHT;i++) e[i].n = 0;
        memset(e, 0, 16*16*TERRAIN_MAP_HEIGHT*sizeof(struct MAP_ELEMENT) );
        for(int i=0; i<16*16;i++) top_block[i] = 0;
    }


    Terrain_map::Terrain_map(int _xdim, int _ydim)
    {
        xdim = (_xdim/16)*16; 
        ydim = (_ydim/16)*16;
        xchunk_dim = _xdim/16; 
        ychunk_dim = _ydim/16;

        chunk = new MAP_CHUNK*[xchunk_dim*ychunk_dim];
        for(int i=0; i<xchunk_dim*ychunk_dim; i++) chunk[i] = NULL;
    }

    Terrain_map::~Terrain_map()
    {
        for(int i=0; i < xchunk_dim*ychunk_dim; i++)
        {
            if(chunk[i] != NULL) delete chunk[i];
        }
        delete[] chunk;
    }


/*
    Get Methods
*/
    struct MAP_ELEMENT Terrain_map::get_element(int x, int y, int z)
    {
    #if T_MAP_GET_OPTIMIZED
        if( ((z & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0) 
            || ((x & TERRAIN_MAP_WIDTH_BIT_MASK) != 0) 
            || ((y & TERRAIN_MAP_WIDTH_BIT_MASK) != 0) 
        ) return NO_MAP_ELEMENT;
        struct MAP_CHUNK* c;
        c = chunk[ MAP_CHUNK_WIDTH*(y >> 4) + (x >> 4) ];
        //c = chunk[ (y | ~15) + (x >> 4)];
        if(c == NULL) return NO_MAP_ELEMENT;
        //return c->e[(16*16)*z+ 16*(y | 15) + (x | 15)];

    #if T_MAP_GET_DEBUG
        if( (z<<8)+((y&15)<<4)+(x&15) >= 16*16*TERRAIN_MAP_HEIGHT) 
        {
            printf("ERROR: terrain map get, index out of bounds!\n");
            printf("0 original: x= %i y= %i z= %i \n", x,y,z);
            printf("1 original: x= %i y= %i z= %i \n", (x&15),(y&15),z);
            printf("2 index= %i, x= %i, y= %i z= %i \n", (z<<8)+((y&15)<<4)+(x&15), (x&15) ,((y&15)<<4), (z<<8)+((y|15)<<4) );
        }

        {
            int xi = x & 15; //bit mask
            int yi = y & 15; //bit mask

            int index = 16*16*z+ 16*yi + xi;
            if(index != (z<<8)+((y&15)<<4)+(x&15))
            {
                printf("ERROR: terrain map get, descrepency between optimized and unoptimized index values! \n");
            }
        }
     #endif

        return c->e[ (z << 8)+ ((y & 15) <<4) + (x & 15)];
    #else

        if( z >= TERRAIN_MAP_HEIGHT || z < 0 ) return NO_MAP_ELEMENT;
        if( x >= 512 || x < 0 ) return NO_MAP_ELEMENT;
        if( y >= 512 || y < 0 ) return NO_MAP_ELEMENT;

        struct MAP_CHUNK* c;
        {
            int xchunk = (x >> 4);
            int ychunk = (y >> 4);
            c = chunk[ MAP_CHUNK_WIDTH*ychunk + xchunk ];
            if( c == NULL ) return NO_MAP_ELEMENT;
        }

        int xi = x & 15; //bit mask
        int yi = y & 15; //bit mask


        int index = 16*16*z+ 16*yi + xi;
    #if T_MAP_GET_DEBUG
        if( index >= 16*16*TERRAIN_MAP_HEIGHT) printf("ERROR: terrain map get, index out of bounds! %i %i %i \n", xi,yi,z);
    #endif
        return c->e[16*16*z+ 16*yi + xi];
    #endif
    }

/*
    Set Methods 
*/
    #define T_MAP_SET_OPTIMIZED 0
    void Terrain_map::set_element(int x, int y, int z, struct MAP_ELEMENT element)
    {
        //printf("set: %i %i %i %i \n", x,y,element.block);
    #if T_MAP_SET_OPTIMIZED
        if( ((z & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0) 
            || ((x & TERRAIN_MAP_WIDTH_BIT_MASK) != 0) 
            ||  ((y & TERRAIN_MAP_WIDTH_BIT_MASK) != 0) 
        ) return; //an error
        struct MAP_CHUNK* c;
        c = chunk[ MAP_CHUNK_WIDTH*(y >> 4) + (x >> 4) ];
        //c = chunk[ (y | ~15) + (x >> 4)];
        if( c != NULL )
        {
            c = new MAP_CHUNK( x & ~15, y & ~15);
            chunk[ MAP_CHUNK_WIDTH*(y >> 4) + (x >> 4) ] = c;
        }
        //c->e[(16*16)*z+ 16*(y | 15) + (x | 15)] = element;
        c->e[ (z << 8)+ ((y & 15) <<4) + (x & 15)] = element;

        #ifdef DC_CLIENT
            c->needs_update = true;
        #endif
    #else
        //printf("set: %i %i %i \n",  x,y,z);

        if( z >= TERRAIN_MAP_HEIGHT || z < 0 ) return;
        if( x >= 512 || x < 0 ) return;
        if( y >= 512 || y < 0 ) return;
        //printf("set %i, %i, %i \n", x,y,z);
        struct MAP_CHUNK* c;
        {
            int xchunk = (x >> 4);
            int ychunk = (y >> 4);
            //printf("chunk= %i, %i \n", xchunk, ychunk);
    
            c = chunk[ MAP_CHUNK_WIDTH*ychunk + xchunk ];
            //printf("chunk index= %i \n", x & ~15, y & ~15, MAP_CHUNK_WIDTH*ychunk + xchunk );

            if( c == NULL )
            {
                //printf("new map chunk= %i %i \n", x & ~15, y & ~15 );
                c = new MAP_CHUNK( x & ~15, y & ~15);
                chunk[ MAP_CHUNK_WIDTH*ychunk + xchunk ] = c;
                
            }
        }

        int xi = x & 15; //bit mask
        int yi = y & 15; //bit mask
        //printf("xi= %i, yi= %i, z= %i \n", xi,yi,z );
        //printf("index2 = %i \n", 16*16*z+ 16*yi + xi);

        c->e[16*16*z+ 16*yi + xi] = element;
        #ifdef DC_CLIENT
            c->needs_update = true;
        #endif
    #endif
    }

/*
    if (dmg <= 0) return -4;
    int xoff, yoff, zoff, xrel, yrel, zrel;
    int tile;
    struct vm_column* column;
    struct vm_chunk* chunk;
    xoff = x >> 3; yoff = y >> 3; zoff = z >> 3;
    if(xoff < 0 || xoff >= vm_map_dim || yoff < 0 || yoff >= vm_map_dim || zoff < 0 || zoff >= vm_column_max) {
        return -2;
    }
    xrel = x - (xoff << 3); yrel = y - (yoff << 3); zrel = z - (zoff << 3);
    column = &map.column[vm_map_dim*yoff + xoff];
    chunk = column->chunk[zoff];
    if(chunk == NULL) {
        //printf("_apply_damage: Chunk is NULL:: %d,%d,%d dmg=%d\n", x,y,z, dmg);
        //printf("xyzoff:: %d %d %d\n", xoff, yoff, zoff);
        return -3;
    }
    tile = chunk->voxel[vm_chunk_size*vm_chunk_size*zrel+ vm_chunk_size*yrel + xrel];
    if (tile==0) {
        return -1;
    }
    unsigned char* damage;
    damage = &chunk->damage[vm_chunk_size*vm_chunk_size*zrel+ vm_chunk_size*yrel + xrel];
    *damage += dmg;
    if(*damage >= _maxDamage(tile) ) {
        // destroy block
        _set(x,y,z, 0);
        return 0;
    } else {
        return *damage;
    }
*/


    int Terrain_map::apply_damage(int x, int y, int z, int dmg)
    {
        //printf("set: %i %i %i %i \n", x,y,element.block);
    #if T_MAP_GET_OPTIMIZED
        if (dmg <= 0) return -4;

        if( ((z & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0) 
            || ((x & TERRAIN_MAP_WIDTH_BIT_MASK) != 0) 
            ||  ((y & TERRAIN_MAP_WIDTH_BIT_MASK) != 0) 
        ) return -2; //an error
        struct MAP_CHUNK* c;
        c = chunk[ MAP_CHUNK_WIDTH*(y >> 4) + (x >> 4) ];
        //c = chunk[ (y | ~15) + (x >> 4)];
        if( c != NULL ) return -3;
        //c->e[(16*16)*z+ 16*(y | 15) + (x | 15)] = element;
        struct MAP_ELEMENT* e = &c->e[ (z << 8)+ ((y | 15) <<4) + (x | 15)];

        if(e->block == 0) return -1;

        e->damage += dmg;

        if(e->damage >= maxDamage(e->block) ) 
        {
            // destroy block
            *e = NO_MAP_ELEMENT; 
            return 0;
        } 
        else 
        {
            return e->damage;
        }

    #else
        if (dmg <= 0) return -4;

        if( z >= TERRAIN_MAP_HEIGHT || z < 0 ) return -2;
        if( x >= 512 || x < 0 ) return -2 ;
        if( y >= 512 || y < 0 ) return -2;
        //printf("set %i, %i, %i \n", x,y,z);
        struct MAP_CHUNK* c;
        {
            int xchunk = (x >> 4);
            int ychunk = (y >> 4);
            //printf("chunk= %i, %i \n", xchunk, ychunk);
    
            c = chunk[ MAP_CHUNK_WIDTH*ychunk + xchunk ];
            //printf("chunk index= %i \n", x & ~15, y & ~15, MAP_CHUNK_WIDTH*ychunk + xchunk );

            if( c == NULL ) return -3;
        }

        int xi = x & 15; //bit mask
        int yi = y & 15; //bit mask
        //printf("xi= %i, yi= %i, z= %i \n", xi,yi,z );
        //printf("index2 = %i \n", 16*16*z+ 16*yi + xi);

        struct MAP_ELEMENT* e =  &c->e[16*16*z+ 16*yi + xi];
        if(e->block == 0) return -1;

        e->damage += dmg;

        if(e->damage >= maxDamage(e->block) ) 
        {
            // destroy block
            *e = NO_MAP_ELEMENT; 
            return 0;
        } 
        else 
        {
            return e->damage;
        }

    #endif
    }


/*
    Block Methodss
*/
    int Terrain_map::get_block(int x, int y, int z)
    {
        return get_element(x,y,z).block;
    }

    void Terrain_map::set_block(int x, int y, int z, int value)
    {
        struct MAP_ELEMENT element = {{{value, 0,0,0}}};
        set_element(x,y,z, element);
    }

}