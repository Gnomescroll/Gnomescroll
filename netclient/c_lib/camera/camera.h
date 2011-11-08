#pragma once

#include <compat_gl.h>
#include <math.h>

struct Camera2 {
float fov;
float x_size,y_size;
float ratio;
float z_near, z_far;

float x,y,z;
float xl, yl, zl;
float xu, yu, zu;
float x_angle, y_angle;
};

int _world_projection(struct Camera2* camera2);
int _hud_projection(struct Camera2* camera2);

int _set_camera(struct Camera2* c);
struct Camera2* _get_camera();
void _set_camera_state(float x, float y, float z, float theta, float phi);
void _pan_camera(float dtheta, float dphi);

void set_model_view_matrix(float *a);

// redundant hack; remove this once SDL_functions are migrated to c_lib
int _set_resolution_camera(int xres, int yres, int fullscreen);

void init_camera_c();
