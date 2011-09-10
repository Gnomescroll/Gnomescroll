#include "common.h"

int* bounce_simple(struct Particle* p) {

    float _x, _y, _z;
    _x = p->x + p->vx/FPS;
    _y = p->y + p->vy/FPS;
    _z = p->z + p->vz/FPS;

    float interval;
    int* s;
    s = _ray_cast4(p->x, p->y, p->z, _x,_y,_z, &interval);
    //printf("interval= %f \n", interval);

    if(s[0] != 0 ) {
        p->vx *= -1;
        //printf("invert vx \n");
    }
    if(s[1] != 0) {
        p->vy *= -1;
        //printf("invert vy \n");
    }
    if(s[2] != 0) {
        p->vz *= -1;
        //printf("invert vz \n");
    }

    p->x = p->x + interval*p->vx/FPS;
    p->y = p->y + interval*p->vy/FPS;
    p->z = p->z + interval*p->vz/FPS;
    return s;
}

int* bounce_collide_tile(struct Particle* p, int* collision, int* tile) {

    float _x, _y, _z;
    _x = p->x + p->vx/FPS;
    _y = p->y + p->vy/FPS;
    _z = p->z + p->vz/FPS;

    float interval;
    int* s;
    s = _ray_cast5(p->x, p->y, p->z, _x,_y,_z, &interval, collision, tile);

    if(s[0] != 0 ) {
        p->vx *= -1;
        //printf("invert vx \n");
    }
    if(s[1] != 0) {
        p->vy *= -1;
        //printf("invert vy \n");
    }
    if(s[2] != 0) {
        p->vz *= -1;
        //printf("invert vz \n");
    }
    
    p->x = p->x + interval*p->vx/FPS;
    p->y = p->y + interval*p->vy/FPS;
    p->z = p->z + interval*p->vz/FPS;

    return s;
}
