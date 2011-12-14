#pragma once

#include <compat_gl.h>
#include <math.h>

#include <c_lib/physics/vector.hpp>

#define N_CAMERAS 2

enum CAMERA_TYPES {
    UNKNOWN_CAM,
    AGENT_CAM,
    CAMERA_CAM
};

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

        int type;

        CCamera();

        void pan(float dx, float dy);

        void set_aspect(float fov, float z_near, float z_far);
        void set_projection(float x, float y, float z, float x_angle, float y_angle);
        void set_dimensions();  // sets x_size,y_size and ratio from window resolution
        void set_fov(float fov);
        void move(float dx, float dy, float dz);
        void set_angles(float theta, float phi);
        
        void hud_projection();
        void world_projection();

        int is_current();
};

CCamera* get_available_camera();
void set_camera(CCamera* cam);
void init_cameras();
void set_camera_first_person(int fp);

extern int first_person;
extern int current_camera_id;
extern CCamera* current_camera;
extern float model_view_matrix[16];
