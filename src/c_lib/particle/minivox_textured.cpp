#include "minivox_textured.hpp"

#if DC_CLIENT
#include <common/compat_gl.h>
#include <t_map/glsl/texture.hpp>
#include <camera/fulstrum_test.hpp>
#endif

#include <voxel/common_data.h>
#include <physics/ray_trace/ray_trace.hpp>

namespace Particle
{

inline void TexturedMinivox::init()
{
    this->theta = 0.0f;
    this->phi = 0.0f;
    this->dtheta = 0.0f;
    this->dphi = 0.0f;
    this->ttl = MINIVOX_TTL;
    this->type = MINIVOX_TYPE;
    this->verlet.dampening = MINIVOX_DAMP;
    
    orient_vectors();
}

TexturedMinivox::TexturedMinivox()
:
ParticleMotion(-1, 0,0,0,0,0,0, MINIVOX_MASS*MINIVOX_SIZE),
size(MINIVOX_SIZE), texture_pixel_width(2)
{
    this->init();
}

// recalculates orientation vectors from angular parameter
void TexturedMinivox::orient_vectors()
{
    vec_x.x = cosf(theta * PI) * cosf(phi * PI);
    vec_x.y = sinf(theta * PI) * cosf(phi * PI);
    vec_x.z = sinf(phi);
    normalize_vector(&vec_x);

    vec_y.x = cosf(theta*PI + PI/2.0f);
    vec_y.y = sinf(theta*PI + PI/2.0f);
    vec_y.z = 0.0f;
    normalize_vector(&vec_y);

    vec_z = vec3_cross(vec_x, vec_y);
    vec_y = vec3_cross(vec_x, vec_z);

    vec_x = vec3_scalar_mult(vec_x, size);
    vec_y = vec3_scalar_mult(vec_y, size);
    vec_z = vec3_scalar_mult(vec_z, size);
}

void TexturedMinivox::set_orientation(float t, float p)
{
    theta = t;
    phi = p;
    orient_vectors();
}

void TexturedMinivox::set_size(float size)
{
    this->size = size;
    this->set_mass(size * MINIVOX_MASS);
}

void TexturedMinivox::set_texture(int tex_id)
{
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
}

void TexturedMinivox::set_spin(float dtheta, float dphi)
{
    this->dtheta = dtheta;
    this->dphi = dphi;
}

void TexturedMinivox::set_angles(float theta, float phi)
{
    this->theta = theta;
    this->phi = phi;
    this->orient_vectors();
}

void TexturedMinivox::spin()
{
    this->set_angles(this->theta + this->dtheta, this->phi + this->dphi);
}

void TexturedMinivox::set_texture(int tex_id, int pixels_wide)
{
    this->texture_pixel_width = pixels_wide;
    set_texture(tex_id);
}

void TexturedMinivox::tick()
{
    this->verlet.bounce();
    this->spin();
    this->ttl--;
}

/* TexturedMinivox list */

void TexturedMinivox_list::tick()
{
    for (unsigned int i=0; i<num; i++)
    {
        a[i].tick();
        if (a[i].ttl <= 0) this->destroy(i);
    }
}

void TexturedMinivox_list::draw()
{
    #if DC_CLIENT
    for (unsigned int i=0; i<num; a[i++].draw());
    #endif
}

void TexturedMinivox::draw()
{
    #if DC_CLIENT
    Vec3 position = this->get_position();
    position = quadrant_translate_position(current_camera_position, position);
    const float
        x0 = position.x,
        y0 = position.y,
        z0 = position.z;

    if (point_fulstrum_test(x0,y0,z0) == false) return; //check to see if they are in viewing fulstrum

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

}
