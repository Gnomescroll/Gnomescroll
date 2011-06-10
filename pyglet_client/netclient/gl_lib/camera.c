#include <SDL.h>
#include <SDL_opengl.h>

typedef struct {
//camera lens params
float fov, x_size, y_size, z_near, z_far;
//camera aspect params
float x,y,z,x_angle,y_angle;
} Camera ;


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

/*
    def worldProjection(self):
        glMatrixMode(GL_PROJECTION)
        glLoadIdentity()
        aspect = self.win.width / float(self.win.height)
        #gluPerspective( 45.0 / self.scale, aspect, 0.1, 100.0);
        gluPerspective(65, aspect, .1, 1000)


        glMatrixMode( GL_MODELVIEW )
        glLoadIdentity()

        #camera_focus_x = self.x + cos( self.x_angle * pi)
        #camera_focus_y = self.y + sin( self.x_angle * pi)
        #camera_focus_z = self.z + sin( self.y_angle)

        camera_focus_x = self.x + cos( self.x_angle * pi) * cos( self.y_angle * pi)
        camera_focus_y = self.y + sin( self.x_angle * pi) * cos( self.y_angle * pi)
        camera_focus_z = self.z + sin( self.y_angle)

        gluLookAt( self.x, self.y, self.z,
                camera_focus_x, camera_focus_y, camera_focus_z,
                0., 0., 1.0)

        glEnable (GL_DEPTH_TEST)
        #glEnable(GL_CULL_FACE);
*/


}

void _hud_projection( Camera c) {

    //fix
    float aspect = c.x_size / c.y_size;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective( c.fov, aspect, c.z_near, c.z_far);
}
