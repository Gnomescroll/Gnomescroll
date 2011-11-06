#pragma once

#include <compat_gl.h>
#include <math.h>

#include <c_lib/physics/vector.h>

struct Camera {
    float fov;
    float x_size,y_size;
    float ratio;
    float z_near, z_far;

    float x,y,z;
    float xl, yl, zl;
    float xu, yu, zu;
    float x_angle, y_angle;
};

int _world_projection(struct Camera* camera);
int _hud_projection(struct Camera* camera);

int _set_camera(struct Camera* c);
struct Camera* _get_camera();
void _set_camera_state(float x, float y, float z, float theta, float phi);
void _pan_camera(float dtheta, float dphi);

void set_model_view_matrix(float *a);

void init_camera_c();
