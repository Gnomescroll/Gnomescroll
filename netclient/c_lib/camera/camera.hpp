#pragma once

#include <compat_gl.h>
#include <math.h>

#include <c_lib/physics/vector.h>

#define N_CAMERAS 2

class CCamera {
    public:
        float fov;
        float x_size,y_size;
        float ratio;
        float z_near, z_far;

        float x,y,z;
        float xl, yl, zl;
        float xu, yu, zu;
        float x_angle, y_angle;

        CCamera();

        void pan(float dx, float dy);

        void set_aspect(float fov, float x_size, float y_size, float z_near, float z_far);
        void set_projection(float x, float y, float z, float x_angle, float y_angle);

        void hud_projection();
        void world_projection();

        int current();
};

CCamera* get_available_camera();
void set_camera(CCamera* cam);
//void set_model_view_matrix(float *a);
void init_cameras();

extern int current_camera_id;
extern CCamera* current_camera;
extern float model_view_matrix[16];
