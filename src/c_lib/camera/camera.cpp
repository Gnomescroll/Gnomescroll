#include "./camera.hpp"

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
    agent_camera->set_position(vec3_init(0,0,50.0f));
    agent_camera->set_fov(85.0f);
    
    free_camera = new Camera();
    free_camera->first_person = false;
    free_camera->set_position(vec3_init(64,64,128));
    free_camera->set_fov(85.0f);

    current_camera = free_camera;

    Vec3 f = vec3_init(1.0f, 0.0f, 0.0f);
    Vec3 r = vec3_init(0.0f, 1.0f, 0.0f);
    Vec3 u = vec3_init(0.0f, 0.0f, 1.0f);
/*
    f = vec3_euler_rotation(f, current_camera->theta+1.00, current_camera->phi - 1.00, 0.0 );
    r = vec3_euler_rotation(r, current_camera->theta+1.00, current_camera->phi - 1.00, 0.0 );
    u = vec3_euler_rotation(u, current_camera->theta+1.00, current_camera->phi - 1.00, 0.0 );
*/
    current_camera->theta = 0;
    current_camera->phi = 0;

    f = vec3_euler_rotation(f, current_camera->theta, current_camera->phi, 0.0f );
    r = vec3_euler_rotation(r, current_camera->theta, current_camera->phi, 0.0f );
    u = vec3_euler_rotation(u, current_camera->theta, current_camera->phi, 0.0f );

    //printf("start: \n");
    //vec3_print(f);
    //vec3_print(r);
    //vec3_print(u);

    Vec3 p = current_camera->get_position();
    setup_fulstrum(
        current_camera->fov, current_camera->ratio, current_camera->z_far,
        p,f,r,u
    );

    setup_fulstrum2(
        current_camera->fov, current_camera->ratio, current_camera->z_near,current_camera->z_far,
        p,f,r,u
    );
}

void teardown_cameras()
{
    if (agent_camera != NULL)
        delete agent_camera;
    if (free_camera != NULL)
        delete free_camera;
}

#define CAMERA_ZOOM_FACTOR 2.0f
Camera::Camera()
:
zoomed(false),
zoom_factor(CAMERA_ZOOM_FACTOR)
{
	const float FOV = 75.0f;
	const float Z_NEAR = 0.1f;
	const float Z_FAR = 320.0f;
    set_aspect(FOV, Z_NEAR, Z_FAR);
    set_dimensions();
    set_projection(0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
}
#undef CAMERA_ZOOM_FACTOR

void Camera::toggle_zoom()
{
    if (zoomed)
        this->unzoom();
    else
        this->zoom();
}

void Camera::zoom()
{
    if (this->zoomed) return;
    this->zoomed = true;
    this->fov /= this->zoom_factor;
}

void Camera::unzoom()
{
    if (!this->zoomed) return;
    this->zoomed = false;
    this->fov *= this->zoom_factor;
}

bool Camera::is_current()
{
    if (this == current_camera) return true;
    return false;
}

void Camera::set_aspect(float fov, float z_near, float z_far)
{
    this->fov = fov;
    this->z_near = z_near;
    this->z_far = z_far;
}

void Camera::set_fov(float fov)
{
    this->fov = fov;
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
    ASSERT_BOXED_POSITION(this->position);
    if (this->is_current()) current_camera_position = this->position;
    if (this->is_current()) GS_ASSERT(vec3_equal(this->position, current_camera_position));
}

void Camera::pan(float dx, float dy)
{   // args are deltas
    theta += dx;
    phi += dy;

    if (theta > 1.0f) theta -= 2.0f;
    else if (theta < -1.0f) theta += 2.0f;

    // DO NOT ADD ANY MORE SIGNIFICANT DIGITS TO 0.4999f
    // Camera behavior when looking straight up or down is fucked up otherwise
    if (phi > 0.4999f) phi = 0.4999f;
    else if (phi < -0.4999f) phi = -0.4999f;
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

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
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

    //glEnable(GL_DEPTH_TEST);

    //glDisable(GL_DEPTH_TEST);
    //glEnable(GL_TEXTURE_2D);
}

Vec3 Camera::forward_vector()
{
    Vec3 f = vec3_init(1.0f, 0.0f, 0.0f);
    f = vec3_euler_rotation(f, this->theta, this->phi, 0.0f);
	normalize_vector(&f);
	return f;
}

void Camera::copy_state_from(Camera* c)
{
    GS_ASSERT(c != NULL);
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
    if (ClientState::playerAgent_state.you == NULL) return;
    // update camera_state
    ClientState::playerAgent_state.pump_camera();

    // set agent_camera from state
    Vec3 pos = ClientState::playerAgent_state.camera_position();
    agent_camera->set_position(pos);
}
