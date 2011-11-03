#include "minivox.hpp"
#pragma once

Minivox::Minivox(int id) {
    create_particle2(&particle, id, MINIVOX_TYPE, 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f, 0, MINIVOX_TTL);
    size = MINIVOX_SIZE;
    //vox = (Voxel *)malloc(sizeof(Voxel));
    vox = MINIVOX_DEFAULT_VOXEL;
}

Minivox::Minivox(int id, float x, float y, float z, float vx, float vy, float vz) {
    create_particle2(&particle, id, MINIVOX_TYPE, x,y,z,vx,vy,vz, 0, MINIVOX_TTL);
    size = MINIVOX_SIZE;
    //vox = (Voxel *)malloc(sizeof(Voxel));
    vox = MINIVOX_DEFAULT_VOXEL;
}

Minivox::~Minivox() {
    //free(vox);
}

void Minivox::set_size(float s) {
    size = s;
}

void Minivox::set_color(unsigned char r, unsigned char g, unsigned char b) {
    vox.r = r;
    vox.g = g;
    vox.b = b;
}

void Minivox::set_color(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    vox.r = r;
    vox.g = g;
    vox.b = b;
    vox.a = a;
}

void Minivox::tick() {
    bounce_simple_rk4(&particle, MINIVOX_DAMP);
    particle.ttl++;
}

void Minivox::draw() {

#ifdef DC_CLIENT

    float up[3] = {
        minivox_proj_mtrx[0]*MINIVOX_TEXTURE_SCALE,
        minivox_proj_mtrx[4]*MINIVOX_TEXTURE_SCALE,
        minivox_proj_mtrx[8]*MINIVOX_TEXTURE_SCALE
    };
    float right[3] = {
        minivox_proj_mtrx[1]*MINIVOX_TEXTURE_SCALE,
        minivox_proj_mtrx[5]*MINIVOX_TEXTURE_SCALE,
        minivox_proj_mtrx[9]*MINIVOX_TEXTURE_SCALE
    };

    float tx_min, tx_max, ty_min, ty_max;
    float x,y,z;

    tx_min = (float)(MINIVOX_TEXTURE_ID%16)* (1.0/16.0);
    tx_max = tx_min + (1.0/16.0);
    ty_min = (float)(MINIVOX_TEXTURE_ID/16)* (1.0/16.0);
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

#endif    
}


/* Minivox list */

void Minivox_list::tick() {
    int i;
    for (i=0; i<n_max; i++) {
        if (a[i] == NULL) continue;
        a[i]->tick();
        if (a[i]->particle.ttl >= a[i]->particle.ttl_max) {
            destroy(a[i]->particle.id);
        }
    }
}

void Minivox_list::draw() {
#ifdef DC_CLIENT

    if(num == 0) { return; }

    glDisable(GL_TEXTURE_2D);
    glEnable (GL_DEPTH_TEST);

    glBegin(GL_QUADS);


    int i;
    for(i=0; i<n_max; i++) {
        if (a[i] == NULL) continue;
        a[i]->draw();
    }

    glEnd();
    glDisable (GL_DEPTH_TEST);
#endif
}


#ifdef DC_CLIENT
static int v_set[3*8] = {
        0,0,0,
        1,0,0,
        1,1,0,
        0,1,0,
        0,0,1,
        1,0,1,
        1,1,1,
        0,1,1 };

static int q_set[4*6]= {
        4,5,6,7,
        3,2,1,0,
        2,3,7,6,
        0,1,5,4,
        0,4,7,3,
        1,2,6,5 };

static float v_buffer[3*8];
static float s_buffer[6*(4*3)];
#endif


void Vox::draw(struct Vector forward, struct Vector right, float x, float y, float z) {
#ifdef DC_CLIENT

    if(vox.a == 0) return;

    int i,j,k;
    int i1;
    int index;

    float vos = vox_size;

    struct Vector vx,vy,vz;

    //vx = f; //instead of look direction
    vx = forward; //instead of look direction
    vz = vector_cross(vx, right);
    vy = vector_cross(vx, vz);

    //save 3 multiplications per voxel by premultiplying in vox
    vx.x *= vos; vx.y *= vos; vx.z *= vos;
    vy.x *= vos; vy.y *= vos; vy.z *= vos;
    vz.x *= vos; vz.y *= vos; vz.z *= vos;

    for(i=0; i<8; i++) {
        v_buffer[3*i+0] = v_set[3*i+0]*vx.x + v_set[3*i+1]*vy.x + v_set[3*i+2]*vz.x ;
        v_buffer[3*i+1] = v_set[3*i+0]*vx.y + v_set[3*i+1]*vy.y + v_set[3*i+2]*vz.y ;
        v_buffer[3*i+2] = v_set[3*i+0]*vx.z + v_set[3*i+1]*vy.z + v_set[3*i+2]*vz.z ;
    }
    for(i=0; i<6; i++) {
        for(j=0; j<4; j++) {
            s_buffer[12*i+3*j+0] = v_buffer[3*q_set[4*i+j] + 0];
            s_buffer[12*i+3*j+1] = v_buffer[3*q_set[4*i+j] + 1];
            s_buffer[12*i+3*j+2] = v_buffer[3*q_set[4*i+j] + 2];
        }
    }

    struct Vector c;
    float cx,cy,cz;

    c.x = anchor.x + length*forward.x;
    c.y = anchor.y + length*forward.y;
    c.z = anchor.z + length*forward.z;

    c.x += x;
    c.y += y;
    c.z += z;

    cx = c.x - (xdim*vx.x + ydim*vy.x + zdim*vz.x)/2;
    cy = c.y - (xdim*vx.y + ydim*vy.y + zdim*vz.y)/2;
    cz = c.z - (xdim*vx.z + ydim*vy.z + zdim*vz.z)/2;

    float x0, y0, z0;

    glColor3ub((unsigned char) vox.r,(unsigned char)vox.g,(unsigned char)vox.b);

    x0 = cx + (i*vx.x + j*vy.x + k*vz.x);
    y0 = cy + (i*vx.y + j*vy.y + k*vz.y);
    z0 = cz + (i*vx.z + j*vy.z + k*vz.z);

    for(i1=0; i1<6; i1++) {
        glVertex3f(x0 + s_buffer[12*i1+3*0+0], y0+ s_buffer[12*i1+3*0+1], z0+ s_buffer[12*i1+3*0+2]);
        glVertex3f(x0 + s_buffer[12*i1+3*1+0], y0+ s_buffer[12*i1+3*1+1], z0+ s_buffer[12*i1+3*1+2]);
        glVertex3f(x0 + s_buffer[12*i1+3*2+0], y0+ s_buffer[12*i1+3*2+1], z0+ s_buffer[12*i1+3*2+2]);
        glVertex3f(x0 + s_buffer[12*i1+3*3+0], y0+ s_buffer[12*i1+3*3+1], z0+ s_buffer[12*i1+3*3+2]);
    }

    }}}
    glEnd();

    glDisable (GL_DEPTH_TEST);

#endif
}


