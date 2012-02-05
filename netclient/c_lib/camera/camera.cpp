#include "./camera.hpp"

#include <c_lib/camera/fulstrum_test.hpp>

#include <c_lib/physics/vector.hpp>
#include <c_lib/physics/matrix.hpp>

#include <c_lib/input/input.hpp>
#include <c_lib/voxel/voxel_loader.hpp>

extern float _xresf;
extern float _yresf;

CCamera* agent_camera = NULL;
CCamera* free_camera = NULL;
CCamera* current_camera = NULL;

float model_view_matrix[16];
double model_view_matrix_dbl[16];
double projection_matrix[16];
GLint viewport[4];

int first_person = 0;

void set_camera(CCamera* cam) {
    current_camera = cam;
}

void set_camera_first_person(int fp) {
    first_person = fp;
}

void init_cameras()
{
    agent_camera = new CCamera();
    agent_camera->first_person = true;

    free_camera = new CCamera();
    free_camera->first_person = false;

    current_camera = free_camera;
}

void teardown_cameras()
{
    if (agent_camera != NULL)
        delete agent_camera;
    if (free_camera != NULL)
        delete free_camera;
}

CCamera::CCamera() {
    set_aspect(85.0f, 0.1f, 1000.0f);
    set_dimensions();
    set_projection(0.0f, 0.0f, 0.0f, 0.5f, 0.0f);
}

int CCamera::is_current() {
    if (this == current_camera) {
        return 1;
    }
    return 0;
}

void CCamera::set_aspect(float fov, float z_near, float z_far) {
    this->fov = fov;
    this->z_near = z_near;
    this->z_far = z_far;
}

void CCamera::set_fov(float fov) {
    this->fov = fov;
}

void CCamera::set_projection(float x, float y, float z, float theta, float phi) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->theta = theta;
    this->phi = phi;
}

void CCamera::set_dimensions() {
    x_size = _xresf;
    y_size = _yresf;
    ratio = x_size / y_size;
}

void CCamera::pan(float dx, float dy) {    // args are deltas
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

void CCamera::move(float dx, float dy, float dz) {
    x += dx*cos(theta * PI);
    x += dy*cos(theta * PI + PI/2.0f);
    y += dx*sin(theta * PI);
    y += dy*sin(theta * PI + PI/2.0f);
    z += dz;
}

void CCamera::world_projection()
{

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective( fov, ratio, z_near, z_far);

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    Vector f = Vector_init(1.0, 0.0, 0.0);
    Vector r = Vector_init(0.0, 1.0, 0.0);
    Vector u = Vector_init(0.0, 0.0, 1.0);
    
    Vector _l  = euler_rotation(f, theta + 1.00, phi - 1.00, 0.0 );

    xl = _l.x; yl = _l.y; zl = _l.z;

    xu = 0.0f;
    yu = 0.0f;
    zu = 1.0f;

    gluLookAt(x,y,z,
               x+xl, y+yl, z+zl,
               xu, yu, zu);

    update_camera_matrices();
    
    //set fulstrum camera up
    {

        f = euler_rotation(f, theta+1.00, phi - 1.00, 0.0 );
        r = euler_rotation(r, theta+1.00, phi - 1.00, 0.0 );
        u = euler_rotation(u, theta+1.00, phi - 1.00, 0.0 );

        setup_fulstrum(fov, ratio, z_far, Vector_init(x,y,z), &f,&r,&u );
         //fulstrum test
    }

    glEnable (GL_DEPTH_TEST);

    glColor3ub(255, 255, 255);

}

void CCamera::set_angles(float theta, float phi) {
    this->theta = theta;
    this->phi = phi;
}

void CCamera::hud_projection() {

    glDisable (GL_DEPTH_TEST);   // end world projection

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, x_size, 0, y_size);

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
}

void CCamera::forward_vector(float f[3])
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


CCamera* get_agent_camera()
{
    return agent_camera;
}

CCamera* get_free_camera()
{
    return free_camera;
}

void use_agent_camera()
{
    current_camera = agent_camera;
    set_camera_first_person(current_camera->first_person);
}

void use_free_camera()
{
    current_camera = free_camera;
    set_camera_first_person(current_camera->first_person);
}

void camera_input_update(int delta_tick, bool invert, float sensitivity)
{
    struct MouseMotionAverage* mm;
    mm = get_mouse_render_state(delta_tick);

    int inv = (invert) ? 1 : -1;
    float dx,dy;
    dx = ((float)(-mm->x) * sensitivity) / 40000.0f; // magic #
    dy = ((float)(inv * mm->y) * sensitivity) / 40000.0f; // magic #

    current_camera->pan(dx,dy);
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
