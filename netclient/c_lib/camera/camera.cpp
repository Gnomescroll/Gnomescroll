#include "./camera.hpp"

#include <c_lib/camera/fulstrum_test.hpp>

#include <c_lib/physics/vector.hpp>
#include <c_lib/physics/matrix.hpp>

extern float _xresf;
extern float _yresf;

static CCamera* cameras = NULL;

CCamera* current_camera = NULL;
float model_view_matrix[16];

double projection_matrix[16];
GLint viewport[4];

int first_person = 0;

void set_camera(CCamera* cam) {
    current_camera = cam;
}

void set_camera_first_person(int fp) {
    first_person = fp;
}

CCamera* get_available_camera() {
    static int ctr = 0;
    if (ctr >= N_CAMERAS) {
        printf("No more available cameras\n");
        return NULL;
    }
    CCamera* cam = &cameras[ctr];
    ctr++;
    return cam;
}

void init_cameras()
{
    cameras = new CCamera[N_CAMERAS];
    set_camera(0);
}

void teardown_cameras()
{
    if (cameras != NULL)
        delete[] cameras;
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

void CCamera::set_projection(float x, float y, float z, float x_angle, float y_angle) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->x_angle = x_angle;
    this->y_angle = y_angle;
}

void CCamera::set_dimensions() {
    x_size = _xresf;
    y_size = _yresf;
    ratio = x_size / y_size;
}

void CCamera::pan(float dx, float dy) {    // args are deltas
    x_angle += dx;
    y_angle += dy;

    if (x_angle > 1.0f) {
        x_angle -= 2.0f;
    } else if (x_angle < -1.0f) {
        x_angle += 2.0f;
    }

    // DO NOT ADD ANY MORE SIGNIFICANT DIGITS TO 0.4999f
    // Camera behavior when looking straight up or down is fucked up otherwise
    if (y_angle > 0.4999f) {
        y_angle = 0.4999f;
    } else if (y_angle < -0.4999f) {
        y_angle = -0.4999f;
    }
}

void CCamera::move(float dx, float dy, float dz) {
    x += dx*cos(x_angle * PI);
    x += dy*cos(x_angle * PI + PI/2.0f);
    y += dx*sin(x_angle * PI);
    y += dy*sin(x_angle * PI + PI/2.0f);
    z += dz;
}

//#include <voxel/voxel_volume.hpp>
//#include <voxel/voxel_render.hpp>

#include <c_lib/voxel/voxel_loader.hpp>

void CCamera::world_projection() {

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective( fov, ratio, z_near, z_far);

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();


/*
    double _x_angle = x_angle * PI; 
    double _y_angle = y_angle * PI;

    xl = cos( _x_angle ) * cos( _y_angle );
    yl = sin( _x_angle ) * cos( _y_angle );
    zl = sin( _y_angle );
*/

    Vector f = Vector_init(1.0, 0.0, 0.0);
    Vector r = Vector_init(0.0, 1.0, 0.0);
    Vector u = Vector_init(0.0, 0.0, 1.0);
    
    Vector _l  = euler_rotation(f, x_angle + 1.00, y_angle - 1.00, 0.0 );

    xl = _l.x; yl = _l.y; zl = _l.z;

    xu = 0.0f;
    yu = 0.0f;
    zu = 1.0f;

    gluLookAt(x,y,z,
               x+xl, y+yl, z+zl,
               xu, yu, zu);

    if(model_view_matrix != NULL) {
        glGetFloatv(GL_MODELVIEW_MATRIX, model_view_matrix);
    } else {
        printf("camera world_projection :: model_view_matrix is null\n");
    }

    //set fulstrum camera up
    {

        f = euler_rotation(f, x_angle+1.00, y_angle - 1.00, 0.0 );
        r = euler_rotation(r, x_angle+1.00, y_angle - 1.00, 0.0 );
        u = euler_rotation(u, x_angle+1.00, y_angle - 1.00, 0.0 );

        setup_fulstrum(fov, ratio, z_far, Vector_init(x,y,z), &f,&r,&u );
         //fulstrum test
    }

    glEnable (GL_DEPTH_TEST);

    glColor3ub(255, 255, 255);

}

void CCamera::set_angles(float theta, float phi) {
    this->x_angle = theta;
    this->y_angle = phi;
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
