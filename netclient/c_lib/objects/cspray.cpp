#include "cspray.hpp"

#pragma once


//struct Cspray* cspray_list[CSPRAY_MAX];
float cspray_proj_mtrx[16];
//int cspray_count=0;
//unsigned int cspray_id=0;

//void inline cspray_Tick(struct Cspray* g);

Cspray::Cspray(int _id) {

    active = 0;
    stopped = 0;

    create_particle2(&particle, _id, CSPRAY_TYPE, 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f, 0, CSPRAY_TTL);
                   // particle, _id,      type,           x,y,z,         vx,vy,vz,   ttl,  ttl_max
}

//void Cspray::draw() {}
void Cspray::client_tick() {}
void Cspray::server_tick() {}

void Cspray::_tick() {

    particle.ttl++;

    int* s;
    int collision[3];
    int tile;

    s = bounce_collide_tile_rk4(&(particle), collision, &tile, CSPRAY_DAMP);

    // cement effect
    if(active == 1) {
        particle.ttl= particle.ttl_max;
        if(!isActive(tile)) {
            _set(collision[0],collision[1],collision[2], CSPRAY_CEMENT_BLOCK_TYPE);    // create block
            particle.ttl = particle.ttl_max;
            return;
        }
    }

    if(s[0] != 0 || s[1] != 0 || s[2] != 0)
    {
        if(isActive(tile)) {
            active=1;
            particle.ttl *= 2;
        }
    }
}


void Cspray_list::server_tick() {

    int i;
    for (i=0; i<n_max; i++) {

        if (a[i] == NULL) continue;
        a[i]->_tick();
        if (a[i]->particle.ttl >= a[i]->particle.ttl_max) {
            destroy(a[i]->particle.id);
        }

    }
}

//void init_objects_cspray() {
    ////printf("RANDMAX= %i \n", RAND_MAX);
    ////srand(15); //seed
    //return;
//}

//void inline cspray_Tick(struct Cspray* g) {
    ////g->particle.vz -= 0.025; //gravity

    //g->particle.ttl++;

    //int* s;
    //int collision[3];
    //int tile;

   ////s = bounce_collide_tile(&(g->particle), collision, &tile);
    ////int n = _GET_MS_TIME();
    //s = bounce_collide_tile_rk4(&(g->particle), collision, &tile, 0.0f);
    ////printf("CSPRAY RK4 %d\n", _GET_MS_TIME() - n);

    //// cement effect
    //if(g->active == 1) {
        //g->particle.ttl= g->particle.ttl_max;
        ////tile = _get(collision,g->particle.collision[1],g->particle.collision[2]);
        //if(!isActive(tile)) {
            //_set(collision[0],collision[1],collision[2], 2);
            //g->particle.ttl= g->particle.ttl_max;
            //return;
        //}
    //}

    //if(s[0] != 0 || s[1] != 0 || s[2] != 0)
    //{
        //if(isActive(tile)) {
            //g->active=1;
            //g->particle.ttl *= 2;
        //}
    //}
//}

//static inline void cspray_Free(struct Cspray* g) {
    ////free(g->particle);
    //free(g);
//}

//void cspray_tick() {
////int n = _GET_MS_TIME();
    
    //struct Cspray* g = NULL;
    //int i;
    //for(i=0; i<CSPRAY_MAX; i++) {
        //if(cspray_list[i] != NULL) {
            //g = cspray_list[i];
            //cspray_Tick(g);
            //if(g->particle.ttl >= g->particle.ttl_max) {
                ////boom!
                //cspray_list[i] = NULL;
                //cspray_Free(g);
                //cspray_count--;
            //}
        //}
    //}
    ////int n2 = _GET_MS_TIME();
    //////if (n2-n > 0) {
        ////printf("cspray_tick :: %d\n", n2-n);
    //////}


//}


//void create_cspray(int type, float x, float y, float z, float vx, float vy, float vz) {
    ////return;
    ////printf("Create cspray\n");
    //struct Cspray* g = NULL;
    //int i;
    //for(i=0; i<CSPRAY_MAX; i++) {
        //if(cspray_list[i] == NULL) {
            //g = (struct Cspray *) malloc (sizeof(struct Cspray));
            //cspray_list[i] = g;
            //cspray_count++;
            //break;
        //}
    //}
    //if(g== NULL) {
        //return;
    //}

    //create_particle2(&(g->particle), (unsigned int)i, type, x,y,z, vx,vy,vz, 0, 1200);
    //g->active = 0;
//}

/*
 *  Networking; spawn packet from server to client
 */

#include <c_lib/template/net.hpp>

class cspray_StoC: public FixedSizeNetPacketToClient<cspray_StoC>
{
    public:

        float x,y,z;
        float vx,vy,vz;
        uint16_t ttl;
        uint16_t ttl_max;
        uint16_t id;
        uint8_t type;

        inline void packet(unsigned char* buff, int* buff_n, bool pack) 
        {
            pack_float(&x, buff, buff_n, pack);
            pack_float(&y, buff, buff_n, pack);
            pack_float(&z, buff, buff_n, pack);

            pack_float(&vx, buff, buff_n, pack);
            pack_float(&vy, buff, buff_n, pack);
            pack_float(&vz, buff, buff_n, pack);

            pack_u16(&id, buff, buff_n, pack);
            pack_u16(&ttl, buff, buff_n, pack);
            pack_u16(&ttl_max, buff, buff_n, pack);
            pack_u8(&type, buff, buff_n, pack);
        }

        inline void handle() {
            printf("Spawn cspray particle: %i \n", id);
        }
};



/*
 *  Client only
 */


void Cspray_list::draw() {
    if (num==0) return;

#ifdef DC_CLIENT
    glEnable(GL_TEXTURE_2D);
    glEnable (GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    glBindTexture( GL_TEXTURE_2D, get_particle_texture() );
    //printf("particle sheet= %i \n", particle_sheet_id);
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE);

    glBegin( GL_QUADS );

    int i;
    for (i=0; i<n_max; i++) {
        a[i]->draw();
    }
    
    glEnd();

    glDepthMask(GL_TRUE);
    glDisable(GL_TEXTURE_2D);
    glDisable (GL_DEPTH_TEST);
    glDisable(GL_BLEND);

#endif
}

//GLint particle_sheet_id;
//void cspray_draw() {
void Cspray::draw() {
#ifdef DC_CLIENT
    //printf("particle sheet id= %i \n", get_particle_texture() );
    //if(cspray_count == 0) { return; }
    glGetFloatv(GL_MODELVIEW_MATRIX, cspray_proj_mtrx);

    //struct Cspray* g = NULL;
    //int i;

    float size = 0.3;
    float up[3] = {cspray_proj_mtrx[0]*size, cspray_proj_mtrx[4]*size, cspray_proj_mtrx[8]*size};
    float right[3] = {cspray_proj_mtrx[1]*size, cspray_proj_mtrx[5]*size, cspray_proj_mtrx[9]*size};
    int id=21;

    float tx_min, tx_max, ty_min, ty_max;

    float x,y,z;

    //int _c = 0;
    //for(i=0; i<CSPRAY_MAX; i++) {
    //if(cspray_list[i] != NULL) {
        ////printf("draw cspray: %i \n", i);
        //_c++;
        //g = cspray_list[i];
        //draw setup
        //id = 21;
        tx_min = (float)(id%16)* (1.0/16.0);
        tx_max = tx_min + (1.0/16.0);
        ty_min = (float)(id/16)* (1.0/16.0);
        ty_max = ty_min + (1.0/16.0);

        x=particle.state.p.x; y=particle.state.p.y; z=particle.state.p.z;

        glTexCoord2f(tx_min,ty_max );
        glVertex3f(x+(-right[0]-up[0]), y+(-right[1]-up[1]), z+(-right[2]-up[2]));  // Bottom left

        glTexCoord2f(tx_min,ty_min );
        glVertex3f(x+(up[0]-right[0]), y+(up[1]-right[1]), z+(up[2]-right[2]));  // Top left

        glTexCoord2f(tx_max,ty_min);
        glVertex3f(x+(up[0]+right[0]), y+(up[1]+right[1]), z+(up[2]+right[2]));  // Top right

        glTexCoord2f(tx_max,ty_max );
        glVertex3f(x+(right[0]-up[0]), y+(right[1]-up[1]), z+(right[2]-up[2]));  // Bottom right
        //}
    //}
    //printf("drew %i gernades\n", _c);
#endif
}

