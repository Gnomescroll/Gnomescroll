#include "grenade.h"


struct Grenade {
    float x,y,z;
    float vx,vy,vz;
    uint ttl;
    uint ttl_max;
    int type;
};

struct Grenade* Grenade_list[1024];

void inline grenade_Tick(struct Grenade* g);

void init_objects_grenade() {
    return;
}

void inline grenade_Tick(struct Grenade* g) {
    g->ttl++;
    float _x, _y, _z;
    _x = g->x + g->vx/30;
    _y = g->y + g->vy/30;
    _z = g->z + g->vz/30;

}

void grenade_tick() {
    struct Grenade* g = NULL;
    int i;
    for(i=0; i<1024; i++) {
        if(Grenade_list[i] != NULL) {
            g = Grenade_list[i];
            grenade_Tick(g);
            if(g->ttl >= g-> ttl_max) {
                //boom!
                Grenade_list[i] = NULL;
                free(g);
            }
        }
    }


}

void grenade_draw() {

}

void create_grenade(int type, float x, float y, float z, float vx, float vy, float vz) {
    struct Grenade* g = NULL;
    int i;
    for(i=0; i<1024; i++) {
        if(Grenade_list[i] == NULL) {
            g = (struct Grenade *) malloc (sizeof(struct Grenade));
            Grenade_list[i] = g;
        }
    }
    if(g== NULL) { printf("Bug: max grenade number reached!\n"); return;}
    g->x=x;
    g->y=y;
    g->z=z;
    g->vx=vx;
    g->vy=vy;
    g->vz=vz;
    g->ttl = 0;
    g->ttl_max = 330;
    g->type = 1;
}
