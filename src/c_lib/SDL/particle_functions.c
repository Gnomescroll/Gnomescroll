#include "particle_functions.h"

#include <camera/camera.hpp>

static GLuint shader_program_1;

int setShaders() {
    char *vs,*gs,*fs;
    GLuint v,g,f;

    v = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
    g = glCreateShaderObjectARB(GL_GEOMETRY_SHADER_ARB);
    f = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

    vs = textFileRead((char*) "./media/shaders/test1.vert");
    gs = textFileRead((char*) "./media/shaders/test1.geom");
    fs = textFileRead((char*) "./media/shaders/test1.frag");

    const char * vv = vs;
    const char * ff = fs;
    const char * gg = gs;

    glShaderSourceARB(v, 1, &vv, NULL);
    glShaderSourceARB(f, 1, &ff, NULL);
    glShaderSourceARB(f, 1, &gg, NULL);

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

    return 0;
}

void __inline bb_q(float x, float y, float z) {

    float x_,y_,z_;

    x_ = model_view_matrix[0]*x+model_view_matrix[4]*y+model_view_matrix[8]*z;
    y_ = model_view_matrix[1]*x+model_view_matrix[5]*y+model_view_matrix[9]*z;
    z_ = model_view_matrix[2]*x+model_view_matrix[6]*y+model_view_matrix[10]*z;

    glVertex3f( x_, y_, z_ );
}

void draw(Vec3 v0, Vec3 v1, int r, int g, int b)
{

    glColor3ub((unsigned char)r,(unsigned char)g,(unsigned char)b);
    glBegin(GL_LINES);
    glVertex3f(v0.x, v0.y, v0.z);
    glVertex3f(v0.x+v1.x, v0.y+v1.y, v0.z+v1.z);
    glEnd();
}

