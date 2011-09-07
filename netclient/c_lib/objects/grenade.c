#include "grenade.h"


struct Grenade {
    float x,y,z;
    float vx,vy,vz;
    unsigned int ttl;
    unsigned int ttl_max;
    int type;
};

struct Grenade* Grenade_list[1024];
float a[16];
int g_count=0;

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

    float interval;
    int* s;
    s = _ray_cast4(g->x, g->y, g->z, _x,_y,_z, &interval);
    //printf("interval= %f \n", interval);

    if(s[0] != 0 ) {
        g->vx *= -1;
        //printf("invert vx \n");
    }
    if(s[1] != 0) {
        g->vy *= -1;
        //printf("invert vy \n");
    }
    if(s[2] != 0) {
        g->vz *= -1;
        //printf("invert vz \n");
    }

    g->x = g->x + interval*g->vx/30;
    g->y = g->y + interval*g->vy/30;
    g->z = g->z + interval*g->vz/30;
}

void grenade_tick() {
    struct Grenade* g = NULL;
    int i;
    for(i=0; i<1024; i++) {
        if(Grenade_list[i] != NULL) {
            g = Grenade_list[i];
            grenade_Tick(g);
            //if(g->ttl >= g-> ttl_max) {
                ////boom!
                //Grenade_list[i] = NULL;
                //free(g);
                //g_count--;
            //}
        }
    }


}

int create_grenade(int type, float x, float y, float z, float vx, float vy, float vz, unsigned int ttl, unsigned int ttl_max) {
    //printf("Create Gernade\n");
    struct Grenade* g = NULL;
    int i=0;
    for(i=0; i<1024; i++) {
        if(Grenade_list[i] == NULL) {
            g = (struct Grenade *) malloc (sizeof(struct Grenade));
            Grenade_list[i] = g;
            g_count++;
            break;
        }
    }
    if(g== NULL) { printf("Bug: max grenade number reached!\n"); return;}
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
    struct Grenade *g = Grenade_list[gid];
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

    struct Grenade* g = NULL;
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
