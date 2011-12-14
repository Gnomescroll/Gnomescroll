#include "draw_functions.h"

#include "SDL_functions.h"

int _draw_line(int r, int g,int b, float x0, float y0, float z0, float x1, float y1, float z1) {

    glColor3ub((unsigned char)r,(unsigned char)g,(unsigned char)b);
    glBegin(GL_LINES);
    glVertex3f(x0,y0,z0); // origin of the line
    glVertex3f(x1,y1,z1); // ending point of the line
    glEnd();

    return 0;
}

int _draw_point(int r, int g,int b, float x0, float y0, float z0) {
    glColor3ub((unsigned char)r,(unsigned char)g,(unsigned char)b);
    glBegin(GL_POINTS);
    glVertex3f(x0,y0,z0); // point
    glEnd();
    return 0;
}

int _draw_rect(int r, int g, int b, float x, float y, float w, float h) {

    float x1, y1, x2, y2;
    x1 = x;
    y1 = y;
    x2 = x + w;
    y2 = y + h;

    glColor3ub((unsigned char)r,(unsigned char)g,(unsigned char)b);
    glBegin(GL_QUADS);
    glVertex2f(x1, y1);
    glVertex2f(x2, y1);
    glVertex2f(x2, y2);
    glVertex2f(x1, y2);
    glEnd();
    return 0;
}

int _draw_border_rect(int r, int g, int b, float x, float y, float w, float h) {

    float x1, y1, x2, y2;
    x1 = x;
    y1 = y;
    x2 = x + w;
    y2 = y + h;

    glColor3ub((unsigned char)r,(unsigned char)g,(unsigned char)b);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x1, y1);
    glVertex2f(x2, y1);
    glVertex2f(x2, y2);
    glVertex2f(x1, y2);
    glEnd();
    return 0;
}

int _blit_sprite(int texture, float x0, float y0, float x1, float y1, float z) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture( GL_TEXTURE_2D, texture );
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor3ub(255, 255, 255);

    glBegin( GL_QUADS );
    glTexCoord2i( 0, 0 );
    glVertex3f( x0, y0, z );

    glTexCoord2i( 1, 0 );
    glVertex3f( x1, y0, z );

    glTexCoord2i( 1, 1 );
    glVertex3f( x1, y1, z );

    glTexCoord2i( 0, 1 );
    glVertex3f( x0, y1, z );
    glEnd();

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
    return 0;
}

// y coordinates start along the bottom
void draw_bound_texture(float x, float y, float w, float h, float z) {

    glBegin(GL_QUADS);

    glTexCoord2f(0.0,0.0);
    glVertex3f(x, y, z);  // Top left

    glTexCoord2f(1.0,0.0);
    glVertex3f(x+w, y, z);  // Top right

    glTexCoord2f(1.0,1.0);
    glVertex3i(x+w, y+h, z);  // Bottom right

    glTexCoord2f(0.0,1.0);
    glVertex3i(x, y+h, z);  // Bottom left
    glEnd();
}

void rotate_point(float x, float y, float theta, float *x1, float *y1) {
    *x1 = x*cos(theta) - y*sin(theta);
    *y1 = y*sin(theta) - y*cos(theta);
}

// y coordinates start along the bottom
void draw_bound_texture_rotated(float x, float y, float w, float h, float z, float theta) {
//printf("theta: %0.2f\n", theta);
    //const float pi = 3.14159;
    //theta += 1.0f;
    //theta *= pi;
    //float cx = x + w/2;
    //float cy = y + h/2; //center

    //x -= cx;
    //y -= cy;

    //float x1,y1,x2,y2;

    //x1 = x - cx;
    //y1 = y - cy;
    //x1 = -(x - cx);
    //y1 = -(y - cy);
    //x2 = x + w - cx;
    //y2 = y + h - cy;

    //x1 = x1*cos(theta) - y1*sin(theta);
    //y1 = x1*sin(theta) + y1*cos(theta);
    //x2 = x2*cos(theta) - y2*sin(theta);
    //y2 = x2*sin(theta) + y2*cos(theta);

    //x1 *= -1;
    //y1 *= -1;
    //x2 *= -1;
    //y2 *= -1;

    //x1 += cx;
    //y1 += cy;
    //x2 += cx;
    //y2 += cy;

    float rx=0.0f,*_rx=&rx,
          ry=0.0f,*_ry=&ry;

    glBegin(GL_QUADS);

    glTexCoord2f(0.0,1.0);
    rotate_point(x, y, theta, _rx, _ry);
    glVertex3f(rx, ry, z);  // Top left

    glTexCoord2f(1.0,1.0);
    rotate_point(x+w, y, theta, _rx, _ry);
    glVertex3f(rx, ry, z);  // Top right

    glTexCoord2f(1.0,0.0);
    rotate_point(x+w, y+h, theta, _rx, _ry);
    glVertex3i(rx, ry, z);  // Bottom right

    glTexCoord2f(0.0,0.0);
    rotate_point(x, y+h, theta, _rx, _ry);
    glVertex3i(rx, ry, z);  // Bottom left
    glEnd();
}

void draw_bound_texture_sprite(float x, float y, float w, float h, float z, float sx, float sy, float sw, float sh, float ssw, float ssh) {

    sx /= ssw;
    sy /= ssh;
    sw /= ssw;
    sh /= ssh;

    glBegin(GL_QUADS);

    glTexCoord2f(sx, sy+sh);
        glVertex3f(x,y,z);

    glTexCoord2f(sx+sw, sy+sh);
        glVertex3f(x+w, y,z);
        
    glTexCoord2f(sx+sw, sy);
        glVertex3f(x+w, y+h, z);

    glTexCoord2f(sx, sy);
        glVertex3f(x, y+h, z);

    glEnd();
}

