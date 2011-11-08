#include "./current_camera.h"

extern int _xres;
extern int _yres;

static struct Camera cameras[N_CAMERAS];
static struct Camera* current_camera;

static float *model_view_matrix;

void _set_camera(struct Camera* cam) {
    current_camera = cam;
}

struct Camera* _get_camera() {
    if(current_camera != NULL) {
        printf("Get Camera: Camera not null \n");
    } else {
        printf("Get Camera: Error: current_camera is null \n");
    }
    return current_camera;
}

void set_model_view_matrix(float *a) {
    model_view_matrix = a;
}

int _world_projection(struct Camera* cam) {

    cam->x_size = (float) _xres;
    cam->y_size = (float) _yres;

    float aspect = cam->x_size / cam->y_size;

    cam->ratio = cam->x_size / cam->y_size;

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective( cam->fov, aspect, cam->z_near, cam->z_far);

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    cam->xl = cam->x + cos( cam->x_angle * PI) * cos( cam->y_angle * PI);
    cam->yl = cam->y + sin( cam->x_angle * PI) * cos( cam->y_angle * PI);
    cam->zl = cam->z + sin( cam->y_angle);

    cam->xu = 0;
    cam->yu = 0;
    cam->zu = 1;

    gluLookAt(cam->x,cam->y,cam->z,
               cam->xl, cam->yl, cam->zl,
               cam->xu, cam->yu, cam->zu);

    if(model_view_matrix != NULL){
        glGetFloatv(GL_MODELVIEW_MATRIX, model_view_matrix);
    } else {
        printf("current_camera.cam _world_projection :: model_view_matrix is null\n");
    }

    glEnable (GL_DEPTH_TEST);

    glColor3ub(255, 255, 255);
    return 0;
}

inline void end_world_projection() {
   glDisable (GL_DEPTH_TEST);
}

int _hud_projection(struct Camera *cam) {

    end_world_projection();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, cam->x_size, 0, cam->y_size);

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);

    glEnable(GL_TEXTURE_2D);

    return 0;
}

void _set_camera_state(float x, float y, float z, float theta, float phi) {
    current_camera->x = x;
    current_camera->y = y;
    current_camera->z = z;

    current_camera->x_angle = theta;
    current_camera->y_angle = phi;
}

void _pan_camera(float dtheta, float dphi) {    // args are deltas
    current_camera->x_angle += dtheta;
    current_camera->y_angle += dphi;

    if (current_camera->x_angle > 0.499999f) {
        current_camera->x_angle = 0.499999f;
    } else if (current_camera->x_angle < -0.499999f) {
        current_camera->x_angle = -0.499999f;
    }

    if (current_camera->y_angle > 0.499999f) {
        current_camera->y_angle = 0.499999f;
    } else if (current_camera->y_angle < -0.499999f) {
        current_camera->x_angle = -0.499999f;
    }

}
