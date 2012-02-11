#include "./camera.hpp"

#include <c_lib/camera/fulstrum_test.hpp>

#include <c_lib/physics/vec3.hpp>
#include <c_lib/physics/mat3.hpp>

#include <c_lib/input/input.hpp>
#include <c_lib/voxel/voxel_loader.hpp>

extern float _xresf;
extern float _yresf;

Camera* agent_camera = NULL;
Camera* free_camera = NULL;
Camera* current_camera = NULL;

float model_view_matrix[16];
double model_view_matrix_dbl[16];
double projection_matrix[16];
GLint viewport[4];

void set_camera(Camera* cam) {
    current_camera = cam;
}

void init_cameras()
{
    agent_camera = new Camera();
    agent_camera->first_person = true;
    agent_camera->set_state(0,0,50.0f);
    agent_camera->set_fov(85.0f);
    
    free_camera = new Camera();
    free_camera->first_person = false;
    free_camera->set_state(64,64,128);
    free_camera->set_fov(85.0f);

    current_camera = free_camera;
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
    set_aspect(85.0f, 0.1f, 1000.0f);
    set_dimensions();
    set_projection(0.0f, 0.0f, 0.0f, 0.5f, 0.0f);
}

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

int Camera::is_current() {
    if (this == current_camera) {
        return 1;
    }
    return 0;
}

void Camera::set_aspect(float fov, float z_near, float z_far) {
    this->fov = fov;
    this->z_near = z_near;
    this->z_far = z_far;
}

void Camera::set_fov(float fov) {
    this->fov = fov;
}

void Camera::set_projection(float x, float y, float z, float theta, float phi) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->theta = theta;
    this->phi = phi;
}

void Camera::set_dimensions() {
    x_size = _xresf;
    y_size = _yresf;
    ratio = x_size / y_size;
}

void Camera::set_state(float x, float y, float z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}

void Camera::pan(float dx, float dy) {    // args are deltas
    theta += dx;
    phi += dy;

    if (theta > 1.0f) {
        theta -= 2.0f;
    } else if (theta < -1.0f) {
        theta += 2.0f;
    }

    // DO NOT ADD ANY MORE SIGNIFICANT DIGITS TO 0.4999f
    // Camera behavior when looking straight up or down is fucked up otherwise
    if (phi > 0.4999f) {
        phi = 0.4999f;
    } else if (phi < -0.4999f) {
        phi = -0.4999f;
    }
}

void Camera::move(float dx, float dy, float dz) {
    x += dx*cos(theta * PI);
    x += dy*cos(theta * PI + PI/2.0f);
    y += dx*sin(theta * PI);
    y += dy*sin(theta * PI + PI/2.0f);
    z += dz;
}

void Camera::world_projection()
{

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

 /*
    DEPRECATE GLU
*/   
    gluPerspective( fov, ratio, z_near, z_far);
/*
    DEPRECATE GLU
*/

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    struct Vec3 _l;
    _l = vec3_init(1.0, 0.0, 0.0);
    _l = vec3_euler_rotation( _l, theta + 1.00, phi - 1.00, 0.0 );

    xl = _l.x; yl = _l.y; zl = _l.z;

    xu = 0.0f;
    yu = 0.0f;
    zu = 1.0f;

/*
    DEPRECATE GLU
*/
    gluLookAt(x,y,z,
               x+xl, y+yl, z+zl,
               xu, yu, zu);

/*
    DEPRECATE GLU
*/
    update_camera_matrices();
    
    //set fulstrum camera up
    {
        Vec3 f = vec3_init(1.0, 0.0, 0.0);
        Vec3 r = vec3_init(0.0, 1.0, 0.0);
        Vec3 u = vec3_init(0.0, 0.0, 1.0);

        f = vec3_euler_rotation(f, theta+1.00, phi - 1.00, 0.0 );
        r = vec3_euler_rotation(r, theta+1.00, phi - 1.00, 0.0 );
        u = vec3_euler_rotation(u, theta+1.00, phi - 1.00, 0.0 );

        setup_fulstrum(fov, ratio, z_far, vec3_init(x,y,z), &f,&r,&u );
         //fulstrum test
    }

    glEnable (GL_DEPTH_TEST);

    glColor3ub(255, 255, 255);

}

void Camera::set_angles(float theta, float phi) {
    this->theta = theta;
    this->phi = phi;
}

void Camera::hud_projection() {

    glDisable (GL_DEPTH_TEST);   // end world projection

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

/*
    DEPRECATE GLU
*/
    gluOrtho2D(0, x_size, 0, y_size);
/*
    DEPRECATE GLU
*/

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
}

void Camera::forward_vector(float f[3])
{

    float xa = theta;
    float ya = phi;
    if (theta > 1.0f) {
        xa -= 2.0f;
    } else if (theta < -1.0f) {
        xa += 2.0f;
    }

    // DO NOT ADD ANY MORE SIGNIFICANT DIGITS TO 0.4999f
    // Camera behavior when looking straight up or down is fucked up otherwise
    if (phi > 0.4999f) {
        ya = 0.4999f;
    } else if (phi < -0.4999f) {
        ya = -0.4999f;
    }
    
    f[0] = cos( xa * PI) * cos( ya * PI);
    f[1] = sin( xa * PI) * cos( ya * PI);
    f[2] = sin( ya * PI);

    // normalize
    float len = sqrt(f[0]*f[0] + f[1]*f[1] + f[2]*f[2]);
    f[0] /= len;
    f[1] /= len;
    f[2] /= len;
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
    agent_camera->x = ClientState::playerAgent_state.camera_state.x;
    agent_camera->y = ClientState::playerAgent_state.camera_state.y;
    agent_camera->z = ClientState::playerAgent_state.camera_state.z + ClientState::playerAgent_state.you->camera_height();

    // set camera_state's angle from agent_camera
    //ClientState::playerAgent_state.camera_state.theta = agent_camera->theta;
    //ClientState::playerAgent_state.camera_state.phi = agent_camera->phi;
}
