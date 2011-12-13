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

