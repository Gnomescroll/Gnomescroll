#include "shrapnel.h"


#define max_shrapnel 4096

struct Particle* shrapnel_list[max_shrapnel];
//float a[16];
int shrapnel_count=0;
unsigned int shrapnel_id=0;

void inline shrapnel_Tick(struct Particle* g);

void init_objects_shrapnel() {
    //printf("RANDMAX= %i \n", RAND_MAX);
    //srand(15); //seed
    return;
}

void inline shrapnel_Tick(struct Particle* g) {
    g->vz -= 0.025; //gravity
    g->ttl++;

    bounce_simple(g);
}

void shrapnel_tick() {
    struct Particle* g = NULL;
    int i;
    for(i=0; i<max_shrapnel; i++) {
        if(shrapnel_list[i] != NULL) {
            g = shrapnel_list[i];
            shrapnel_Tick(g);
            if(g->ttl >= g-> ttl_max) {
                //boom!
                shrapnel_list[i] = NULL;
                free(g);
                shrapnel_count--;
            }
        }
    }
}


void create_shrapnel(int type, float x, float y, float z, float vx, float vy, float vz) {
    //printf("Create shrapnel\n");
    struct Particle* g = NULL;
    int i;
    for(i=0; i<max_shrapnel; i++) {
        if(shrapnel_list[i] == NULL) {
            g = (struct Particle *) malloc (sizeof(struct Particle));
            shrapnel_list[i] = g;
            shrapnel_count++;
            break;
        }
    }
    if(g== NULL) {
        //printf("Bug: max shrapnel number reached!\n");
        return;}
    create_particle(g, (unsigned int)i, type, x,y,z, vx,vy,vz, 0, 30);
}


/*
 *  Client only
 */

#ifdef DC_CLIENT

//GLint particle_sheet_id;
void shrapnel_draw() {
    //printf("particle sheet id= %i \n", get_particle_texture() );
    if(shrapnel_count == 0) { return; }
    glGetFloatv(GL_MODELVIEW_MATRIX, a);

    struct Particle* g = NULL;
    int i;

    float size = 0.1;
    float up[3] = {a[0]*size, a[4]*size, a[8]*size};
    float right[3] = {a[1]*size, a[5]*size, a[9]*size};
    int id;

    float tx_min, tx_max, ty_min, ty_max;

    //should not change state unless there is something to draw
    glEnable(GL_TEXTURE_2D);
    glEnable (GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    glBindTexture( GL_TEXTURE_2D, get_particle_texture() );
    //printf("particle sheet= %i \n", particle_sheet_id);
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE);

    glBegin( GL_QUADS );
    float x,y,z;

    int _c = 0;
    for(i=0; i<max_shrapnel; i++) {
    if(shrapnel_list[i] != NULL) {
        //printf("draw shrapnel: %i \n", i);
        _c++;
        g = shrapnel_list[i];
        //draw setup

        id = 63;    // location in spritesheet
        tx_min = (float)(id%16)* (1.0/16.0);
        tx_max = tx_min + (1.0/16.0);
        ty_min = (float)(id/16)* (1.0/16.0);
        ty_max = ty_min + (1.0/16.0);

        x=g->x; y=g->y; z=g->z;

        glTexCoord2f(tx_min,ty_max );
        glVertex3f(x+(-right[0]-up[0]), y+(-right[1]-up[1]), z+(-right[2]-up[2]));  // Bottom left

        glTexCoord2f(tx_min,ty_min );
        glVertex3f(x+(up[0]-right[0]), y+(up[1]-right[1]), z+(up[2]-right[2]));  // Top left

        glTexCoord2f(tx_max,ty_min);
        glVertex3f(x+(up[0]+right[0]), y+(up[1]+right[1]), z+(up[2]+right[2]));  // Top right

        glTexCoord2f(tx_max,ty_max );
        glVertex3f(x+(right[0]-up[0]), y+(right[1]-up[1]), z+(right[2]-up[2]));  // Bottom right
        }
    }
    //printf("drew %i gernades\n", _c);
    glEnd();

    glDepthMask(GL_TRUE);
    glDisable(GL_TEXTURE_2D);
    glDisable (GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}

#endif

