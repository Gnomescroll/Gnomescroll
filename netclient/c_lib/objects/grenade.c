#include "grenade.h"


struct Particle* Grenade_list[1024];
float a[16];
int g_count=0;

void inline grenade_Tick(struct Particle* g);

void init_objects_grenade() {
    return;
}

void inline grenade_Tick(struct Particle* g) {
    g->ttl++;
    bounce_simple(g);
}

void grenade_tick() {
    struct Particle* g = NULL;
    int i;
    for(i=0; i<1024; i++) {
        if(Grenade_list[i] != NULL) {
            g = Grenade_list[i];
            grenade_Tick(g);
            // destruction handled at python level
            //if(g->ttl >= g->ttl_max) {
                //destroy_grenade(i);
            //}
        }
    }
}

struct Vector g_pos;
struct Vector* _get_grenade_position(int gid) {
    struct Particle* g = Grenade_list[gid];
    if (g != NULL) {
        g_pos.x = g->x;
        g_pos.y = g->y;
        g_pos.z = g->z;
    } else {
        g_pos.x = 0.0f;
        g_pos.y = 0.0f;
        g_pos.z = 0.0f;
    }
    return &g_pos;
}

int create_grenade(int type, float x, float y, float z, float vx, float vy, float vz, unsigned int ttl, unsigned int ttl_max) {
    //printf("Create Gernade\n");
    struct Particle* g = NULL;
    int i=0;
    for(i=0; i<1024; i++) {
        if(Grenade_list[i] == NULL) {
            g = (struct Particle *) malloc (sizeof(struct Particle));
            Grenade_list[i] = g;
            g_count++;
            break;
        }
    }
    if(g== NULL) { printf("Bug: max grenade number reached!\n"); return -1;}
    g->x=x;
    g->y=y;
    g->z=z;
    g->vx=vx;
    g->vy=vy;
    g->vz=vz;
    g->ttl = ttl;
    g->ttl_max = ttl_max;
    g->type = 1;
    return i;
}

void destroy_grenade(int gid) {
    struct Particle *g = Grenade_list[gid];
    Grenade_list[gid] = NULL;
    free(g);
    g_count--;
}


/*
 *  Client only
 */
#ifdef DC_CLIENT

//GLint particle_sheet_id;
void grenade_draw() {
    //printf("particle sheet id= %i \n", get_particle_texture() );
    if(g_count == 0) { return; }
    glGetFloatv(GL_MODELVIEW_MATRIX, a);

    struct Particle* g = NULL;
    int i;

    float up[3] = {a[0], a[4], a[8]};
    float right[3] = {a[1], a[5], a[9]};
    int id = 5;

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
    for(i=0; i<1024; i++) {
    if(Grenade_list[i] != NULL) {
        //printf("drew gernade: %i \n", i);
        _c++;
        g = Grenade_list[i];
        //draw setup

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
