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
        float theta, phi;

        int type;

        bool first_person;
        bool zoomed;
        float zoom_factor;
        
        void pan(float dx, float dy);

        void set_aspect(float fov, float z_near, float z_far);
        void set_projection(float x, float y, float z, float theta, float phi);
        void set_dimensions();  // sets x_size,y_size and ratio from window resolution
        void set_fov(float fov);
        void move(float dx, float dy, float dz);
        void set_angles(float theta, float phi);

        void forward_vector(float f[3]);
        
        void hud_projection();
        void world_projection();

        int is_current();

        void toggle_zoom();

        CCamera();
};

void set_camera(CCamera* cam);
void init_cameras();
void update_camera_matrices();

//extern int first_person;
extern int current_camera_id;
extern CCamera* current_camera;
extern CCamera* agent_camera;
extern CCamera* free_camera;

extern float model_view_matrix[16];
extern double model_view_matrix_dbl[16];
extern double projection_matrix[16];
extern GLint viewport[4];

CCamera* get_agent_camera();
CCamera* get_free_camera();
void use_agent_camera();
void use_free_camera();
void update_agent_camera();
void camera_input_update(int delta_tick, bool invert, float sensitivity);
void world_projection();
void hud_projection();
