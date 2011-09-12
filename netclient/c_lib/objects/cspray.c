#include "cspray.h"


struct Cspray {
    struct Particle particle;
    int active;
};

#define max_cspray 4096

struct Cspray* cspray_list[max_cspray];
float a[16];
int cspray_count=0;
unsigned int cspray_id=0;

void inline cspray_Tick(struct Cspray* g);

void init_objects_cspray() {
    //printf("RANDMAX= %i \n", RAND_MAX);
    //srand(15); //seed
    return;
}

void inline cspray_Tick(struct Cspray* g) {
    //g->particle.vz -= 0.025; //gravity

    g->particle.ttl++;

    int* s;
    int collision[3];
    int tile;

    //s = bounce_collide_tile(&(g->particle), collision, &tile);
    //int n = _GET_MS_TIME();
    s = bounce_collide_tile_rk4(&(g->particle), collision, &tile, 0.0f);
    //printf("CSPRAY RK4 %d\n", _GET_MS_TIME() - n);

    // cement effect
    if(g->active == 1) {
        g->particle.ttl= g->particle.ttl_max;
        //tile = _get(collision,g->particle.collision[1],g->particle.collision[2]);
        if(!isActive(tile)) {
            _set(collision[0],collision[1],collision[2], 2);
            g->particle.ttl= g->particle.ttl_max;
            return;
        }
    }

    if(s[0] != 0 || s[1] != 0 || s[2] != 0)
    {
        if(isActive(tile)) {
            g->active=1;
        }
    }
}

static inline void cspray_Free(struct Cspray* g) {
    //free(g->particle);
    free(g);
}

void cspray_tick() {
    struct Cspray* g = NULL;
    int i;
    for(i=0; i<max_cspray; i++) {
        if(cspray_list[i] != NULL) {
            g = cspray_list[i];
            cspray_Tick(g);
            if(g->particle.ttl >= g->particle. ttl_max) {
                //boom!
                cspray_list[i] = NULL;
                cspray_Free(g);
                cspray_count--;
            }
        }
    }


}


void create_cspray(int type, float x, float y, float z, float vx, float vy, float vz) {
    //printf("Create cspray\n");
    struct Cspray* g = NULL;
    int i;
    for(i=0; i<max_cspray; i++) {
        if(cspray_list[i] == NULL) {
            g = (struct Cspray *) malloc (sizeof(struct Cspray));
            cspray_list[i] = g;
            cspray_count++;
            break;
        }
    }
    if(g== NULL) {
        //printf("Bug: max cspray number reached!\n");
        return;}

    //struct Particle* p = (struct Particle*) malloc (sizeof(struct Particle));
    create_particle(&(g->particle), (unsigned int)i, type, x,y,z, vx,vy,vz, 0, 1200);
    //g->particle = p;
    g->active = 0;
}


/*
 *  Client only
 */
#ifdef DC_CLIENT

//GLint particle_sheet_id;
void cspray_draw() {
    //printf("particle sheet id= %i \n", get_particle_texture() );
    if(cspray_count == 0) { return; }
    glGetFloatv(GL_MODELVIEW_MATRIX, a);

    struct Cspray* g = NULL;
    int i;

    float size = 0.3;
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
    for(i=0; i<max_cspray; i++) {
    if(cspray_list[i] != NULL) {
        //printf("draw cspray: %i \n", i);
        _c++;
        g = cspray_list[i];
        //draw setup
        id = 21;
        tx_min = (float)(id%16)* (1.0/16.0);
        tx_max = tx_min + (1.0/16.0);
        ty_min = (float)(id/16)* (1.0/16.0);
        ty_max = ty_min + (1.0/16.0);

        x=g->particle.x; y=g->particle.y; z=g->particle.z;

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
