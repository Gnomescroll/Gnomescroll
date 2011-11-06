#include "particle_functions.h"

static SDL_Surface *particle_surface;
static GLuint particle_texture;

static struct Camera* particle_c;

static float a[16];

static GLuint shader_program_1;

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

    return 0;
}

int _init_particle_functions() {

    particle_c = _get_camera();
    set_model_view_matrix(a);

    particle_surface=IMG_Load("./media/texture/particles_01.png");
    if(!particle_surface) {
        printf("_init_particle_functions, IMG_Load: %s \n", IMG_GetError());
        return 1;
    }
    if(particle_surface->format->BytesPerPixel != 4) {printf("IMG_Load: image is missing alpha channel \n"); return 0;}

    glEnable(GL_TEXTURE_2D);
    glGenTextures( 1, &particle_texture );
    glBindTexture( GL_TEXTURE_2D, particle_texture );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR ); ///tweak?
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR ); ///tweak?

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, 4, particle_surface->w, particle_surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, particle_surface->pixels );
    glDisable(GL_TEXTURE_2D);

    printf("particle_texture_id= %i \n", particle_texture);
    return 0;
}

void __inline bb_q(float x, float y, float z) {

    float x_,y_,z_;

    x_ = a[0]*x+a[4]*y+a[8]*z;
    y_ = a[1]*x+a[5]*y+a[9]*z;
    z_ = a[2]*x+a[6]*y+a[10]*z;

    glVertex3f( x_, y_, z_ );
}

int _draw_particle(int id, float size, float x, float y, float z) {
    int i, j;
    int p = 3; //deprecate this

    glGetFloatv(GL_MODELVIEW_MATRIX, a);
    glPushMatrix();

    for(i=0; i<3; i++) {
        for(j=0; j<3; j++) {
            if (i==j) {
                a[i*4+j] = 1.0;
            } else {
                a[i*4+j] = 0.0;
            }
        }
    }

    glLoadMatrixf(a);

    glEnable(GL_TEXTURE_2D);
    glBindTexture( GL_TEXTURE_2D, particle_texture );

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
    return 0;
}

int _draw_particle2(int id, float size, float x, float y, float z) {

    float up[3] = {a[0], a[4], a[8]};
    float right[3] = {a[1], a[5], a[9]};

    float tx_min, tx_max, ty_min, ty_max;

    glEnable(GL_TEXTURE_2D);
    glEnable (GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    glBindTexture( GL_TEXTURE_2D, particle_texture);
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE);

    tx_min = (float)(id%16)* (1.0/16.0);
    tx_max = tx_min + (1.0/16.0);

    ty_min = (float)(id/16)* (1.0/16.0);
    ty_max = ty_min + (1.0/16.0);

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
    glDisable(GL_BLEND);
    glColor3ub(255,255,255);
    return 0;
}

void draw(struct Vector v0, struct Vector v1, int r, int g, int b) {

    glColor3ub((unsigned char)r,(unsigned char)g,(unsigned char)b);
    glBegin(GL_LINES);
    glVertex3f(v0.x, v0.y, v0.z);
    glVertex3f(v0.x+v1.x, v0.y+v1.y, v0.z+v1.z);
    glEnd();
}

int _planar_laser(float x0, float y0, float z0, float x1, float y1, float z1) {

    if(particle_c == NULL) {
        printf("particle_functions _planar_laser camera is null \n");
        return 0;
    }
    struct Vector pos = Vector_init(particle_c->x, particle_c->y, particle_c->z);

    struct Vector po;
    po.x =  pos.x - (x0+y1)/2;
    po.y =  pos.y - (y0+y1)/2;
    po.z =  pos.z -(z0+z1)/2;
    normalize_vector(&po);

    int i =0 ;
    struct Vector up = Vector_init(x1-x0, y1-y0, z1-z0);
    normalize_vector(&up);
    vector_length(&up);
    struct Vector left = vector_cross(po, up);
    normalize_vector(&left);
    vector_length(&left);
    struct Vector right = vector_cross(left, up);
    vector_length(&right);

    struct Vector vu;
    vu = Vector_init((x0+y1)/2, (y0+y1)/2, (z0+z1)/2);
    draw(vu, up, 255,0,0);
    draw(vu, left, 0,255,0);
    draw(vu, right, 0,0,255);

    float a, b;

    struct Vector v1;

    while(i< 8) {
    a = sin(i*PI/8);
    b = cos(i*PI/8);
    i++;

    v1.x = a*left.x + b*right.x;
    v1.y = a*left.y + b*right.y;
    v1.z = a*left.z + b*right.z;

    glColor3ub(255,255,255);
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
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
    glDisable (GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
    return 0;
}

static struct Vector ta[1024*2]; //temp array

int _planar_laser2(int density, float width, float x0, float y0, float z0, float x1, float y1, float z1) {
    if(particle_c == NULL) {
        printf("particle_c");
        return 0;
    }

    density = (density > 1023) ? 1023 : density;

    float dx,dy,dz;
    dx = (x1-x0)/density;
    dy = (y1-y0)/density;
    dz = (z1-z0)/density;

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

    int i=0;
    float bx,by,bz;

    struct Vector pos = Vector_init(particle_c->x, particle_c->y, particle_c->z); //camera position
    struct Vector up = Vector_init(x1-x0, y1-y0, z1-z0); //up position
    normalize_vector(&up);
    struct Vector po; //camera to point vector

    struct Vector left;

    bx = i*dx + x0;
    by = i*dy + y0;
    bz = i*dz + z0;

    po.x =  pos.x - bx;
    po.y =  pos.y - by;
    po.z =  pos.z - bz;
    normalize_vector(&po);

    left = vector_cross(po, up);

    for(i=0; i<=density; i++) {
        bx = i*dx + x0;
        by = i*dy + y0;
        bz = i*dz + z0;

        po.x =  pos.x - bx;
        po.y =  pos.y - by;
        po.z =  pos.z - bz;
        normalize_vector(&po);

        left = vector_cross(po, up);

        ta[2*i+0] = Vector_init(bx - width*left.x, by - width*left.y, bz - width*left.z);
        ta[2*i+1] = Vector_init(bx + width*left.x, by + width*left.y, bz + width*left.z);
    }

    glEnable(GL_TEXTURE_2D);
    glEnable (GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glColor3ub(255,255,255);

    glBindTexture( GL_TEXTURE_2D, particle_texture );
    glEnable(GL_BLEND);
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
    return 0;
}
