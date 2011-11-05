#include "./camera.h"

#define PI 3.14159265

extern int _xres;
extern int _yres;

static struct Camera* camera;

int _set_camera(struct Camera* c) {
    camera = c;
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

static float* model_view_matrix;

void set_model_view_matrix(float *a){
    model_view_matrix = a;
}

int _world_projection(struct Camera* c) {

    c->x_size = (float) _xres;
    c->y_size = (float) _yres;

    float aspect = c->x_size / c->y_size;
    float length;

    c->ratio = c->x_size / c->y_size;

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective( c->fov, aspect, c->z_near, c->z_far);

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    c->xl = c->x + cos( c->x_angle * PI) * cos( c->y_angle * PI);
    c->yl = c->y + sin( c->x_angle * PI) * cos( c->y_angle * PI);
    c->zl = c->z + sin( c->y_angle);

    length = sqrt(c->xl*c->xl + c->yl*c->yl + c->zl*c->zl);

    c->xu = 0;
    c->yu = 0;
    c->zu = 1;

    gluLookAt(c->x,c->y,c->z,
               c->xl, c->yl, c->zl,
               c->xu, c->yu, c->zu);

    if(model_view_matrix != NULL){
        glGetFloatv(GL_MODELVIEW_MATRIX, model_view_matrix);
    } else {
        printf("!? null pointer \n");
    }

    glEnable (GL_DEPTH_TEST);

    glColor3ub(255, 255, 255);
    return 0;
}

void __inline end_world_projection() {
   glDisable (GL_DEPTH_TEST);
}

int _hud_projection(struct Camera * c) {

end_world_projection();

glMatrixMode(GL_PROJECTION);
glLoadIdentity();
gluOrtho2D(0, c->x_size, 0, c->y_size);

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
