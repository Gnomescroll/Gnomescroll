#include "./camera.h"

extern int _xres;
extern int _yres;

static struct Camera* camera;

int _set_camera(struct Camera* c_cam) {
    camera = c_cam;
    return 0;
}

struct Camera* _get_camera() {
    if(camera != NULL) {
        printf("Get Camera: Camera not null \n");
    } else {
        printf("Get Camera: Error: camera is null \n");
    }
    return camera;
}

static float *model_view_matrix;

void set_model_view_matrix(float *a) {
    model_view_matrix = a;
}

int _world_projection(struct Camera* c_cam) {

    c_cam->x_size = (float) _xres;
    c_cam->y_size = (float) _yres;

    float aspect = c_cam->x_size / c_cam->y_size;

    c_cam->ratio = c_cam->x_size / c_cam->y_size;

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective( c_cam->fov, aspect, c_cam->z_near, c_cam->z_far);

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    c_cam->xl = c_cam->x + cos( c_cam->x_angle * PI) * cos( c_cam->y_angle * PI);
    c_cam->yl = c_cam->y + sin( c_cam->x_angle * PI) * cos( c_cam->y_angle * PI);
    c_cam->zl = c_cam->z + sin( c_cam->y_angle);

    c_cam->xu = 0;
    c_cam->yu = 0;
    c_cam->zu = 1;

    gluLookAt(c_cam->x,c_cam->y,c_cam->z,
               c_cam->xl, c_cam->yl, c_cam->zl,
               c_cam->xu, c_cam->yu, c_cam->zu);

    if(model_view_matrix != NULL){
        glGetFloatv(GL_MODELVIEW_MATRIX, model_view_matrix);
    } else {
        printf("camera.c_cam _world_projection :: model_view_matrix is null\n");
    }

    glEnable (GL_DEPTH_TEST);

    glColor3ub(255, 255, 255);
    return 0;
}

inline void end_world_projection() {
   glDisable (GL_DEPTH_TEST);
}

int _hud_projection(struct Camera * c_cam) {

    end_world_projection();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, c_cam->x_size, 0, c_cam->y_size);

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);

    glEnable(GL_TEXTURE_2D);

    return 0;
}

void _set_camera_state(float x, float y, float z, float theta, float phi) {
    camera->x = x;
    camera->y = y;
    camera->z = z;

    camera->x_angle = theta;
    camera->y_angle = phi;
}

void _pan_camera(float dtheta, float dphi) {    // args are deltas
    camera->x_angle += dtheta;
    camera->y_angle += dphi;

    if (camera->x_angle > 0.499999f) {
        camera->x_angle = 0.499999f;
    } else if (camera->x_angle < -0.499999f) {
        camera->x_angle = -0.499999f;
    }

    if (camera->y_angle > 0.499999f) {
        camera->y_angle = 0.499999f;
    } else if (camera->y_angle < -0.499999f) {
        camera->x_angle = -0.499999f;
    }

}
