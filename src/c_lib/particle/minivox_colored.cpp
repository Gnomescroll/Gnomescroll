#include "minivox_colored.hpp"

#if DC_CLIENT
#include <common/compat_gl.h>
#include <camera/fulstrum_test.hpp>
#endif

#include <voxel/common_data.h>
#include <physics/ray_trace/ray_trace.hpp>


namespace Particle
{

inline void ColoredMinivox::init()
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


ColoredMinivox::ColoredMinivox()
:
ParticleMotion(-1, 0,0,0,0,0,0, MINIVOX_MASS*MINIVOX_SIZE),
r(MINIVOX_R), g(MINIVOX_G), b(MINIVOX_B), a(MINIVOX_A),
size(MINIVOX_SIZE)
{
    this->init();
}

// recalculates orientation vectors from angular parameter
void ColoredMinivox::orient_vectors()
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

void ColoredMinivox::set_orientation(float t, float p)
{
    theta = t;
    phi = p;
    orient_vectors();
}

void ColoredMinivox::set_color(unsigned char r, unsigned char g, unsigned char b)
{
    this->r = r;
    this->g = g;
    this->b = b;
}

void ColoredMinivox::set_color(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = a;
}

void ColoredMinivox::set_size(float size)
{
    this->size = size;
    this->set_mass(size * MINIVOX_MASS);
}

void ColoredMinivox::set_spin(float dtheta, float dphi)
{
    this->dtheta = dtheta;
    this->dphi = dphi;
}

void ColoredMinivox::set_angles(float theta, float phi)
{
    this->theta = theta;
    this->phi = phi;
    this->orient_vectors();
}

void ColoredMinivox::spin()
{
    this->set_angles(this->theta + this->dtheta, this->phi + this->dphi);
}

void ColoredMinivox::tick()
{
    this->verlet.bounce();
    this->spin();
    this->ttl--;
}

/* ColoredMinivox list */

void ColoredMinivox_list::tick()
{
    for (unsigned int i=0; i<num; i++)
    {
        a[i].tick();
        if (a[i].ttl <= 0) this->destroy(i);
    }
}

void ColoredMinivox_list::draw()
{
    #if DC_CLIENT
    for (unsigned int i=0; i<num; a[i++].draw());
    #endif
}

void ColoredMinivox::draw()
{
    #if DC_CLIENT
    // Quit if voxel is completely transparent
    if (this->a == 0) return;

    Vec3 p = this->get_position();
    p = quadrant_translate_position(current_camera_position, p);
    if (point_fulstrum_test(p.x,p.y,p.z) == false) return;

    glColor4ub(this->r, this->g, this->b, 255);

    // fill vertex buffer
    int i,j;
    for (i=0; i<8; i++) {
        v_buffer[3*i+0] = v_set[3*i+0]*vec_x.x + v_set[3*i+1]*vec_y.x + v_set[3*i+2]*vec_z.x;
        v_buffer[3*i+1] = v_set[3*i+0]*vec_x.y + v_set[3*i+1]*vec_y.y + v_set[3*i+2]*vec_z.y;
        v_buffer[3*i+2] = v_set[3*i+0]*vec_x.z + v_set[3*i+1]*vec_y.z + v_set[3*i+2]*vec_z.z;
    }
    for (i=0; i<6; i++) {
        for(j=0; j<4; j++) {
            s_buffer[12*i+3*j+0] = v_buffer[3*q_set[4*i+j] + 0];
            s_buffer[12*i+3*j+1] = v_buffer[3*q_set[4*i+j] + 1];
            s_buffer[12*i+3*j+2] = v_buffer[3*q_set[4*i+j] + 2];
        }
    }

    // draw voxel
    for (i=0; i<6; i++) {
        glVertex3f(p.x + s_buffer[12*i+3*0+0], p.y + s_buffer[12*i+3*0+1], p.z + s_buffer[12*i+3*0+2]);
        glVertex3f(p.x + s_buffer[12*i+3*1+0], p.y + s_buffer[12*i+3*1+1], p.z + s_buffer[12*i+3*1+2]);
        glVertex3f(p.x + s_buffer[12*i+3*2+0], p.y + s_buffer[12*i+3*2+1], p.z + s_buffer[12*i+3*2+2]);
        glVertex3f(p.x + s_buffer[12*i+3*3+0], p.y + s_buffer[12*i+3*3+1], p.z + s_buffer[12*i+3*3+2]);
    }
    #endif
}


}
