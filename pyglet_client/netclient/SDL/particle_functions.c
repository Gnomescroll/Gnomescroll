

#include "particle_functions.h"

SDL_Surface *surface;
GLuint texture;

float a[16];

_init_particle_functions() {

set_model_view_matrix(&a);

surface=IMG_Load("./texture/particles_01.png");
if(!surface) {
    printf("_init_particle_functions, IMG_Load: %s \n", IMG_GetError());
    return 0;
}
if(surface->format->BytesPerPixel != 4) {printf("IMG_Load: image is missing alpha channel \n"); return 0;}

glEnable(GL_TEXTURE_2D);
glGenTextures( 1, &texture );
glBindTexture( GL_TEXTURE_2D, texture );
glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR ); ///tweak?
glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR ); ///tweak?

glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

glTexImage2D(GL_TEXTURE_2D, 0, 4, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels );
glDisable(GL_TEXTURE_2D);

printf("particle_texture_id= %i \n", texture);
}

void inline bb_q(float x, float y, float z) {

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
    glBindTexture( GL_TEXTURE_2D, texture );
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glColor3ub(255, 255, 255);

    int p = 3;
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
}


int _draw_particle2(int id, float size, float x, float y, float z) {
    //int i, j;
    //glLoadMatrixf(&a); //only need to call this once per frame

    float up[3] = {a[0], a[4], a[8]};
    float right[3] = {a[1], a[5], a[9]};

    glEnable(GL_TEXTURE_2D);
    glEnable (GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    glBindTexture( GL_TEXTURE_2D, texture );
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glBlendFunc (GL_ONE, GL_ONE);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE);

    float tx_min, tx_max, ty_min, ty_max;
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
    glDisable (GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}
