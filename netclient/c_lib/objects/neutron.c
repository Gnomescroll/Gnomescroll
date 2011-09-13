#include "neutron.h"

#define max_neutron 1024

//struct Neutron {
    //unsigned int id;
    //float x,y,z;
    //float vx,vy,vz;
    //unsigned int ttl;
    //unsigned int ttl_max;
    //int type;
    //int energy;
    //unsigned int event_ttl;
//};


//struct Neutron {
    ////struct EventParticle* particle;
    //struct EventParticle particle;
    //int energy;
//};

struct Neutron {
   struct EventParticle2 particle;
   int energy;
};

struct Neutron* neutron_list[max_neutron];
float a[16];
int neutron_count=0;
unsigned int neutron_id=0;

void inline neutron_Tick(struct Neutron* g);

void init_objects_neutron() {
    printf("RANDMAX= %i \n", RAND_MAX);
    srand(15); //seed
    return;
}

void inline neutron_Tick(struct Neutron* g) {
    struct EventParticle2* ep = &(g->particle);
    struct Particle2* p = &(g->particle.particle);
    

    //float _x, _y, _z;
    //_x = p.x + p.vx/30;
    //_y = p.y + p.vy/30;
    //_z = p.z + p.vz/30;

    //float interval;
    int* s;
    int collision[3];
    int tile;

    //float _vx,_vy,_vz;
    //float len;
    //float vel = 2;
    //s = _ray_cast5(p.x, p.y, p.z, _x,_y,_z, &interval, collision, &tile);
    //printf("interval= %f \n", interval);
    //printf("collides %i, %i, %i \n", collision[0],collision[1],collision[2]);

//printf("%f, %f, %f\n", p.state.p.x, p.state.p.y, p.state.p.z);
    s = bounce_collide_tile_rk4(p, collision, &tile, 1.0f);
//printf("%f, %f, %f\n", p.state.p.x, p.state.p.y, p.state.p.z);
//printf("\n");
//if (p->id == 2){
//printf("-tick\n");
        //printf("%f,%f,%f\n", p->state.p.x, p->state.p.y, p->state.p.z);}
    if(g->energy==3) {
        ep->event_ttl--;
        if(ep->event_ttl == 0) {
            create_neutron(p->type, 1, p->state.p.x,p->state.p.y,p->state.p.z, p->state.v.x,p->state.v.y,p->state.v.z);
            create_neutron(p->type, 1, p->state.p.x,p->state.p.y,p->state.p.z, -p->state.v.x,-p->state.v.y,-p->state.v.z);
        }

    }

    if(s[0] != 0 || s[1] != 0 || s[2] != 0)
    {
        //if(s[0] != 0 ) {
            //p.vx *= -1;
            ////printf("invert vx \n");
        //}
        //if(s[1] != 0) {
            //p.vy *= -1;
            ////printf("invert vy \n");
        //}
        //if(s[2] != 0) {
            //p.vz *= -1;
            ////printf("invert vz \n");
        //}
        if(isNuclear(tile)) {
            p->ttl=0; //reset TTL
            if(g->energy ==3) {
                ep->event_ttl = 0;

                float _vx, _vy, _vz, len, vel=2;

                _vx = (float)rand()/(float)RAND_MAX;
                _vy = (float)rand()/(float)RAND_MAX;
                _vz = (float)rand()/(float)RAND_MAX;
                len = sqrt(_vx*_vx+_vy*_vy+_vz*_vz);
                _vx *= vel/len;
                _vy *= vel/len;
                _vz *= vel/len;

                create_neutron(p->type, 1, p->state.p.x,p->state.p.y,p->state.p.z, _vx,_vy,_vz);
                create_neutron(p->type, 1, p->state.p.x,p->state.p.y,p->state.p.z, -_vx,-_vy,-_vz);
            }
            if(g->energy < 3) {
                g->energy++;

                if(g->energy==3) {
                    ep->event_ttl= 60;
                }
            }
        }
    }
    //p.x = p.x + interval*p.vx/30;
    //p.y = p.y + interval*p.vy/30;
    //p.z = p.z + interval*p.vz/30;
    p->ttl++;

    //if (p->id == 2){
        //printf("%f,%f,%f\n", g->particle.particle.state.p.x, g->particle.particle.state.p.y, g->particle.particle.state.p.z);}

}

void inline neutron_Free(struct Neutron* g) {
    //free_event_particle(g->particle);
    free(g);
}

void neutron_tick() {
    struct Neutron* g = NULL;
    int i;
    for(i=0; i<max_neutron; i++) {
        if(neutron_list[i] != NULL) {
            g = neutron_list[i];
            neutron_Tick(g);
            if(g->particle.particle.ttl >= g->particle.particle.ttl_max || g->particle.event_ttl == 0) {
                //boom!
                neutron_list[i] = NULL;
                neutron_Free(g);
                neutron_count--;
            }
        }
    }


}

void create_neutron(int type, int energy, float x, float y, float z, float vx, float vy, float vz) {
    //printf("Create neutron\n");
    struct Neutron* g = NULL;
    int i;
    for(i=0; i<max_neutron; i++) {
        if(neutron_list[i] == NULL) {
            g = (struct Neutron *) malloc (sizeof(struct Neutron));
            neutron_list[i] = g;
            neutron_count++;
            break;
        }
    }
    if(g== NULL) {
        //printf("Bug: max neutron number reached!\n");
        return;}

    //struct EventParticle* p = (struct EventParticle *) malloc (sizeof(struct EventParticle));
    int event_ttl = 1;
    if (energy == 3) {
        event_ttl = 150;
    }
    create_event_particle2(&(g->particle), (unsigned int)i, type, x,y,z, vx,vy,vz, 0, 600, event_ttl);
    
    //g->particle = p;
    g->energy = energy;
}


/*
 *  Client only
 */
#ifdef DC_CLIENT

//GLint particle_sheet_id;
void neutron_draw() {
    //printf("particle sheet id= %i \n", get_particle_texture() );
    if(neutron_count == 0) { return; }
    glGetFloatv(GL_MODELVIEW_MATRIX, a);

    struct Neutron* g = NULL;
    int i;

    float size = 0.3;
    float up[3] = {a[0]*size, a[4]*size, a[8]*size};
    float right[3] = {a[1]*size, a[5]*size, a[9]*size};
    int id = 21;

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
    for(i=0; i<max_neutron; i++) {
    if(neutron_list[i] != NULL) {
        //printf("draw neutron: %i \n", i);
        _c++;
        g = neutron_list[i];
        //draw setup
        //id = 48+3*g->particle.particle.type+ (g->energy-1);
        tx_min = (float)(id%16)* (1.0/16.0);
        tx_max = tx_min + (1.0/16.0);
        ty_min = (float)(id/16)* (1.0/16.0);
        ty_max = ty_min + (1.0/16.0);

        x=g->particle.particle.state.p.x; y=g->particle.particle.state.p.y; z=g->particle.particle.state.p.z;
//printf("Draw neutron at\n");
//printf("%f, %f, %f\n", x,y,z);
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
