#include "minivox.hpp"
#pragma once

Minivox::Minivox(int id) {
    create_particle2(&particle, id, MINIVOX_TYPE, 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f, 0, MINIVOX_TTL);
    size = MINIVOX_SIZE;

    vox.r = MINIVOX_R;
    vox.g = MINIVOX_G;
    vox.b = MINIVOX_B;
    vox.a = MINIVOX_A;

    theta = 0.0f;
    phi = 0.0f;
    orient_vectors();
}

Minivox::Minivox(int id, float x, float y, float z, float vx, float vy, float vz) {
    create_particle2(&particle, id, MINIVOX_TYPE, x,y,z,vx,vy,vz, 0, MINIVOX_TTL);
    size = MINIVOX_SIZE;

    vox.r = MINIVOX_R;
    vox.g = MINIVOX_G;
    vox.b = MINIVOX_B;
    vox.a = MINIVOX_A;

    theta = 0.0f;
    phi = 0.0f;
    orient_vectors();
}

// recalculates orientation vectors from angular parameter
static Vector minivox_intermediate = {0.0f, 0.0f, 0.0f};
void Minivox::orient_vectors() {

    vec_x.x = cos(theta * PI);
    vec_x.y = sin(theta * PI);
    vec_x.z = 0.0f;
    normalize_vector(&vec_x);

    vec_y.x = cos(theta*PI + PI/2);
    vec_y.x = sin(theta*PI + PI/2);
    vec_y.x = 0.0f;
    normalize_vector(&vec_y);
    
    vector_cross_ptr(&minivox_intermediate, &vec_x, &vec_y);
    vector_cross_ptr(&vec_z, &vec_x, &minivox_intermediate);
    normalize_vector(&vec_z);   // necessary?

    mult_vec_scalar_ptr(&vec_x, size);
    mult_vec_scalar_ptr(&vec_y, size);
    mult_vec_scalar_ptr(&vec_z, size);
}

void Minivox::set_orientation(float t, float p) {
    theta = t;
    phi = p;
    orient_vectors();
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

    glDisable(GL_DEPTH_TEST);
    glEnd();
#endif
}

#ifdef DC_CLIENT
namespace miniset {
const int v_set[3*8] = {
        0,0,0,
        1,0,0,
        1,1,0,
        0,1,0,
        0,0,1,
        1,0,1,
        1,1,1,
        0,1,1 };

const int q_set[4*6]= {
        4,5,6,7,
        3,2,1,0,
        2,3,7,6,
        0,1,5,4,
        0,4,7,3,
        1,2,6,5 };

float v_buffer[3*8];
float s_buffer[6*(4*3)];
}
#endif

void Minivox::draw() {
#ifdef DC_CLIENT

    // Quit if voxel is completely transparent
    if(vox.a == 0) return;

    // set color mode
    glColor3ub(vox.r, vox.g, vox.b);

    // fill vertex buffer
    int i,j;
    for(i=0; i<8; i++) {
        miniset::v_buffer[3*i+0] = miniset::v_set[3*i+0]*vec_x.x + miniset::v_set[3*i+1]*vec_y.x + miniset::v_set[3*i+2]*vec_z.x;
        miniset::v_buffer[3*i+1] = miniset::v_set[3*i+0]*vec_x.y + miniset::v_set[3*i+1]*vec_y.y + miniset::v_set[3*i+2]*vec_z.y;
        miniset::v_buffer[3*i+2] = miniset::v_set[3*i+0]*vec_x.z + miniset::v_set[3*i+1]*vec_y.z + miniset::v_set[3*i+2]*vec_z.z;
    }
    for(i=0; i<6; i++) {
        for(j=0; j<4; j++) {
            miniset::s_buffer[12*i+3*j+0] = miniset::v_buffer[3*miniset::q_set[4*i+j] + 0];
            miniset::s_buffer[12*i+3*j+1] = miniset::v_buffer[3*miniset::q_set[4*i+j] + 1];
            miniset::s_buffer[12*i+3*j+2] = miniset::v_buffer[3*miniset::q_set[4*i+j] + 2];
        }
    }

    float x0, y0, z0;
    //x0 = vec_x.x + vec_y.x + vec_z.x + particle.state.p.x;
    //y0 = vec_x.y + vec_y.y + vec_z.y + particle.state.p.y;
    //z0 = vec_x.z + vec_y.z + vec_z.z + particle.state.p.z;
    x0 = particle.state.p.x;
    y0 = particle.state.p.y;
    z0 = particle.state.p.z;

    //printf("%0.2f %0.2f %0.2f\n", x0, y0, z0);

    // draw voxel
    for(i=0; i<6; i++) {
        glVertex3f(x0 + s_buffer[12*i+3*0+0], y0+ s_buffer[12*i+3*0+1], z0+ s_buffer[12*i+3*0+2]);
        //printf("draw minivox at %0.2f %0.2f %0.2f\n", x0 + s_buffer[12*i+3*0+0], y0+ s_buffer[12*i+3*0+1], z0+ s_buffer[12*i+3*0+2]);
        glVertex3f(x0 + s_buffer[12*i+3*1+0], y0+ s_buffer[12*i+3*1+1], z0+ s_buffer[12*i+3*1+2]);
        glVertex3f(x0 + s_buffer[12*i+3*2+0], y0+ s_buffer[12*i+3*2+1], z0+ s_buffer[12*i+3*2+2]);
        glVertex3f(x0 + s_buffer[12*i+3*3+0], y0+ s_buffer[12*i+3*3+1], z0+ s_buffer[12*i+3*3+2]);
    }

    //printf("should see minivox\n");

#endif
}


