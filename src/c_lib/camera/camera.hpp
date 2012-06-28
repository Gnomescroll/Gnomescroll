#pragma once

#include <common/compat_gl.h>
#include <math.h>

const int N_CAMERAS = 2;
const float CAMERA_VIEW_DISTANCE = 128.0f; // 16.0 for fog
const float CAMERA_VIEW_DISTANCE_SQUARED = CAMERA_VIEW_DISTANCE*CAMERA_VIEW_DISTANCE;


//For chunks use
////(CAMERA_VIEW_DISTANCE+11.4)*(CAMERA_VIEW_DISTANCE+11.4);
enum CAMERA_TYPES
{
    UNKNOWN_CAM,
    AGENT_CAM,
    CAMERA_CAM
};

class Camera
{
    private:
        struct Vec3 position;
    public:
        float fov;
        float x_size,y_size;
        float ratio;
        float z_near, z_far;

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
        
        void set_position(struct Vec3 p);
        struct Vec3 get_position() { return this->position; }
        
        struct Vec3 forward_vector();
        
        struct Vec3 right_vector()
        {
			Vec3 right = vec3_init(0.0f, 1.0f, 0.0f);
			right = vec3_euler_rotation(right, this->theta, this->phi, 0.0f);
			normalize_vector(&right);
			return right;
		}
        
        struct Vec3 up_vector()
        {
			Vec3 up = vec3_init(0.0f, 0.0f, 1.0f);
			up = vec3_euler_rotation(up, this->theta, this->phi, 0.0f);
			normalize_vector(&up);
			return up;
		}
        
        void hud_projection();
        void world_projection();

        bool is_current();

        void toggle_zoom();
        void zoom();
        void unzoom();

        void copy_state_from(Camera* c);

        Camera();
};

void set_camera(Camera* cam);
void init_cameras();
void update_camera_matrices();

extern struct Vec3 current_camera_position;
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
