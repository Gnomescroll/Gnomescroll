#include "minivox.hpp"

#ifdef DC_CLIENT
    #include <compat_gl.h>
#endif

#include <voxel/common_data.h>

#include <ray_trace/ray_trace.h>

#include <t_map/t_properties.h>
#include <t_map/t_extern.h>

#include <c_lib/camera/fulstrum_test.hpp>

//#include <t_map/t_map.hpp>
//#include <t_map/t_vbo.h>

Minivox::Minivox(int id)
:
theta(0.0f), phi(0.0f),
dtheta(0.0f), dphi(0.0f),
size(minivox_size), draw_mode(0), texture_pixel_width(2)
{
    create_particle2(&particle, id, MINIVOX_TYPE, 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f, 0, MINIVOX_TTL);

    vox.r = MINIVOX_R;
    vox.g = MINIVOX_G;
    vox.b = MINIVOX_B;
    vox.a = MINIVOX_A;

    orient_vectors();
}

Minivox::Minivox(int id, float x, float y, float z, float vx, float vy, float vz)
:
theta(0.0f), phi(0.0f),
dtheta(0.0f), dphi(0.0f),
size(minivox_size), draw_mode(0), texture_pixel_width(2)
{
    create_particle2(&particle, id, MINIVOX_TYPE, x,y,z,vx,vy,vz, 0, MINIVOX_TTL);

    vox.r = MINIVOX_R;
    vox.g = MINIVOX_G;
    vox.b = MINIVOX_B;
    vox.a = MINIVOX_A;

    orient_vectors();
}

// recalculates orientation vectors from angular parameter
void Minivox::orient_vectors() {

    vec_x.x = cos(theta * PI) * cos(phi * PI);
    vec_x.y = sin(theta * PI) * cos(phi * PI);
    vec_x.z = sin(phi);
    normalize_vector(&vec_x);

    vec_y.x = cos(theta*PI + PI/2.0f);
    vec_y.y = sin(theta*PI + PI/2.0f);
    vec_y.z = 0.0f;
    normalize_vector(&vec_y);

    vec_z = vector_cross(vec_x, vec_y);
    vec_y = vector_cross(vec_x, vec_z);

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

void Minivox::set_ttl(int ttl) {
    particle.ttl = ttl;
}
void Minivox::set_size(float size) {
    this->size = size;
}

void Minivox::set_texture(int tex_id) {
    const int pix = this->texture_pixel_width;  // NxN random texture sample within cube identified by tex_id
    const int cube_w = 32;
    const int cube_surface_w = 512;
    const int cubes_per_surface_axis = cube_surface_w / cube_w;
    const float scale = 2.0f/((float)cube_w);

    const float pixel_w = 1.0f/((float)cube_surface_w);

    this->pix_margin = pixel_w*pix;

    int itx,ity;
    float ftx,fty;

    // integer index of cube in spritesheet
    itx = tex_id % cubes_per_surface_axis;
    ity = tex_id / cubes_per_surface_axis;

    // ...scaled to texture
    ftx = itx * scale;
    fty = ity * scale;

    int ix,iy;
    float fx,fy;
    // random point within cube, with margins wide enough to fit pix
    ix = (rand() % (cube_w -(pix*2))) +pix;
    iy = (rand() % (cube_w -(pix*2))) +pix;

    // ...scaled to texture
    fx = ((float)ix) * pixel_w;
    fy = ((float)iy) * pixel_w;

    ftx += fx;
    fty += fy;

    this->tx = ftx;
    this->ty = fty;

    draw_mode = MINIVOX_DRAW_MODE_TEXTURED;
}

void Minivox::set_spin(float dtheta, float dphi) {
    this->dtheta = dtheta;
    this->dphi = dphi;
}

void Minivox::set_angles(float theta, float phi)
{
    this->theta = theta;
    this->phi = phi;
    this->orient_vectors();
}

void Minivox::spin() {
    this->set_angles(this->theta + this->dtheta, this->phi + this->dphi);
}

void Minivox::set_texture(int tex_id, int pixels_wide) {
    int prev = this->texture_pixel_width;
    this->texture_pixel_width = pixels_wide;
    set_texture(tex_id);
    this->texture_pixel_width = prev;
}

void Minivox::tick() {
    bounce_simple_rk4(&particle, MINIVOX_DAMP);
    this->spin();
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

    //glColor3ub(255,255,255);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glBindTexture(GL_TEXTURE_2D, block_texture_no_gamma_correction);
    glBegin(GL_QUADS);

    int i;

    for(i=0; i<n_max; i++) {
        if (a[i] == NULL) continue;
        a[i]->draw();
    }        

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glEnd();

#endif
}

void Minivox::draw() {
#ifdef DC_CLIENT

    // Quit if voxel is completely transparent
    //if(vox.a == 0) return;

    // copy the particle position for cleaner code
    const float
        x0 = particle.state.p.x,
        y0 = particle.state.p.y,
        z0 = particle.state.p.z;

        
    if( point_fulstrum_test(x0,y0,z0) == false ) return; //check to see if they are in viewing fulstrum

    if (draw_mode == MINIVOX_DRAW_MODE_TEXTURED) 
    {
        draw_textured();
        return;
    }

    // set color mode
    glColor3ub(vox.r, vox.g, vox.b);

    // fill vertex buffer
    int i,j;
    for(i=0; i<8; i++) {
        v_buffer[3*i+0] = v_set[3*i+0]*vec_x.x + v_set[3*i+1]*vec_y.x + v_set[3*i+2]*vec_z.x;
        v_buffer[3*i+1] = v_set[3*i+0]*vec_x.y + v_set[3*i+1]*vec_y.y + v_set[3*i+2]*vec_z.y;
        v_buffer[3*i+2] = v_set[3*i+0]*vec_x.z + v_set[3*i+1]*vec_y.z + v_set[3*i+2]*vec_z.z;
    }
    for(i=0; i<6; i++) {
        for(j=0; j<4; j++) {
            s_buffer[12*i+3*j+0] = v_buffer[3*q_set[4*i+j] + 0];
            s_buffer[12*i+3*j+1] = v_buffer[3*q_set[4*i+j] + 1];
            s_buffer[12*i+3*j+2] = v_buffer[3*q_set[4*i+j] + 2];
        }
    }


    // draw voxel
    for(i=0; i<6; i++) {
        glVertex3f(x0 + s_buffer[12*i+3*0+0], y0+ s_buffer[12*i+3*0+1], z0+ s_buffer[12*i+3*0+2]);
        glVertex3f(x0 + s_buffer[12*i+3*1+0], y0+ s_buffer[12*i+3*1+1], z0+ s_buffer[12*i+3*1+2]);
        glVertex3f(x0 + s_buffer[12*i+3*2+0], y0+ s_buffer[12*i+3*2+1], z0+ s_buffer[12*i+3*2+2]);
        glVertex3f(x0 + s_buffer[12*i+3*3+0], y0+ s_buffer[12*i+3*3+1], z0+ s_buffer[12*i+3*3+2]);
    }

#endif
}

void Minivox::draw_textured() {
#ifdef DC_CLIENT
    if(vox.a == 0) return;

    glColor3ub(255,255,255);

    float tx_min = tx;
    float tx_max = tx + pix_margin;
    float ty_min = ty;
    float ty_max = ty + pix_margin;

    // fill vertex buffer
    int i,j;
    for(i=0; i<8; i++) {
        v_buffer[3*i+0] = v_set[3*i+0]*vec_x.x + v_set[3*i+1]*vec_y.x + v_set[3*i+2]*vec_z.x;
        v_buffer[3*i+1] = v_set[3*i+0]*vec_x.y + v_set[3*i+1]*vec_y.y + v_set[3*i+2]*vec_z.y;
        v_buffer[3*i+2] = v_set[3*i+0]*vec_x.z + v_set[3*i+1]*vec_y.z + v_set[3*i+2]*vec_z.z;
    }
    for(i=0; i<6; i++) {
        for(j=0; j<4; j++) {
            s_buffer[12*i+3*j+0] = v_buffer[3*q_set[4*i+j] + 0];
            s_buffer[12*i+3*j+1] = v_buffer[3*q_set[4*i+j] + 1];
            s_buffer[12*i+3*j+2] = v_buffer[3*q_set[4*i+j] + 2];
        }
    }

    // copy the particle position for cleaner code
    const float
        x0 = particle.state.p.x,
        y0 = particle.state.p.y,
        z0 = particle.state.p.z;

    // draw voxel
    for(i=0; i<6; i++) {
        glTexCoord2f(tx_min, ty_min);
        glVertex3f(x0 + s_buffer[12*i+3*0+0], y0+ s_buffer[12*i+3*0+1], z0+ s_buffer[12*i+3*0+2]);
        glTexCoord2f(tx_min, ty_max);
        glVertex3f(x0 + s_buffer[12*i+3*1+0], y0+ s_buffer[12*i+3*1+1], z0+ s_buffer[12*i+3*1+2]);
        glTexCoord2f(tx_max, ty_max);
        glVertex3f(x0 + s_buffer[12*i+3*2+0], y0+ s_buffer[12*i+3*2+1], z0+ s_buffer[12*i+3*2+2]);
        glTexCoord2f(tx_max, ty_min);
        glVertex3f(x0 + s_buffer[12*i+3*3+0], y0+ s_buffer[12*i+3*3+1], z0+ s_buffer[12*i+3*3+2]);
    }

    #endif
}

void Minivox_list::set_size(float s)
{
    _s = minivox_size;
    minivox_size = s;
}
void Minivox_list::unset_size()
{
    minivox_size = _s;
}


Minivox_list::Minivox_list()
:
_s(minivox_size)
{}
