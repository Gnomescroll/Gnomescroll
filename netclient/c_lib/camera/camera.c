#include "./camera.h"

#define PI 3.14159265

// revert to extern once SDL_functions migrated to c_lib
//extern int _xres;
//extern int _yres;
int _xres;
int _yres;
int _fullscreen; //remove

int _set_resolution_camera(int xres, int yres, int fullscreen) {
    _xres = xres;
    _yres = yres;
    _fullscreen = fullscreen;
    return 0;
}


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

static float* model_view_matrix;

void set_model_view_matrix(float *a_cam){
    model_view_matrix = a_cam;
}

int _world_projection(struct Camera* c_cam) {

    c_cam->x_size = (float) _xres;
    c_cam->y_size = (float) _yres;

    float aspect = c_cam->x_size / c_cam->y_size;
    float length;

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

    length = sqrt(c_cam->xl*c_cam->xl + c_cam->yl*c_cam->yl + c_cam->zl*c_cam->zl);

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

void __inline end_world_projection() {
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

    if (camera->y_angle > 0.499f) {
        camera->y_angle = 0.499f;
    } else if (camera->x_angle < -0.499f) {
        camera->x_angle = -0.499f;
    }
}


/* Copied from particle functions. need the projection matrix */



#include <compat_gl.h>

//#include "camera.h"

#include "math.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int _init_particle_functions();

//int _draw_particle(int id, float size, float x, float y, float z);
//int _draw_particle2(int id, float size, float x, float y, float z);

//int _planar_laser(float x0, float y0, float z0, float x1, float y1, float z1);
//int _planar_laser2(int density, float width, float x0, float y0, float z0, float x1, float y1, float z1);


static inline char *textFileRead(char *fn) {
    FILE *fp;
    char *content = NULL;

    int count=0;

    if (fn != NULL) {
        fp = fopen(fn,"rt");
        if (fp == NULL) {
            printf("Error: cannot open file %s \n", fn);
            return NULL;
        }

        if (fp != NULL) {

      fseek(fp, 0, SEEK_END);
      count = ftell(fp);
      rewind(fp);

            if (count > 0) {
                content = (char *)malloc(sizeof(char) * (count+1));
                count = fread(content,sizeof(char),count,fp);
                content[count] = '\0';
            }
            fclose(fp);
        }
    }
    return content;
}

//info log print
static inline void printShaderInfoLog(GLuint obj)
{
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;

    glGetShaderiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

    if (infologLength > 0)
    {
        infoLog = (char *)malloc(infologLength);
        glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
    printf("%s\n",infoLog);
        free(infoLog);
    }
}

static inline void printProgramInfoLog(GLuint obj)
{
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;

    glGetProgramiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

    if (infologLength > 0)
    {
        infoLog = (char *)malloc(infologLength);
        glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
    printf("%s\n",infoLog);
        free(infoLog);
    }
}

SDL_Surface *cam_surface;
GLuint particle_texture;

struct Camera* c_cam=NULL;


static float a_cam[16];

GLuint shader_program_1;

int setShaders() {
    char *vs,*gs,*fs;
    GLuint v,g,f;

    v = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
    g = glCreateShaderObjectARB(GL_GEOMETRY_SHADER_ARB);
    f = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

    vs = textFileRead("./media/shaders/test1.vert");
    gs = textFileRead("./media/shaders/test1.geom");
    fs = textFileRead("./media/shaders/test1.frag");

    const char * vv = vs;
    const char * ff = fs;
    const char * gg = gs;

    glShaderSourceARB(v, 1, &vv,NULL);
    glShaderSourceARB(f, 1, &ff,NULL);
    glShaderSourceARB(f, 1, &gg,NULL);

    free(vs);
    free(fs);
    free(gs);

    glCompileShaderARB(v); printShaderInfoLog(v); ///diag
    glCompileShaderARB(f); printShaderInfoLog(f); ///diag
    glCompileShaderARB(g); printShaderInfoLog(g); ///diag

    GLuint p;
    p = glCreateProgramObjectARB();

    glAttachObjectARB(p,v);
    glAttachObjectARB(p,f);
    glAttachObjectARB(p,g);

    glLinkProgramARB(p);
    printProgramInfoLog(p); // print diagonostic information
    shader_program_1 = p;

    //glUseProgramObjectARB(p);
    return 0;
}

int _init_particle_functions() {
    /*
        printf("set shaders\n");
        setShaders();
        printf("shaders set\n");
    */
    //while(1) {
    //    printf("t\n");
    //}

    //c_cam = _get_camera();
    /*
        if(c_cam != NULL) {
            printf("Particle Function Camera: Camera not null \n");
        } else {
            printf("Particle Function Camera: Error: camera is null \n");
        }
    */
    //set_model_view_matrix(&a_cam);
    glLoadMatrixf(a_cam);
    set_model_view_matrix(a_cam);

    cam_surface=IMG_Load("./media/texture/particles_01.png");
    if(!cam_surface) {
        printf("_init_particle_functions, IMG_Load: %s \n", IMG_GetError());
        return 0;
    }
    if(cam_surface->format->BytesPerPixel != 4) {printf("IMG_Load: image is missing alpha channel \n"); return 0;}

    glEnable(GL_TEXTURE_2D);
    glGenTextures( 1, &particle_texture );
    glBindTexture( GL_TEXTURE_2D, particle_texture );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR ); ///tweak?
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR ); ///tweak?

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glTexImage2D(GL_TEXTURE_2D, 0, 4, cam_surface->w, cam_surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, cam_surface->pixels );
    glDisable(GL_TEXTURE_2D);

    printf("particle_texture_id= %i \n", particle_texture);
}

void __inline bb_q(float x, float y, float z) {

float x_,y_,z_;

x_ = a_cam[0]*x+a_cam[4]*y+a_cam[8]*z;
y_ = a_cam[1]*x+a_cam[5]*y+a_cam[9]*z;
z_ = a_cam[2]*x+a_cam[6]*y+a_cam[10]*z;

/*
x_ = a_cam[0]*x+a_cam[1]*y+a_cam[2]*z;
y_ = a_cam[4]*x+a_cam[5]*y+a_cam[6]*z;
y_ = a_cam[8]*x+a_cam[9]*y+a_cam[10]*z;
*/
glVertex3f( x_, y_, z_ );
}

//int _draw_particle(int id, float size, float x, float y, float z) {
    //int i, j;
    //int p = 3; //deprecate this
///*
//position_camera();
//glGetDoublev(GL_MODELVIEW_MATRIX, &mv_matrix);
//rot_matrix=mv_matrix.get_sub(0, 0, 3, 3).inverse();
//billboard_alligned_quad_array=quad_array.mult_matrix(rot_matix);
//for(i=0; i<billboard_count; i++)
//{
    //glPushMatrix();
    //glTranslate(billboard[i].position);
    //bilboard[i].draw_using(billboard_alligned_quad_array);
    //glPopMatrix()
//}
//*/
    //glGetFloatv(GL_MODELVIEW_MATRIX, &a_cam);
    //glPushMatrix();

    ////glTranslatef(x,y,z);
///*
    //a_cam[0]=1;a_cam[5]=1;a_cam[10]=1;
    //a_cam[1]=0;a_cam[2]=0;a_cam[6]=0;
    //a_cam[4]=0;a_cam[8]=0;a_cam[9]=0;
//*/

//for( i=0; i<3; i++ )
    //for( j=0; j<3; j++ ) {
        //if ( i==j )
            //a_cam[i*4+j] = 1.0;
        //else
            //a_cam[i*4+j] = 0.0;
    //}
    ////GL_MODELVIEW_MATRIX
    //glLoadMatrixf(&a_cam);

    ////glTranslatef(x,y,z);
    ////struct Camera *c_cam;
    ////float x0,y0,z0
    ////c_cam = _get_camera();

    //glEnable(GL_TEXTURE_2D);
    //glBindTexture( GL_TEXTURE_2D, particle_texture );
    ////glEnable(GL_BLEND);
    ////glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    ////glColor3ub(255, 255, 255);

//glBegin( GL_QUADS );
    //if(p == 1) {
    //glTexCoord2f( 0.0, 0.0 );
    //glVertex3f( size, -size, 0 );

    //glTexCoord2f( 1.0/16.0, 0.0 );
    //glVertex3f( size, size, 0 );

    //glTexCoord2f( 1.0/16.0, 1.0/16.0 );
    //glVertex3f( -size, size, 0 );

    //glTexCoord2f( 0.0, 1.0/16.0 );
    //glVertex3f( -size,-size, 0 );
    //}
    //if(p == 2) {
    //glTexCoord2f( 0.0, 0.0 );
    //bb_q( size, -size, 0 );

    //glTexCoord2f( 1.0/16.0, 0.0 );
    //bb_q(size, size, 0 );

    //glTexCoord2f( 1.0/16.0, 1.0/16.0 );
    //bb_q(-size, size, 0 );

    //glTexCoord2f( 0.0, 1.0/16.0 );
    //bb_q(-size,-size, 0 );
    //}
    //if(p == 3) {
    //glTexCoord2f( 0.0, 0.0 );
    //glVertex3f( x+size, y+-size, z );

    //glTexCoord2f( 1.0/16.0, 0.0 );
    //glVertex3f( x+size, y+size, z);

    //glTexCoord2f( 1.0/16.0, 1.0/16.0 );
    //glVertex3f( x+-size, y+size, z );

    //glTexCoord2f( 0.0, 1.0/16.0 );
    //glVertex3f( x+-size,y+-size, z );
    //}
//glEnd();

    //glPopMatrix();
    //glDisable(GL_BLEND);
    //glDisable(GL_TEXTURE_2D);
    //glColor3ub(255,255,255);
    //return 0;
//}


//int _draw_particle2(int id, float size, float x, float y, float z) {
    ////int i, j;
    ////glLoadMatrixf(&a_cam); //only need to call this once per frame

    //float up[3] = {a_cam[0], a_cam[4], a_cam[8]};
    //float right[3] = {a_cam[1], a_cam[5], a_cam[9]};
    ////float pos[3] = {a_cam[3], a_cam[4], a_cam[5]}
    //float tx_min, tx_max, ty_min, ty_max;

    //glEnable(GL_TEXTURE_2D);
    //glEnable (GL_DEPTH_TEST);
    //glDepthMask(GL_FALSE);

    //glBindTexture( GL_TEXTURE_2D, particle_texture);
    //glEnable(GL_BLEND);
    ////glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    ////glBlendFunc (GL_ONE, GL_ONE);
    //glBlendFunc (GL_SRC_ALPHA, GL_ONE);


    //tx_min = (float)(id%16)* (1.0/16.0);
    //tx_max = tx_min + (1.0/16.0);

    //ty_min = (float)(id/16)* (1.0/16.0);
    //ty_max = ty_min + (1.0/16.0);

    ////printf("tmin, tmax= %f, %f \n", tx_min, tx_max);

//glBegin( GL_QUADS );

    //glTexCoord2f(tx_min,ty_max );
    //glVertex3f(x+(-right[0]-up[0]), y+(-right[1]-up[1]), z+(-right[2]-up[2]));  // Bottom left

    //glTexCoord2f(tx_min,ty_min );
    //glVertex3f(x+(up[0]-right[0]), y+(up[1]-right[1]), z+(up[2]-right[2]));  // Top left

    //glTexCoord2f(tx_max,ty_min);
    //glVertex3f(x+(up[0]+right[0]), y+(up[1]+right[1]), z+(up[2]+right[2]));  // Top right

    //glTexCoord2f(tx_max,ty_max );
    //glVertex3f(x+(right[0]-up[0]), y+(right[1]-up[1]), z+(right[2]-up[2]));  // Bottom right

//glEnd();

    //glDepthMask(GL_TRUE);
    //glDisable(GL_TEXTURE_2D);
    ////glDisable (GL_DEPTH_TEST);
    //glDisable(GL_BLEND);
    //glColor3ub(255,255,255);
        //return 0;

//}


struct Vec {
float x,y,z;
};

inline struct Vec init_Vec(float x, float y, float z) {
    struct Vec v; v.x=x;v.y=y;v.z=z; return v;
}

inline void normalize(struct Vec* v) {
float l = sqrt(v->x*v->x + v->y*v->y + v->z*v->z);
v->x /= l; v->y /=l; v->z /=l;
}

struct Vec cross(struct Vec v1, struct Vec v2) {
    struct Vec v0;
    v0.x = v1.y*v2.z - v1.z*v2.y;
    v0.y = v1.z*v2.x - v1.x*v2.z;
    v0.z = v1.x*v2.y - v1.y*v2.x;
    return v0;
}

void dot(struct Vec v1, struct Vec v2) {
    float d;
    d = v1.x*v2.x + v1.y*v2.y + v1.z*+v2.z;
    //printf("dot= %f \n", d);
}

void calc_len(struct Vec *v) {
    float l;
    l = sqrt(v->x*v->x + v->y*v->y + v->z*v->z);
    v->x /= l; v->y /=l; v->z /=l;
    //printf("l= %fs \n", l);
    }

void draw(struct Vec v0, struct Vec v1, int r, int g, int b) {

glColor3ub((unsigned char)r,(unsigned char)g,(unsigned char)b);
glBegin(GL_LINES);
glVertex3f(v0.x, v0.y, v0.z);
glVertex3f(v0.x+v1.x, v0.y+v1.y, v0.z+v1.z);
glEnd();
//glColor3ub(255,255,255);
}

/*
inline void draw2(float a_cam, float b, struct v0, struct Vec v1, struct Vec v2) {

    glVertex3f(v0.x+a_cam*v1.x+b*v2.x, v0.y+a_cam*v1.y+b*v2.y, v0.z+a_cam*v1.z+b*v2.z);
    glVertex3f(v0.x+a_cam*v1.x+b*v2.x, v0.y+a_cam*v1.y+b*v2.y, v0.z+a_cam*v1.z+b*v2.z);
    glVertex3f(v0.x+a_cam*v1.x+b*v2.x, v0.y+a_cam*v1.y+b*v2.y, v0.z+a_cam*v1.z+b*v2.z);
}
*/

//#define pi 3.141519
//int _planar_laser(float x0, float y0, float z0, float x1, float y1, float z1) {
    ////printf("Laser draw start \n");

    //if(c_cam == NULL) {
        //printf("particle_functions _planar_laser camera is null \n");
        //return 0;
    //}
    //struct Vec pos = init_Vec(c_cam->x, c_cam->y, c_cam->z);

    ////printf("pos= %f, %f, %f \n", c_cam->x, c_cam->y, c_cam->z);

    //struct Vec po;
    //po.x =  pos.x - (x0+y1)/2;
    //po.y =  pos.y - (y0+y1)/2;
    //po.z =  pos.z -(z0+z1)/2;
    //normalize(&po);

    //int i =0 ;
    ////normalize(&pos);
    //struct Vec up = init_Vec(x1-x0, y1-y0, z1-z0);
    //normalize(&up);
    //calc_len(&up);
    //struct Vec left = cross(po, up);
    //normalize(&left);
    //calc_len(&left);
    //struct Vec right = cross(left, up);
    //calc_len(&right);
    ////printf("\n");

    //struct Vec vu;
    //vu = init_Vec((x0+y1)/2, (y0+y1)/2, (z0+z1)/2);
    //draw(vu, up, 255,0,0);
    //draw(vu, left, 0,255,0);
    //draw(vu, right, 0,0,255);

    //float a_cam, b;
    ////struct Vec bot, top;
    ////bot = init_Vec(x0,y0,z0);
    ////top = init_Vec(x1,y1,z1);

    ////struct Vec v1 = left;
    ////struct Vec v2 = right;

    //dot(up, left);
    //dot(up, right);
    //dot(left, right);

    //struct Vec v1;

    //while(i< 8) {
    //a_cam = sin(i*pi/8);
    //b = cos(i*pi/8);
    //i++;

    //v1.x = a_cam*left.x + b*right.x;
    //v1.y = a_cam*left.y + b*right.y;
    //v1.z = a_cam*left.z + b*right.z;

    //glColor3ub(255,255,255);
    //glDepthMask(GL_FALSE);
    //glEnable(GL_BLEND);
    ////glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    ////glBlendFunc (GL_ONE, GL_ONE);
    //glBlendFunc (GL_SRC_ALPHA, GL_ONE);

    //glColor3ub((unsigned char)0,(unsigned char)0,(unsigned char)20);
    //glBegin( GL_QUADS );
        //glVertex3f(x1 -a_cam*v1.x, y1 -a_cam*v1.y, z1 -a_cam*v1.z);
        //glVertex3f(x1 +a_cam*v1.x, y1 +a_cam*v1.y, z1 +a_cam*v1.z);
        //glVertex3f(x0 +a_cam*v1.x, y0 +a_cam*v1.y, z0 +a_cam*v1.z);
        //glVertex3f(x0 -a_cam*v1.x, y0 -a_cam*v1.y, z0 -a_cam*v1.z);
    //glEnd();

    //}
    //glDisable(GL_TEXTURE_2D);
    //glDisable (GL_DEPTH_TEST);
    //glDisable(GL_BLEND);
    //glDepthMask(GL_TRUE);
    //return 0;
//}

//struct Vec ta[1024*2]; //temp array

//int _planar_laser2(int density, float width, float x0, float y0, float z0, float x1, float y1, float z1) {
    //if(c_cam == NULL) {
    //printf("C");
    //return 0;
    //}

    //if(density > 1023) { density = 1023; }

    //float dx,dy,dz;
    //dx = (x1-x0)/density;
    //dy = (y1-y0)/density;
    //dz = (z1-z0)/density;
    ////sprite
    ////int id =255;
    //int id = 19;
    //int x,y;
    //x = id %16;
    //y = id /16;
    //float tx_min, tx_max, ty_min, ty_max;
    //float epi = 0.0625/8;//0.0625 / 16; //epilon border
    //tx_min = x* (1.0/16.0);
    //tx_max = tx_min + (1.0/16.0);
    //ty_min = y* (1.0/16.0);
    //ty_max = ty_min + (1.0/16.0);
    //ty_min += epi; ty_max -= epi;

    //int i=0;
    //float bx,by,bz;
    ////struct Vec* vl, vr;
    //struct Vec pos = init_Vec(c_cam->x, c_cam->y, c_cam->z); //camera position
    //struct Vec up = init_Vec(x1-x0, y1-y0, z1-z0); //up position
    //normalize(&up);
    //struct Vec po; //camera to point vector

    //struct Vec left;
    ////struct Vec up2;
    ////struct Vec last;

    //bx = i*dx + x0;
    //by = i*dy + y0;
    //bz = i*dz + z0;

    //po.x =  pos.x - bx;
    //po.y =  pos.y - by;
    //po.z =  pos.z - bz;
    //normalize(&po);

    //left = cross(po, up);

    //for(i=0; i<=density; i++) {
        //bx = i*dx + x0;
        //by = i*dy + y0;
        //bz = i*dz + z0;

        //po.x =  pos.x - bx;
        //po.y =  pos.y - by;
        //po.z =  pos.z - bz;
        //normalize(&po);

        //left = cross(po, up);
        ////up2 = cross(left, up);

///*
        //bx - width*left.x
        //by - width*left.y
        //bz - width*left.z

        //bx + width*left.x
        //by + width*left.y
        //bz + width*left.z
//*/
        //ta[2*i+0] = init_Vec(bx - width*left.x, by - width*left.y, bz - width*left.z);
        //ta[2*i+1] = init_Vec(bx + width*left.x, by + width*left.y, bz + width*left.z);
    //}

    //glEnable(GL_TEXTURE_2D);
    //glEnable (GL_DEPTH_TEST);
    //glDepthMask(GL_FALSE);
    //glColor3ub(255,255,255);

    //glBindTexture( GL_TEXTURE_2D, particle_texture );
    //glEnable(GL_BLEND);
    ////glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    ////glBlendFunc (GL_ONE, GL_ONE);
    //glBlendFunc (GL_SRC_ALPHA, GL_ONE);

    //int bl, tl, tr, br;

    //glBegin( GL_QUADS );
    //for(i=0; i < density; i++) {
        //bl = 2*i+0;
        //tl = 2*(i+1)+0;
        //tr = 2*(i+1)+1;
        //br = 2*i+1;

        //glTexCoord2f(tx_min,ty_max );
        //glVertex3f(ta[bl].x,ta[bl].y,ta[bl].z);  // Bottom left
        //glTexCoord2f(tx_min,ty_min );
        //glVertex3f(ta[tl].x,ta[tl].y,ta[tl].z);  // Top left
        //glTexCoord2f(tx_max,ty_min);
        //glVertex3f(ta[tr].x,ta[tr].y,ta[tr].z);  // Top right
        //glTexCoord2f(tx_max,ty_max );
        //glVertex3f(ta[br].x,ta[br].y,ta[br].z);  // Bottom right

    //}

    //glEnd();

    //glDepthMask(GL_TRUE);
    //glDisable(GL_TEXTURE_2D);
    //glDisable (GL_DEPTH_TEST);
    //glDisable(GL_BLEND);
    ////glColor3ub(255,255,255);
    //return 0;
//}


void init_camera_c() {
    _init_particle_functions();
}

