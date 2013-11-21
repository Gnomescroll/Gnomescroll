/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#include <common/compat_gl.h>
#include <math.h>

const int N_CAMERAS = 2;
float CAMERA_VIEW_DISTANCE = Options::view_distance;    // use default view_distance specified in client_options
float CAMERA_VIEW_DISTANCE_SQUARED = CAMERA_VIEW_DISTANCE*CAMERA_VIEW_DISTANCE;

//For chunks use
////(CAMERA_VIEW_DISTANCE+sqrt(2)*8)*(CAMERA_VIEW_DISTANCE+sqrt(2)*8);
//sqrt(2)*8 = 11.4;

class Camera
{
    private:
        struct Vec3 position;
        float shake_force;
        float shake_cooldown_rate;

    Vec3 apply_shake(const Vec3& f, const Vec3& r, const Vec3& u);

    public:
        float x_size, y_size;
        float ratio;
        float z_near, z_far;
        float theta, phi;
        bool first_person;
        bool zoomed;
        float zoom_factor;
        bool third_person;

    void pan(float dx, float dy);

    float get_fov();
    void set_aspect(float z_near, float z_far);
    void set_projection(float x, float y, float z, float theta, float phi);
    void set_dimensions();  // sets x_size,y_size and ratio from window resolution
    void move(float dx, float dy, float dz);
    void set_angles(float theta, float phi);

    void shake(float force);
    void tick();

    void set_position(struct Vec3 p);
    struct Vec3 get_position() const;

    struct Vec3 forward_vector() const;
    struct Vec3 lateral_vector() const;

    struct Vec3 right_vector() const
    {
        return vec3_euler_rotation(vec3_init(0, 1, 0), this->theta, this->phi, 0.0f);
    }

    struct Vec3 up_vector() const
    {
        return vec3_euler_rotation(vec3_init(0, 0, 1), this->theta, this->phi, 0.0f);
    }

    void hud_projection();
    void world_projection();

    bool is_current();

    void toggle_zoom();
    void zoom();
    void unzoom();

    void copy_state_from(const Camera* c);

    void toggle_third_person()
    {
        this->third_person = (!this->third_person);
        this->first_person = (!this->first_person);
    }

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

void update_camera_settings(float view_distance);

Camera* get_agent_camera();
Camera* get_free_camera();
void use_agent_camera();
void use_free_camera();
void update_agent_camera();
void world_projection();
void hud_projection();

void tick_cameras();
