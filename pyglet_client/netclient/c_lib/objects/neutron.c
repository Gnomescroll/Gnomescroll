#include "neutron.h"


struct neutron {
    unsigned int id;
    float x,y,z;
    float vx,vy,vz;
    uint ttl;
    uint ttl_max;
    int type;
    int energy;
    uint event_ttl;
};

struct neutron* neutron_list[1024];
float a[16];
int neutron_count=0;
unsigned int neutron_id=0;

void inline neutron_Tick(struct neutron* g);

void init_objects_neutron() {
    printf("RANDMAX= %i \n", RAND_MAX);
    srand(15); //seed
    return;
}

void inline neutron_Tick(struct neutron* g) {
    g->ttl++;
    float _x, _y, _z;
    _x = g->x + g->vx/30;
    _y = g->y + g->vy/30;
    _z = g->z + g->vz/30;

    float interval;
    int* s;
    int collision[3];
    int tile;

    float _vx,_vy,_vz;
    float len;
    float vel = 2;
    s = _ray_cast5(g->x, g->y, g->z, _x,_y,_z, &interval, collision, &tile);
    //printf("interval= %f \n", interval);
    //printf("collides %i, %i, %i \n", collision[0],collision[1],collision[2]);

    if(g->energy==3) {
        g->event_ttl--;
        if(g->event_ttl == 0) {
            create_neutron(g->type, 1, g->x,g->y,g->z, g->vx,g->vy,g->vz);
            create_neutron(g->type, 1, g->x,g->y,g->z, -g->vx,-g->vy,-g->vz);
        }

    }

    if(s[0] != 0 || s[1] != 0 || s[2] != 0)
    {
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
        if(isNuclear(tile)) {
            g->ttl=0; //reset TTL
            if(g->energy ==3) {
                g->event_ttl = 0;


                _vx = (float)rand()/(float)RAND_MAX;
                _vx = (float)rand()/(float)RAND_MAX;
                _vx = (float)rand()/(float)RAND_MAX;
                len = sqrt(_vx*_vx+_vy*_vy+_vz*_vz);
                _vx *= vel/len;
                _vy *= vel/len;
                _vz *= vel/len;

                create_neutron(g->type, 1, g->x,g->y,g->z, _vx,_vy,_vz);
                create_neutron(g->type, 1, g->x,g->y,g->z, -_vx,-_vy,-_vz);
            }
            if(g->energy < 3) {
                g->energy++;

                if(g->energy==3) {
                    g->event_ttl= 60;
                }
            }
        }
    }
    g->x = g->x + interval*g->vx/30;
    g->y = g->y + interval*g->vy/30;
    g->z = g->z + interval*g->vz/30;
}

void neutron_tick() {
    struct neutron* g = NULL;
    int i;
    for(i=0; i<1024; i++) {
        if(neutron_list[i] != NULL) {
            g = neutron_list[i];
            neutron_Tick(g);
            if(g->ttl >= g-> ttl_max || g->event_ttl == 0) {
                //boom!
                neutron_list[i] = NULL;
                free(g);
                neutron_count--;
            }
        }
    }


}

//GLint particle_sheet_id;

void neutron_draw() {
    //printf("particle sheet id= %i \n", get_particle_texture() );
    if(neutron_count == 0) { return; }
    glGetFloatv(GL_MODELVIEW_MATRIX, a);

    struct neutron* g = NULL;
    int i;

    float size = 0.3;
    float up[3] = {a[0]*size, a[4]*size, a[8]*size};
    float right[3] = {a[1]*size, a[5]*size, a[9]*size};
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
    if(neutron_list[i] != NULL) {
        //printf("draw neutron: %i \n", i);
        _c++;
        g = neutron_list[i];
        //draw setup
        id = 48+3*g->type+ (g->energy-1);
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



void create_neutron(int type, int energy, float x, float y, float z, float vx, float vy, float vz) {
    //printf("Create neutron\n");
    struct neutron* g = NULL;
    int i;
    for(i=0; i<1024; i++) {
        if(neutron_list[i] == NULL) {
            g = (struct neutron *) malloc (sizeof(struct neutron));
            neutron_list[i] = g;
            neutron_count++;
            break;
        }
    }
    if(g== NULL) {
        //printf("Bug: max neutron number reached!\n");
        return;}
    g->x=x;
    g->y=y;
    g->z=z;
    g->vx=vx;
    g->vy=vy;
    g->vz=vz;
    g->ttl = 0;
    g->ttl_max = 600;
    g->energy = energy;
    g->type = type;
    if(energy == 3) {
        g->event_ttl = 150;
    } else {
        g->event_ttl = 1;
    }
}
