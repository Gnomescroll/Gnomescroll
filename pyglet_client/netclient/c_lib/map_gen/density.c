#include "density.h"

struct Map_e {
    int density = 0;

}

struct Map_e* map_e;

#define me_xdim 64
#define me_ydim 64
#define me_zdim 32
#define me_t me_xdim*me_ydim*me_zdim

int map_gen_density_active = 1;
void init_map_gen_density() {
    if(map_gen_density_active = 0) return;
    map_e = (struct Map_e ) malloc(me_xdim*me_ydim*me_zdim*sizeof(struct Map_e));
    int i;
    for(i=0; i< me_t; i++ ) {
        map_e[i].density = 0

    }
}
/*
printf("RANDMAX= %i \n", RAND_MAX);
srand(15); //seed
_vx = (float)rand()/(float)RAND_MAX;
*/


