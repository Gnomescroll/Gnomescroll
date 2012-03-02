#include "minivox.hpp"

#ifdef DC_CLIENT
    #include <compat_gl.h>
#endif

#include <voxel/common_data.h>

#include <ray_trace/ray_trace.hpp>

#include <t_map/glsl/texture.hpp>

#include <c_lib/camera/fulstrum_test.hpp>

//#include <t_map/t_map.hpp>
//#include <t_map/t_vbo.h>

const float MINIVOX_MASS = 2.0f;

Minivox::Minivox(int id)
:
Particle(id, 0,0,0,0,0,0, MINIVOX_MASS*minivox_size),
theta(0.0f), phi(0.0f),
dtheta(0.0f), dphi(0.0f),
r(MINIVOX_R), g(MINIVOX_G), b(MINIVOX_B), a(MINIVOX_A),
size(minivox_size), draw_mode(0), texture_pixel_width(2)
{
    this->ttl_max = MINIVOX_TTL;
    this->type = MINIVOX_TYPE;
    orient_vectors();
}

Minivox::Minivox(int id, float x, float y, float z, float mx, float my, float mz)
:
Particle(id, x,y,z, mx,my,mz, MINIVOX_MASS*minivox_size),
theta(0.0f), phi(0.0f),
dtheta(0.0f), dphi(0.0f),
r(MINIVOX_R), g(MINIVOX_G), b(MINIVOX_B), a(MINIVOX_A),
size(minivox_size), draw_mode(0), texture_pixel_width(2)
{
    this->ttl_max = MINIVOX_TTL;
    this->type = MINIVOX_TYPE;
    orient_vectors();
}

// recalculates orientation vectors from angular parameter
void Minivox::orient_vectors()
{
    vec_x.x = cos(theta * PI) * cos(phi * PI);
    vec_x.y = sin(theta * PI) * cos(phi * PI);
    vec_x.z = sin(phi);
    normalize_vector(&vec_x);

    vec_y.x = cos(theta*PI + PI/2.0f);
    vec_y.y = sin(theta*PI + PI/2.0f);
    vec_y.z = 0.0f;
    normalize_vector(&vec_y);

    vec_z = vec3_cross(vec_x, vec_y);
    vec_y = vec3_cross(vec_x, vec_z);

    vec_x = vec3_scalar_mult(vec_x, size);
    vec_y = vec3_scalar_mult(vec_y, size);
    vec_z = vec3_scalar_mult(vec_z, size);
}

void Minivox::set_orientation(float t, float p) {
    theta = t;
    phi = p;
    orient_vectors();
}

void Minivox::set_color(unsigned char r, unsigned char g, unsigned char b) {
    this->r = r;
    this->g = g;
    this->b = b;
}

void Minivox::set_color(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = a;
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

    int mx,my;
    float fx,fy;
    // random point within cube, with margins wide enough to fit pix
    mx = (rand() % (cube_w -(pix*2))) +pix;
    my = (rand() % (cube_w -(pix*2))) +pix;

    // ...scaled to texture
    fx = ((float)mx) * pixel_w;
    fy = ((float)my) * pixel_w;

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
    //bounce_simple_rk4(&particle, MINIVOX_DAMP);
    Verlet::bounce(vp, MINIVOX_DAMP);
    this->spin();
    this->ttl++;
}

/* Minivox list */

void Minivox_list::tick() {
    int i;
    for (i=0; i<n_max; i++) {
        if (a[i] == NULL) continue;
        a[i]->tick();
        if (a[i]->ttl >= a[i]->ttl_max) {
            destroy(a[i]->id);
        }
    }
}

void Minivox_list::draw() {
#ifdef DC_CLIENT

    if(num == 0) return;

    glEnable(GL_DEPTH_TEST);
    glBegin(GL_QUADS);

    for (int i=0; i<n_max; i++) {
        if (a[i] == NULL) continue;
        if (a[i]->draw_mode != MINIVOX_DRAW_MODE_TEXTURED)
            a[i]->draw_colored();
    }

    glEnd();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, block_texture_no_gamma_correction);
    glBegin(GL_QUADS);

    for (int i=0; i<n_max; i++) {
        if (a[i] == NULL) continue;
        if (a[i]->draw_mode == MINIVOX_DRAW_MODE_TEXTURED)
            a[i]->draw_textured();
    }        

    glEnd();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    
#endif
}

void Minivox::draw_colored() {
#ifdef DC_CLIENT

    // Quit if voxel is completely transparent
    if(this->a == 0) return;
    if (current_camera == NULL || !current_camera->in_view(this->vp->p.x, this->vp->p.y, this->vp->p.z)) return;


    const float
        x0 = this->vp->p.x,
        y0 = this->vp->p.y,
        z0 = this->vp->p.z;

    if( point_fulstrum_test(x0,y0,z0) == false ) return; //check to see if they are in viewing fulstrum

    glColor3ub(this->r, this->g, this->b);

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
    if(this->a == 0) return;
    if (current_camera == NULL || !current_camera->in_view(this->vp->p.x, this->vp->p.y, this->vp->p.z)) return;


    const float
        x0 = this->vp->p.x,
        y0 = this->vp->p.y,
        z0 = this->vp->p.z;

    if( point_fulstrum_test(x0,y0,z0) == false ) return; //check to see if they are in viewing fulstrum

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
