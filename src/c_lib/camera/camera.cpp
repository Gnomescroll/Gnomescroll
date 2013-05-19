#include "camera.hpp"

#include <camera/fulstrum_test.hpp>

#include <physics/vec3.hpp>
#include <physics/mat3.hpp>

#include <input/input.hpp>
#include <voxel/voxel_loader.hpp>

extern float _xresf;
extern float _yresf;

struct Vec3 current_camera_position;

Camera* agent_camera = NULL;
Camera* free_camera = NULL;
Camera* current_camera = NULL;

float model_view_matrix[16];
double model_view_matrix_dbl[16];
double projection_matrix[16];
GLint viewport[4];

void set_camera(Camera* cam)
{
    current_camera = cam;
}

void init_cameras()
{
    agent_camera = new Camera();
    agent_camera->first_person = true;
    agent_camera->set_position(vec3_init(0, 0, 50.0f));

    free_camera = new Camera();
    free_camera->first_person = false;
    free_camera->set_position(vec3_init(64, 64, 128));

    current_camera = free_camera;

    const int n = 2;
    class Camera* cameras[n] = { agent_camera, free_camera };
    for (int i=0; i<n; i++)
    {
        class Camera* camera = cameras[i];

        camera->theta = 0;
        camera->phi = 0;

        Vec3 f = vec3_init(1, 0, 0);
        Vec3 r = vec3_init(0, 1, 0);
        Vec3 u = vec3_init(0, 0, 1);

        f = vec3_euler_rotation(f, camera->theta, camera->phi, 0.0f);
        r = vec3_euler_rotation(r, camera->theta, camera->phi, 0.0f);
        u = vec3_euler_rotation(u, camera->theta, camera->phi, 0.0f);

        Vec3 p = camera->get_position();
        float fov = camera->get_fov();
        setup_fulstrum(fov, camera->ratio, camera->z_far, p,f,r,u);
        setup_fulstrum2(fov, camera->ratio, camera->z_near,camera->z_far, p,f,r,u);
    }

    // custom fog shaders use EXP4
    glFogf(GL_FOG_DENSITY, 0.0625f);
    unsigned char fog_color[4] = {0,0,0,0};
    float fc[4];
    for (int i=0; i<4; i++) fc[i] = ((float)fog_color[i]/256.0f);
    glFogfv(GL_FOG_COLOR, fc);
    glFogf(GL_FOG_START, Options::view_distance - 24);  // 24 is magic number calibrated for exp4 inflection with 0.0625 density
}

void teardown_cameras()
{
    delete agent_camera;
    delete free_camera;
}

#define CAMERA_ZOOM_FACTOR 2.0f
Camera::Camera() :
    zoomed(false), zoom_factor(CAMERA_ZOOM_FACTOR)
{
    const float Z_NEAR = 0.1f;
    const float Z_FAR = 320.0f;
    set_aspect(Z_NEAR, Z_FAR);
    set_dimensions();
    set_projection(256.0f, 256.0f, 256.0f, 0.0f, 0.0f);
}
#undef CAMERA_ZOOM_FACTOR

void Camera::toggle_zoom()
{
    if (zoomed)
        this->unzoom();
    else
        this->zoom();
}

float Camera::get_fov()
{
    if (this->zoomed)
        return Options::fov / this->zoom_factor;
    return Options::fov;
}

void Camera::zoom()
{
    if (this->zoomed) return;
    this->zoomed = true;
}

void Camera::unzoom()
{
    if (!this->zoomed) return;
    this->zoomed = false;
}

bool Camera::is_current()
{
    return (this == current_camera);
}

void Camera::set_aspect(float z_near, float z_far)
{
    this->z_near = z_near;
    this->z_far = z_far;
}

void Camera::set_projection(float x, float y, float z, float theta, float phi)
{
    this->set_position(vec3_init(x,y,z));
    this->theta = theta;
    this->phi = phi;
}

void Camera::set_dimensions()
{
    x_size = _xresf;
    y_size = _yresf;
    ratio = x_size / y_size;
}

void Camera::set_position(struct Vec3 p)
{
    this->position = translate_position(p);
    GS_ASSERT(is_boxed_position(this->position));
    if (this->is_current()) current_camera_position = this->position;
    if (this->is_current())
    {
        GS_ASSERT(vec3_equal(this->position, current_camera_position));
    }
}

void Camera::pan(float dx, float dy)
{   // args are deltas
    theta += dx;
    phi += dy;

    if (theta > 1.0f) theta -= 2.0f;
    else if (theta < -1.0f) theta += 2.0f;

    // DO NOT ADD ANY MORE SIGNIFICANT DIGITS TO 0.4999f
    // Camera behavior when looking straight up or down is fucked up otherwise
    if (phi > 0.49f) phi = 0.49f;
    else if (phi < -0.49f) phi = -0.49f;
}

void Camera::move(float dx, float dy, float dz)
{
    float x = this->position.x;
    float y = this->position.y;
    float z = this->position.z;
    x += dx*cosf(theta * PI);
    x += dy*cosf(theta * PI + PI/2.0f);
    y += dx*sinf(theta * PI);
    y += dy*sinf(theta * PI + PI/2.0f);
    z += dz;
    this->set_position(vec3_init(x,y,z));
}

void Camera::world_projection()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    float fov = this->get_fov();
    gluPerspective(fov, ratio, z_near, z_far);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    Vec3 look = vec3_init(1.0f, 0.0f, 0.0f);
    Vec3 right = vec3_init(0.0f, 1.0f, 0.0f);
    Vec3 up = vec3_init(0.0f, 0.0f, 1.0f);

    //look = vec3_euler_rotation(look, theta + 1.00, phi - 1.00, 0.0);
    look = vec3_euler_rotation(look, theta, phi, 0.0f);

    float x = this->position.x;
    float y = this->position.y;
    float z = this->position.z;

    // DEPRECATE GLU
    gluLookAt(
        x,y,z,
        x + look.x, y + look.y, z + look.z,
        up.x, up.y, up.z
    );
    // DEPRECATE GLU

    //glGetDoublev(GL_MODELVIEW_MATRIX, model_view_matrix_dbl);

    glGetDoublev(GL_MODELVIEW_MATRIX, model_view_matrix_dbl);

    double uxd, uyd, uzd;

    double offset = 0.1*randf();

    uxd = offset*model_view_matrix_dbl[4*2+0];
    uyd = offset*model_view_matrix_dbl[4*2+1];
    uzd = offset*model_view_matrix_dbl[4*2+2];

    model_view_matrix_dbl[4*3+0] += uxd;
    model_view_matrix_dbl[4*3+1] += uyd;
    model_view_matrix_dbl[4*3+2] += uzd;


    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixd(model_view_matrix_dbl);


    //glSetDoublev(GL_MODELVIEW_MATRIX, model_view_matrix_dbl);

    update_camera_matrices();


    //right = vec3_euler_rotation(right, theta+1.00, phi-1.00, 0.0 );
    //up = vec3_euler_rotation(up, theta+1.00, phi-1.00, 0.0 );

    right = vec3_euler_rotation(right, theta, phi, 0.0f);
    up = vec3_euler_rotation(up, theta, phi, 0.0f);

    //printf("V: theta= %f phi= %f \n", theta, phi);
    //vec3_print(look);
    //vec3_print(right);
    //vec3_print(up);

    setup_fulstrum(fov, ratio, z_far, this->position, look, right, up);

    setup_fulstrum2(
        fov, ratio, z_near,z_far,
        this->position, look, right, up);

    glColor3ub(255, 255, 255);
}

void Camera::set_angles(float theta, float phi)
{
    this->theta = theta;
    this->phi = phi;
}

void Camera::hud_projection()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // DEPRECATE GLU
    gluOrtho2D(0, x_size, 0, y_size);
    // DEPRECATE GLU

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

Vec3 Camera::forward_vector()
{
    return vec3_euler_rotation(vec3_init(1, 0, 0), this->theta, this->phi, 0.0f);
}

Vec3 Camera::lateral_vector()
{
    return vec3_euler_rotation(vec3_init(1, 0, 0), this->theta, 0.0f, 0.0f);
}

void Camera::copy_state_from(const Camera* c)
{
    IF_ASSERT(c == NULL) return;
    this->set_position(c->get_position());
    this->set_angles(c->theta, c->phi);
}

void update_camera_matrices()
{
    glGetFloatv(GL_MODELVIEW_MATRIX, model_view_matrix);
    glGetDoublev(GL_MODELVIEW_MATRIX, model_view_matrix_dbl);
    glGetDoublev(GL_PROJECTION_MATRIX, projection_matrix);
    glGetIntegerv(GL_VIEWPORT, viewport);
}

Camera* get_agent_camera()
{
    return agent_camera;
}

Camera* get_free_camera()
{
    return free_camera;
}

void use_agent_camera()
{
    current_camera = agent_camera;
}

void use_free_camera()
{
    current_camera = free_camera;
}

void world_projection()
{
    current_camera->world_projection();
}

void hud_projection()
{
    current_camera->hud_projection();
}

void update_agent_camera()
{
    class Agents::Agent* you = ClientState::player_agent.you();
    if (you == NULL) return;
    // update camera_state
    ClientState::player_agent.pump_camera();

    // set agent_camera from state
    Vec3 pos = ClientState::player_agent.camera_position();
    agent_camera->set_position(pos);
}

void update_camera_settings(float view_distance)
{
    static const float min_vd = 32.0f;
    static const float max_vd = 128.0f;
    if (view_distance < min_vd) view_distance = min_vd;
    if (view_distance > max_vd) view_distance = max_vd;
    CAMERA_VIEW_DISTANCE = view_distance;
    CAMERA_VIEW_DISTANCE_SQUARED = view_distance*view_distance;
    printf("Camera view distance : %0.2f\n", view_distance);
}
