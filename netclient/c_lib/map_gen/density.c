#include "density.h"


int me_xdim = 32;
int me_ydim = 32;
int me_zdim = 16;
int me_n;

int map_gen_density_active = 1;



struct Map_density_element * MAPE;


void init_map_gen_density() {
    if(map_gen_density_active == 0) return;
    MAPE = (struct Map_density_element *) malloc(me_xdim*me_ydim*me_zdim*sizeof(struct Map_density_element));
    if(MAPE == NULL) {
        printf("MAPE NULL AFTER MALLOC !!!!WTF!!!!\n\n");
        }
    me_n = me_xdim*me_ydim*me_zdim;
    int i;
    for(i=0; i< me_n; i++ ) {
        MAPE[i].density = 0;
    }
    printf("map_gen_density init finished\n");
}

/*
printf("RANDMAX= %i \n", RAND_MAX);
srand(15); //seed
_vx = (float)rand()/(float)RAND_MAX;
*/

#define kernel_dim 8

inline float map_kernel_function1(int x, int y, int z) {
    int i,j,k;
    int c= 0;
    int tile;
    float density = 0;
    for(i=-kernel_dim/2; i<kernel_dim/2; i++) {
    for(j=-kernel_dim/2; j<kernel_dim/2; j++) {
    for(k=-kernel_dim/2; k<kernel_dim/2; k++) {

        tile = _get(x+i, y+j, z+k);
        if(isActive(tile)) {
            density += 1.0;

        }

    }}}

    density /= (float)(kernel_dim*kernel_dim*kernel_dim);
    /*
    if(density > 0.10) {
        printf("%i, %i, %i, %f \n", x,y,z,density);
    }
    */
    //printf("%f \n", density);
    return density;
}

int _ci = 0;
int _cj = 0;
int _ck = 0;

void update_density_map(int iterations) {
    //int i,j,k;
    //int a,b,c;
    if(MAPE == NULL) {
        printf("map_gen density: MAPE is null!\n");
        return;
    }
    int i;
    for(i=0; i<iterations; i++) {
    _ci++;
    if(_ci >= me_xdim) { _ci=0; _cj++; }
    if(_cj >= me_ydim) { _cj=0; _ck++; }
    if(_ck >= me_zdim) { _ck=0; printf("Complete Map Density Update\n"); }

    MAPE[_ci+ _cj*me_xdim + _ck*me_xdim*me_ydim].density = map_kernel_function1(_ci,_cj,_ck);
    }

/*
    for(i=0; i<me_xdim; i++) {
    for(j=0; j<me_ydim; j++) {
    for(k=0; k<me_zdim; k++) {
        //printf("%i, %i, %i\n",i,j,k);
        MAPE[i+ j*me_xdim + k*me_xdim*me_ydim].density = map_kernel_function1(i,j,k);
    }}}
*/
    //printf("Density Map Finished \n");
}

void map_density_visualize(int density, float min, float max) {
    int i,j,k;
    float d;
    float x,y,z;
    int c=0, _c=0;
    if(MAPE == NULL) {
        printf("map_density_visualize: MAPE is null!\n");
        return;
    }
    glBegin(GL_POINTS);

    for(i=0; i<me_xdim; i++) {
    for(j=0; j<me_ydim; j++) {
    for(k=0; k<me_zdim; k++) {
        _c++;
        //printf("%i, %i, %i\n",i,j,k);
        d = MAPE[i+ j*me_xdim + k*me_xdim*me_ydim].density;
        if(d < max && d > min) {
            glColor3ub((unsigned char)0,(unsigned char)64+191*d,(unsigned char)0);
            //printf("%i \n", 64+191*d);
            c++;
            x = i + 0.5;
            y = j + 0.5;
            z = k + 0.5;
            //printf("%f, %f, %f\n", x,y,z);
            glVertex3f(x,y,z);
        }
    }}}
    glEnd();
    printf("Drew %i of %i total density indicators\n", c, _c);

}
