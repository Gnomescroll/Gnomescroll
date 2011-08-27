#include "density.h"

struct Map_e {
    float density = 0;

}

struct Map_e* map_e;

int me_xdim 64;
int me_ydim 64;
int me_zdim 32;
int me_n;

int map_gen_density_active = 1;

void init_map_gen_density() {
    if(map_gen_density_active == 0) return;
    map_e = (struct Map_e ) malloc(me_xdim*me_ydim*me_zdim*sizeof(struct Map_e));
    me_n = me_xdim*me_ydim*me_zdim;
    int i;
    for(i=0; i< me_t; i++ ) {
        map_e[i].density = 0
    }
    printf("map_gen_density init finished\n");
}
/*
printf("RANDMAX= %i \n", RAND_MAX);
srand(15); //seed
_vx = (float)rand()/(float)RAND_MAX;
*/

#dim kernel_dim 8

inline float map_kernel_function(int x, int y, int z) {
    int i,j,k;
    int c= 0;
    int tile;
    float density;

    for(i=-kernel_dim/2; i<kernel_dim/2; i++) {
    for(j=-kernel_dim/2; j<kernel_dim/2; j++) {
    for(k=-kernel_dim/2; j<kernel_dim/2; k++) {

        tile = _get(x+i, y+j, z+k);
        if(isActive(tile)) c++;

    }}}

    density = ((float) c)/ (kernel_dim*kernel_dim*kernel_dim);
    return density;
}

void compute_density_map() {
    int i,j,k;
    //int a,b,c;

    for(i=0; i<me_xdim; i++) {
    for(j=0; j<me_ydim; j++) {
    for(k=0; k<me_zdim; k++) {
        map_e[i+ j*me_xdim + k*me_xdim*me_ydim].density = map_kernel_function(i,j,k);
    }}}

}

void map_density_visualize(int density, float min, float max) {
    int i,j,k;
    float d;
    float x,y,z;
    glColor3ub((unsigned char)0,(unsigned char)255,(unsigned char)0);
    glBegin(GL_POINTS);
    for(i=0; i<me_xdim; i+= density) {
    for(j=0; j<me_ydim; j+= density) {
    for(k=0; k<me_zdim; k+= density) {
        d = map_e[i+ j*me_xdim + k*me_xdim*me_ydim].density
        if(d < max && d > min) {
            x = i + 0.5;
            y = j + 0.5;
            z = k + 0.5;
            glVertex3f(x,y,z);
        }
    }}}
    glEnd();
}
