/* moved to header
#include <SDL.h>
#include <SDL_opengl.h>

#include <math.h>

struct Camera {
float fov;
float x_size;
float y_size;
float z_near;
float z_far;

float x,y,z;
float x_angle;
float y_angle;
};
*/

#include "./camera.h"

#define PI 3.14159265

/*
void _camera_projection( Camera c) {
    float aspect = c.x_size / c.y_size;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective( c.fov, aspect, c.z_near, c.z_far);
}
*/

struct Camera* camera;

float * model_view_matrix;

void set_model_view_matrix(float *a){
    model_view_matrix = a;
}

int _world_projection(struct Camera* c) {
    float aspect = c->x_size / c->y_size;
    float camera_focus_x,  camera_focus_y,  camera_focus_z;
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
    //c->xl /= length;
    //c->yl /= length;
    //c->zl /= length;

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

    //printf( "(%f, %f, %f), (%f, %f, %f) \n",c.x,c.y,c.z,camera_focus_x, camera_focus_y,  camera_focus_z );

    //glEnable(GL_TEXTURE_2D);

    glEnable (GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);

/*
        glEnable (GL_DEPTH_TEST)
        #glEnable(GL_CULL_FACE);
*/
    //printf( "gl error: %s\n", SDL_GetError() );
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

//printf( "(%f, %f) \n", c.x_size, c.y_size);

glMatrixMode( GL_MODELVIEW );
glLoadIdentity();

glDisable(GL_DEPTH_TEST);

glEnable(GL_TEXTURE_2D);
//glEnable(GL_TEXTURE_2D);

/*
         glMatrixMode(GL_PROJECTION)
        glLoadIdentity()
        gluOrtho2D(0, self.win.width, 0, self.win.height)

        glMatrixMode( GL_MODELVIEW )
        glLoadIdentity()

        glDisable(GL_DEPTH_TEST);
        #glDisable(GL_CULL_FACE);
        glEnable(gl.GL_TEXTURE_2D)
*/
return 0;
}

struct Camera* _get_camera() {
    return camera;
}
