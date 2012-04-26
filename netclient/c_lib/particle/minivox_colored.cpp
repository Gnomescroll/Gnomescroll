#include "minivox_colored.hpp"

#if DC_CLIENT
#include <compat_gl.h>
#include <c_lib/camera/fulstrum_test.hpp>
#endif

#include <c_lib/voxel/common_data.h>
#include <c_lib/physics/ray_trace/ray_trace.hpp>


namespace Particle
{

ColoredMinivox::ColoredMinivox(int id)
:
ParticleMotion(id, 0,0,0,0,0,0, MINIVOX_MASS*MINIVOX_SIZE),
theta(0.0f), phi(0.0f),
dtheta(0.0f), dphi(0.0f),
r(MINIVOX_R), g(MINIVOX_G), b(MINIVOX_B), a(MINIVOX_A),
size(MINIVOX_SIZE)
{
    this->ttl_max = MINIVOX_TTL;
    this->type = MINIVOX_TYPE;
    orient_vectors();
}

ColoredMinivox::ColoredMinivox(int id, float x, float y, float z, float mx, float my, float mz)
:
ParticleMotion(id, x,y,z, mx,my,mz, MINIVOX_MASS*MINIVOX_SIZE),
theta(0.0f), phi(0.0f),
dtheta(0.0f), dphi(0.0f),
r(MINIVOX_R), g(MINIVOX_G), b(MINIVOX_B), a(MINIVOX_A),
size(MINIVOX_SIZE)
{
    this->ttl_max = MINIVOX_TTL;
    this->type = MINIVOX_TYPE;
    orient_vectors();
}

// recalculates orientation vectors from angular parameter
void ColoredMinivox::orient_vectors()
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
    this->verlet_bounce(MINIVOX_DAMP);
    this->spin();
    this->ttl++;
}

/* ColoredMinivox list */

void ColoredMinivox_list::tick()
{
    for (int i=0; i<n_max; i++)
    {
        if (a[i] == NULL) continue;
        a[i]->tick();
        if (a[i]->ttl >= a[i]->ttl_max)
            destroy(a[i]->id);
    }
}

void ColoredMinivox_list::draw()
{
    #if DC_CLIENT
    if(num == 0) return;
    for (int i=0; i<n_max; i++)
        if (a[i] != NULL)
            a[i]->draw();
    #endif
}

void ColoredMinivox::draw()
{
    #if DC_CLIENT
    // Quit if voxel is completely transparent
    if (this->a == 0) return;

    Vec3 position = this->get_position();
    const float
        x0 = position.x,
        y0 = position.y,
        z0 = position.z;

    if (point_fulstrum_test(x0, y0, z0) == false) return;

    glColor3ub(this->r, this->g, this->b);

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
        glVertex3f(x0 + s_buffer[12*i+3*0+0], y0+ s_buffer[12*i+3*0+1], z0+ s_buffer[12*i+3*0+2]);
        glVertex3f(x0 + s_buffer[12*i+3*1+0], y0+ s_buffer[12*i+3*1+1], z0+ s_buffer[12*i+3*1+2]);
        glVertex3f(x0 + s_buffer[12*i+3*2+0], y0+ s_buffer[12*i+3*2+1], z0+ s_buffer[12*i+3*2+2]);
        glVertex3f(x0 + s_buffer[12*i+3*3+0], y0+ s_buffer[12*i+3*3+1], z0+ s_buffer[12*i+3*3+2]);
    }
    #endif
}


}
