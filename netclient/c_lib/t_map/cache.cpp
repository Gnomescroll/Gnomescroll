#include "cache.hpp"

void init_cache()
{
    if( quad_cache == NULL) quad_cache = (struct Vertex*) malloc( max_cubes*6*4 * sizeof(struct Vertex));
}


/*
    Deprecate, use lookup table
*/


int _init_quad_cache_normals() 
{
    int cube_id, side, i;

    int n[3];
    for(cube_id=0;cube_id<max_cubes;cube_id++) {
        for(side=0;side<6;side++) {

        /*
            if(side == 1) n = {{0,0,-1}};
            if(side == 2) n = {{1,0,0}};
            if(side == 3) n = {{-1,0,0}};
            if(side == 4) n = {{0,1,0}};
            if(side == 5) n = {{0,-1,0}};
        */      
            if(side == 0) {
                n[0]= 0;
                n[1]= 0;
                n[2]= 1;
            }
            if(side == 1) {
                n[0]= 0;
                n[1]= 0;
                n[2]= -1;
            }
            if(side == 2) {
                n[0]= 1;
                n[1]= 0;
                n[2]= 0;
            }

            if(side == 3) {
                n[0]= -1;
                n[1]= 0;
                n[2]= 0;
            }
            if(side == 4) {
                n[0]= 0;
                n[1]= 1;
                n[2]= 0;
            }
            if(side == 5) {
                n[0]= 0;
                n[1]= -1;
                n[2]= 0;
            }

            int index; 
            for(i=0;i<4;i++) {
                index = cube_id*6*4+ 4*side+ i;
                quad_cache[index].normal[0] = n[0];
                quad_cache[index].normal[1] = n[1];
                quad_cache[index].normal[2] = n[2];
                quad_cache[index].normal[3] = 0;
            }
        }
    }
    return 0;
}