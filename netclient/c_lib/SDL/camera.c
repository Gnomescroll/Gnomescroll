//#include "./camera.h"

//#define PI 3.14159265

//// revert to extern once SDL_functions migrated to c_lib
//extern int _xres;
//extern int _yres;
//extern int _fullscreen;

//int _set_resolution_camera(int xres, int yres, int fullscreen) {
    //_xres = xres;
    //_yres = yres;
    //_fullscreen = fullscreen;
    //return 0;
//}

//static struct Camera* camera;

//int _set_camera(struct Camera* c_cam) {
    //camera = c_cam;
    //return 0;
//}

//struct Camera* _get_camera() {
    //if(camera != NULL) {
        //printf("Get Camera: Camera not null \n");
    //} else {
        //printf("Get Camera: Error: camera is null \n");
    //}
    //return camera;
//}

//static float a_cam[16];

//int _world_projection(struct Camera* c_cam) {

    //c_cam->x_size = (float) _xres;
    //c_cam->y_size = (float) _yres;

    //float aspect = c_cam->x_size / c_cam->y_size;
    //float length;

    //c_cam->ratio = c_cam->x_size / c_cam->y_size;

    //glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    //glMatrixMode(GL_PROJECTION);
    //glLoadIdentity();
    //gluPerspective( c_cam->fov, aspect, c_cam->z_near, c_cam->z_far);

    //glMatrixMode( GL_MODELVIEW );
    //glLoadIdentity();

    //c_cam->xl = c_cam->x + cos( c_cam->x_angle * PI) * cos( c_cam->y_angle * PI);
    //c_cam->yl = c_cam->y + sin( c_cam->x_angle * PI) * cos( c_cam->y_angle * PI);
    //c_cam->zl = c_cam->z + sin( c_cam->y_angle);

    //length = sqrt(c_cam->xl*c_cam->xl + c_cam->yl*c_cam->yl + c_cam->zl*c_cam->zl);

    //c_cam->xu = 0;
    //c_cam->yu = 0;
    //c_cam->zu = 1;

    //gluLookAt(c_cam->x,c_cam->y,c_cam->z,
               //c_cam->xl, c_cam->yl, c_cam->zl,
               //c_cam->xu, c_cam->yu, c_cam->zu);

    //if(a_cam != NULL){
        //glGetFloatv(GL_MODELVIEW_MATRIX, a_cam);
    //} else {
        //printf("camera.c_cam _world_projection :: a_cam is null\n");
    //}

    //glEnable (GL_DEPTH_TEST);

    //glColor3ub(255, 255, 255);
    //return 0;
//}

//void __inline end_world_projection() {
   //glDisable (GL_DEPTH_TEST);
//}

//int _hud_projection(struct Camera * c_cam) {

//end_world_projection();

//glMatrixMode(GL_PROJECTION);
//glLoadIdentity();
//gluOrtho2D(0, c_cam->x_size, 0, c_cam->y_size);

//glMatrixMode( GL_MODELVIEW );
//glLoadIdentity();

//glDisable(GL_DEPTH_TEST);

//glEnable(GL_TEXTURE_2D);

//return 0;
//}

//void _set_camera_state(float x, float y, float z, float theta, float phi) {
    //camera->x = x;
    //camera->y = y;
    //camera->z = z;

    //camera->x_angle = theta;
    //camera->y_angle = phi;
//}

//void _pan_camera(float dtheta, float dphi) {    // args are deltas
    //camera->x_angle += dtheta;
    //camera->y_angle += dphi;

    //if (camera->y_angle > 0.499f) {
        //camera->y_angle = 0.499f;
    //} else if (camera->x_angle < -0.499f) {
        //camera->x_angle = -0.499f;
    //}
//}


///* Copied from particle functions. need the projection matrix */



////#include <compat_gl.h>

//////#include "camera.h"

////#include "math.h"

////#include <stdio.h>
////#include <stdlib.h>
////#include <string.h>


////int _init_particle_functions();

////SDL_Surface *cam_surface;
////GLuint particle_texture;

////struct Camera* c_cam=NULL;

////static inline void _begin_matrix () {
    ////glGetFloatv(GL_MODELVIEW_MATRIX, a_cam);
    ////glPushMatrix();

    ////int i,j;
    ////for( i=0; i<3; i++ )
        ////for( j=0; j<3; j++ ) {
            ////if ( i==j ) {
                ////a_cam[i*4+j] = 1.0;
            ////} else {
                ////a_cam[i*4+j] = 0.0;
            ////}
    ////}
    ////glLoadMatrixf(a_cam);
////}

////int _init_particle_functions_camera() {

    ////_begin_matrix();

    ////cam_surface=IMG_Load("./media/texture/particles_01.png");
    ////if(!cam_surface) {
        ////printf("_init_particle_functions, IMG_Load: %s \n", IMG_GetError());
        ////return 0;
    ////}
    ////if(cam_surface->format->BytesPerPixel != 4) {printf("IMG_Load: image is missing alpha channel \n"); return 0;}

    ////glEnable(GL_TEXTURE_2D);
    ////glGenTextures( 1, &particle_texture );
    ////glBindTexture( GL_TEXTURE_2D, particle_texture );
    ////glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR ); ///tweak?
    ////glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR ); ///tweak?

    ////glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    ////glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    ////glTexImage2D(GL_TEXTURE_2D, 0, 4, cam_surface->w, cam_surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, cam_surface->pixels );
    ////glDisable(GL_TEXTURE_2D);

    ////printf("particle_texture_id= %i \n", particle_texture);
////}

////void __inline bb_q(float x, float y, float z) {

////float x_,y_,z_;

////x_ = a_cam[0]*x+a_cam[4]*y+a_cam[8]*z;
////y_ = a_cam[1]*x+a_cam[5]*y+a_cam[9]*z;
////z_ = a_cam[2]*x+a_cam[6]*y+a_cam[10]*z;

////glVertex3f( x_, y_, z_ );
////}

////struct Vec {
////float x,y,z;
////};

////inline struct Vec init_Vec(float x, float y, float z) {
    ////struct Vec v; v.x=x;v.y=y;v.z=z; return v;
////}

////inline void normalize(struct Vec* v) {
////float l = sqrt(v->x*v->x + v->y*v->y + v->z*v->z);
////v->x /= l; v->y /=l; v->z /=l;
////}

////struct Vec cross(struct Vec v1, struct Vec v2) {
    ////struct Vec v0;
    ////v0.x = v1.y*v2.z - v1.z*v2.y;
    ////v0.y = v1.z*v2.x - v1.x*v2.z;
    ////v0.z = v1.x*v2.y - v1.y*v2.x;
    ////return v0;
////}

////void dot(struct Vec v1, struct Vec v2) {
    ////float d;
    ////d = v1.x*v2.x + v1.y*v2.y + v1.z*+v2.z;
    //////printf("dot= %f \n", d);
////}

////void calc_len(struct Vec *v) {
    ////float l;
    ////l = sqrt(v->x*v->x + v->y*v->y + v->z*v->z);
    ////v->x /= l; v->y /=l; v->z /=l;
    //////printf("l= %fs \n", l);
    ////}

////void draw(struct Vec v0, struct Vec v1, int r, int g, int b) {

////glColor3ub((unsigned char)r,(unsigned char)g,(unsigned char)b);
////glBegin(GL_LINES);
////glVertex3f(v0.x, v0.y, v0.z);
////glVertex3f(v0.x+v1.x, v0.y+v1.y, v0.z+v1.z);
////glEnd();
//////glColor3ub(255,255,255);
////}

////void init_camera_c() {
    ////_init_particle_functions_camera();
////}

/* moved to header

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

extern int _xres;
extern int _yres;

/*
void _camera_projection( Camera c) {
    float aspect = c.x_size / c.y_size;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective( c.fov, aspect, c.z_near, c.z_far);
}
*/

struct Camera* camera;

int _set_camera(struct Camera* c) {
    /*
    if(c != NULL) {
        printf("Set Camera: Camera not null \n");
    } else {
        printf("Set Camera: Error: camera is null \n");
    }
    */
    //while(1);
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

float * model_view_matrix;

void set_model_view_matrix(float *a){
    model_view_matrix = a;
}

int _world_projection(struct Camera* c) {
    //float aspect = c->x_size / c->y_size;

    c->x_size = (float) _xres;
    c->y_size = (float) _yres;

    float aspect = c->x_size / c->y_size;
    //float camera_focus_x,  camera_focus_y,  camera_focus_z;
    float length;
    //camera = c;
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

//printf( "(%f, %f) (%i,%i)\n", c->x_size, c->y_size, _xres, _yres);

glMatrixMode( GL_MODELVIEW );
glLoadIdentity();

glDisable(GL_DEPTH_TEST);

glEnable(GL_TEXTURE_2D);

//glClearColor(0, 0, 0, 0);
//glClear(GL_COLOR_BUFFER_BIT); //causes terrain not to render

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


// NEW
//void _pan_camera(float dtheta, float dphi) {    // args are deltas
    //camera->x_angle += dtheta;
    //camera->y_angle += dphi;

    //if (camera->y_angle > 0.499f) {
        //camera->y_angle = 0.499f;
    //} else if (camera->x_angle < -0.499f) {
        //camera->x_angle = -0.499f;
    //}
//}
