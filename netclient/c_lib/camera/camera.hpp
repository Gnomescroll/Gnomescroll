#pragma once

#include <compat_gl.h>
#include <math.h>

#define N_CAMERAS 2

enum CAMERA_TYPES {
    UNKNOWN_CAM,
    AGENT_CAM,
    CAMERA_CAM
};

class Camera {
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
        void set_state(float x, float y, float z);
        
        void forward_vector(float f[3]);
        
        void hud_projection();
        void world_projection();

        int is_current();

        void toggle_zoom();
        void zoom();
        void unzoom();

        Camera();
};

void set_camera(Camera* cam);
void init_cameras();
void update_camera_matrices();

extern int current_camera_id;
extern Camera* current_camera;
extern Camera* agent_camera;
extern Camera* free_camera;

extern float model_view_matrix[16];
extern double model_view_matrix_dbl[16];
extern double projection_matrix[16];
extern GLint viewport[4];

Camera* get_agent_camera();
Camera* get_free_camera();
void use_agent_camera();
void use_free_camera();
void update_agent_camera();
void world_projection();
void hud_projection();
