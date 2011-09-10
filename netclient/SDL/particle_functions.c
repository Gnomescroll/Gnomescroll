

#include "particle_functions.h"

SDL_Surface *surface;
GLuint particle_texture;

struct Camera* c;

float a[16];

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
}

_init_particle_functions() {
/*
    printf("set shaders\n");
    setShaders();
    printf("shaders set\n");
*/
//while(1) {
//    printf("t\n");
//}

c = _get_camera();
/*
    if(c != NULL) {
        printf("Particle Function Camera: Camera not null \n");
    } else {
        printf("Particle Function Camera: Error: camera is null \n");
    }
*/
set_model_view_matrix(&a);

surface=IMG_Load("./media/texture/particles_01.png");
if(!surface) {
    printf("_init_particle_functions, IMG_Load: %s \n", IMG_GetError());
    return 0;
}
if(surface->format->BytesPerPixel != 4) {printf("IMG_Load: image is missing alpha channel \n"); return 0;}

glEnable(GL_TEXTURE_2D);
glGenTextures( 1, &particle_texture );
glBindTexture( GL_TEXTURE_2D, particle_texture );
glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR ); ///tweak?
glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR ); ///tweak?

glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

glTexImage2D(GL_TEXTURE_2D, 0, 4, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels );
glDisable(GL_TEXTURE_2D);

printf("particle_texture_id= %i \n", particle_texture);
}

void __inline bb_q(float x, float y, float z) {

float x_,y_,z_;

x_ = a[0]*x+a[4]*y+a[8]*z;
y_ = a[1]*x+a[5]*y+a[9]*z;
y_ = a[2]*x+a[6]*y+a[10]*z;

/*
x_ = a[0]*x+a[1]*y+a[2]*z;
y_ = a[4]*x+a[5]*y+a[6]*z;
y_ = a[8]*x+a[9]*y+a[10]*z;
*/
glVertex3f( x_, y_, z_ );
}

int _draw_particle(int id, float size, float x, float y, float z) {
    int i, j;
    int p = 3; //deprecate this
/*
position_camera();
glGetDoublev(GL_MODELVIEW_MATRIX, &mv_matrix);
rot_matrix=mv_matrix.get_sub(0, 0, 3, 3).inverse();
billboard_alligned_quad_array=quad_array.mult_matrix(rot_matix);
for(i=0; i<billboard_count; i++)
{
    glPushMatrix();
    glTranslate(billboard[i].position);
    bilboard[i].draw_using(billboard_alligned_quad_array);
    glPopMatrix()
}
*/
    glGetFloatv(GL_MODELVIEW_MATRIX, &a);
    glPushMatrix();

    //glTranslatef(x,y,z);
/*
    a[0]=1;a[5]=1;a[10]=1;
    a[1]=0;a[2]=0;a[6]=0;
    a[4]=0;a[8]=0;a[9]=0;
*/

for( i=0; i<3; i++ )
    for( j=0; j<3; j++ ) {
        if ( i==j )
            a[i*4+j] = 1.0;
        else
            a[i*4+j] = 0.0;
    }
    //GL_MODELVIEW_MATRIX
    glLoadMatrixf(&a);

    //glTranslatef(x,y,z);
    //struct Camera *c;
    //float x0,y0,z0
    //c = _get_camera();

    glEnable(GL_TEXTURE_2D);
    glBindTexture( GL_TEXTURE_2D, particle_texture );
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glColor3ub(255, 255, 255);

glBegin( GL_QUADS );
    if(p == 1) {
    glTexCoord2f( 0.0, 0.0 );
    glVertex3f( size, -size, 0 );

    glTexCoord2f( 1.0/16.0, 0.0 );
    glVertex3f( size, size, 0 );

    glTexCoord2f( 1.0/16.0, 1.0/16.0 );
    glVertex3f( -size, size, 0 );

    glTexCoord2f( 0.0, 1.0/16.0 );
    glVertex3f( -size,-size, 0 );
    }
    if(p == 2) {
    glTexCoord2f( 0.0, 0.0 );
    bb_q( size, -size, 0 );

    glTexCoord2f( 1.0/16.0, 0.0 );
    bb_q(size, size, 0 );

    glTexCoord2f( 1.0/16.0, 1.0/16.0 );
    bb_q(-size, size, 0 );

    glTexCoord2f( 0.0, 1.0/16.0 );
    bb_q(-size,-size, 0 );
    }
    if(p == 3) {
    glTexCoord2f( 0.0, 0.0 );
    glVertex3f( x+size, y+-size, z );

    glTexCoord2f( 1.0/16.0, 0.0 );
    glVertex3f( x+size, y+size, z);

    glTexCoord2f( 1.0/16.0, 1.0/16.0 );
    glVertex3f( x+-size, y+size, z );

    glTexCoord2f( 0.0, 1.0/16.0 );
    glVertex3f( x+-size,y+-size, z );
    }
glEnd();

    glPopMatrix();
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
    glColor3ub(255,255,255);
}


int _draw_particle2(int id, float size, float x, float y, float z) {
    //int i, j;
    //glLoadMatrixf(&a); //only need to call this once per frame

    float up[3] = {a[0], a[4], a[8]};
    float right[3] = {a[1], a[5], a[9]};
    //float pos[3] = {a[3], a[4], a[5]}
    float tx_min, tx_max, ty_min, ty_max;

    glEnable(GL_TEXTURE_2D);
    glEnable (GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    glBindTexture( GL_TEXTURE_2D, particle_texture);
    glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glBlendFunc (GL_ONE, GL_ONE);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE);


    tx_min = (float)(id%16)* (1.0/16.0);
    tx_max = tx_min + (1.0/16.0);

    ty_min = (float)(id/16)* (1.0/16.0);
    ty_max = ty_min + (1.0/16.0);

    //printf("tmin, tmax= %f, %f \n", tx_min, tx_max);

glBegin( GL_QUADS );

    glTexCoord2f(tx_min,ty_max );
    glVertex3f(x+(-right[0]-up[0]), y+(-right[1]-up[1]), z+(-right[2]-up[2]));  // Bottom left

    glTexCoord2f(tx_min,ty_min );
    glVertex3f(x+(up[0]-right[0]), y+(up[1]-right[1]), z+(up[2]-right[2]));  // Top left

    glTexCoord2f(tx_max,ty_min);
    glVertex3f(x+(up[0]+right[0]), y+(up[1]+right[1]), z+(up[2]+right[2]));  // Top right

    glTexCoord2f(tx_max,ty_max );
    glVertex3f(x+(right[0]-up[0]), y+(right[1]-up[1]), z+(right[2]-up[2]));  // Bottom right

glEnd();

    glDepthMask(GL_TRUE);
    glDisable(GL_TEXTURE_2D);
    //glDisable (GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glColor3ub(255,255,255);
}


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
glColor3ub(255,255,255);
}

/*
inline void draw2(float a, float b, struct v0, struct Vec v1, struct Vec v2) {

    glVertex3f(v0.x+a*v1.x+b*v2.x, v0.y+a*v1.y+b*v2.y, v0.z+a*v1.z+b*v2.z);
    glVertex3f(v0.x+a*v1.x+b*v2.x, v0.y+a*v1.y+b*v2.y, v0.z+a*v1.z+b*v2.z);
    glVertex3f(v0.x+a*v1.x+b*v2.x, v0.y+a*v1.y+b*v2.y, v0.z+a*v1.z+b*v2.z);
}
*/

#define pi 3.141519
int _planar_laser(float x0, float y0, float z0, float x1, float y1, float z1) {
    //printf("Laser draw start \n");

    if(c == NULL) {
        printf("particle_functions _planar_laser camera is null \n");
        return 0;
    }
    struct Vec pos = init_Vec(c->x, c->y, c->z);

    //printf("pos= %f, %f, %f \n", c->x, c->y, c->z);

    struct Vec po;
    po.x =  pos.x - (x0+y1)/2;
    po.y =  pos.y - (y0+y1)/2;
    po.z =  pos.z -(z0+z1)/2;
    normalize(&po);

    int i =0 ;
    //normalize(&pos);
    struct Vec up = init_Vec(x1-x0, y1-y0, z1-z0);
    normalize(&up);
    calc_len(&up);
    struct Vec left = cross(po, up);
    normalize(&left);
    calc_len(&left);
    struct Vec right = cross(left, up);
    calc_len(&right);
    //printf("\n");

    struct Vec vu;
    vu = init_Vec((x0+y1)/2, (y0+y1)/2, (z0+z1)/2);
    draw(vu, up, 255,0,0);
    draw(vu, left, 0,255,0);
    draw(vu, right, 0,0,255);

    float a, b;
    //struct Vec bot, top;
    //bot = init_Vec(x0,y0,z0);
    //top = init_Vec(x1,y1,z1);

    //struct Vec v1 = left;
    //struct Vec v2 = right;

    dot(up, left);
    dot(up, right);
    dot(left, right);

    struct Vec v1;

    while(i< 8) {
    a = sin(i*pi/8);
    b = cos(i*pi/8);
    i++;

    v1.x = a*left.x + b*right.x;
    v1.y = a*left.y + b*right.y;
    v1.z = a*left.z + b*right.z;

    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glBlendFunc (GL_ONE, GL_ONE);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE);

    glColor3ub((unsigned char)0,(unsigned char)0,(unsigned char)20);
    glBegin( GL_QUADS );
        glVertex3f(x1 -a*v1.x, y1 -a*v1.y, z1 -a*v1.z);
        glVertex3f(x1 +a*v1.x, y1 +a*v1.y, z1 +a*v1.z);
        glVertex3f(x0 +a*v1.x, y0 +a*v1.y, z0 +a*v1.z);
        glVertex3f(x0 -a*v1.x, y0 -a*v1.y, z0 -a*v1.z);
    glEnd();

    }
    glDisable(GL_TEXTURE_2D);
    //glDisable (GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
    return 0;
}

struct Vec ta[1024*2]; //temp array

int _planar_laser2(int density, float width, float x0, float y0, float z0, float x1, float y1, float z1) {
    if(c == NULL) {
    printf("C");
    return 0;
    }

    if(density > 1023) { density = 1023; }

    float dx,dy,dz;
    dx = (x1-x0)/density;
    dy = (y1-y0)/density;
    dz = (z1-z0)/density;
    //sprite
    //int id =255;
    int id = 19;
    int x,y;
    x = id %16;
    y = id /16;
    float tx_min, tx_max, ty_min, ty_max;
    float epi = 0.0625/8;//0.0625 / 16; //epilon border
    tx_min = x* (1.0/16.0);
    tx_max = tx_min + (1.0/16.0);
    ty_min = y* (1.0/16.0);
    ty_max = ty_min + (1.0/16.0);
    ty_min += epi; ty_max -= epi;

    int i;
    float bx,by,bz;
    //struct Vec* vl, vr;
    struct Vec pos = init_Vec(c->x, c->y, c->z); //camera position
    struct Vec up = init_Vec(x1-x0, y1-y0, z1-z0); //up position
    normalize(&up);
    struct Vec po; //camera to point vector

    struct Vec left;
    //struct Vec up2;
    //struct Vec last;

    bx = i*dx + x0;
    by = i*dy + y0;
    bz = i*dz + z0;

    po.x =  pos.x - bx;
    po.y =  pos.y - by;
    po.z =  pos.z - bz;
    normalize(&po);

    left = cross(po, up);

    for(i=0; i<=density; i++) {
        bx = i*dx + x0;
        by = i*dy + y0;
        bz = i*dz + z0;

        po.x =  pos.x - bx;
        po.y =  pos.y - by;
        po.z =  pos.z - bz;
        normalize(&po);

        left = cross(po, up);
        //up2 = cross(left, up);

/*
        bx - width*left.x
        by - width*left.y
        bz - width*left.z

        bx + width*left.x
        by + width*left.y
        bz + width*left.z
*/
        ta[2*i+0] = init_Vec(bx - width*left.x, by - width*left.y, bz - width*left.z);
        ta[2*i+1] = init_Vec(bx + width*left.x, by + width*left.y, bz + width*left.z);
    }

    glEnable(GL_TEXTURE_2D);
    glEnable (GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    glBindTexture( GL_TEXTURE_2D, particle_texture );
    glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glBlendFunc (GL_ONE, GL_ONE);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE);

    int bl, tl, tr, br;

    glBegin( GL_QUADS );
    for(i=0; i < density; i++) {
        bl = 2*i+0;
        tl = 2*(i+1)+0;
        tr = 2*(i+1)+1;
        br = 2*i+1;

        glTexCoord2f(tx_min,ty_max );
        glVertex3f(ta[bl].x,ta[bl].y,ta[bl].z);  // Bottom left
        glTexCoord2f(tx_min,ty_min );
        glVertex3f(ta[tl].x,ta[tl].y,ta[tl].z);  // Top left
        glTexCoord2f(tx_max,ty_min);
        glVertex3f(ta[tr].x,ta[tr].y,ta[tr].z);  // Top right
        glTexCoord2f(tx_max,ty_max );
        glVertex3f(ta[br].x,ta[br].y,ta[br].z);  // Bottom right

    }

    glEnd();

    glDepthMask(GL_TRUE);
    glDisable(GL_TEXTURE_2D);
    glDisable (GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glColor3ub(255,255,255);
}
