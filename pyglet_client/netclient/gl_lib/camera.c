#include <SDL.h>
#include <SDL_opengl.h>

#include <math.h>

typedef struct {
//camera lens params
float fov, x_size, y_size, z_near, z_far;
//camera aspect params
float x,y,z,x_angle,y_angle;
} Camera ;

#define PI 3.14159265

/*
void _camera_projection( Camera c) {
    float aspect = c.x_size / c.y_size;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective( c.fov, aspect, c.z_near, c.z_far);
}
*/

void _world_projection( Camera c) {
    float aspect = c.x_size / c.y_size;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective( c.fov, aspect, c.z_near, c.z_far);

    float camera_focus_x,  camera_focus_y,  camera_focus_z;

    camera_focus_x = c.x + cos( c.x_angle * PI) * cos( c.y_angle * PI);
    camera_focus_y = c.y + sin( c.x_angle * PI) * cos( c.y_angle * PI);
    camera_focus_z = c.z + sin( c.y_angle);

    gluLookAt(c.x,c.y,c.z,
               camera_focus_x, camera_focus_y,  camera_focus_z,
               0, 0, 1);
    //glEnable(GL_TEXTURE_2D);

    //glEnable (GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);

/*
        glEnable (GL_DEPTH_TEST)
        #glEnable(GL_CULL_FACE);
*/


}

void _hud_projection( Camera c) {

glMatrixMode(GL_PROJECTION);
glLoadIdentity();
gluOrtho2D(0, c.x_size, 0, c.y_size);

glMatrixMode( GL_MODELVIEW );
glLoadIdentity();

glDisable(GL_DEPTH_TEST);
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

}
