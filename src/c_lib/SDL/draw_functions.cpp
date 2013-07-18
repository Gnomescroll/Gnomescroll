#include "draw_functions.hpp"

#include <stdio.h>

#include <common/compat_gl.h>
#include <SDL/SDL_functions.hpp>


void draw_line(const Color& color,
               float x0, float y0, float z0,
               float x1, float y1, float z1)
{
    color.set_gl();
    glBegin(GL_LINES);
    glVertex3f(x0,y0,z0);  // origin of the line
    glVertex3f(x1,y1,z1);  // ending point of the line
    glEnd();
}

void draw_line(const Color& color, const struct Vec3& a, const struct Vec3& b)
{
    draw_line(color, a.x, a.y, a.z, b.x, b.y, b.z);
}


void draw_point(const Color& color, float x, float y, float z)
{
    color.set_gl();
    glBegin(GL_POINTS);
    glVertex3f(x,y,z);  // single point
    glEnd();
}

void draw_rect(const Color& color, const Vec2& position, const Vec2& dimension)
{
    Vec2 p = position;
    Vec2 q = vec2_add(position, dimension);
    color.set_gl();
    glBegin(GL_QUADS);
    glVertex2f(p.x, p.y);
    glVertex2f(q.x, p.y);
    glVertex2f(q.x, q.y);
    glVertex2f(p.x, q.y);
    glEnd();
    glColor4ub(255, 255, 255, 255);
}

void draw_rect(const Color& color, float x, float y, float w, float h)
{
    draw_rect(color, vec2_init(x, y), vec2_init(w, h));
}

void draw_border_rect(const Color& color, float x, float y, float w, float h, int line_width)
{   // draws from top left corner
    draw_border_rect(color, vec2_init(x, y), vec2_init(w, h), line_width);
}

void draw_border_rect(const Color& color, const Vec2& position, const Vec2& dimension, float line_width)
{
    Vec2 p = position;
    Vec2 q = vec2_init(p.x + dimension.x, p.y + dimension.y);
    GLfloat _line_width;
    glGetFloatv(GL_LINE_WIDTH, &_line_width);
    glLineWidth(line_width);
    color.set_gl();
    glBegin(GL_LINE_LOOP);
    glVertex2f(p.x, p.y);
    glVertex2f(q.x, p.y);
    glVertex2f(q.x, q.y);
    glVertex2f(p.x, q.y);
    glEnd();
    glColor4ub(255, 255, 255, 255);
    glLineWidth(_line_width);
}

void draw_bound_texture(float x, float y, float w, float h, float z)
{   // y coordinates start along the bottom
    glBegin(GL_QUADS);

    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(x, y, z);  // Top left

    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(x+w, y, z);  // Top right

    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(x+w, y+h, z);  // Bottom right

    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(x, y+h, z);  // Bottom left
    glEnd();
}


void draw_bound_texture(float x, float y, float w, float h)
{   // y coordinates start along the bottom
    glBegin(GL_QUADS);

    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(x, y);  // Top left

    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(x+w, y);  // Top right

    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(x+w, y+h);  // Bottom right

    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(x, y+h);  // Bottom left
    glEnd();
}


void draw_bound_texture_rotated(float x, float y, float w, float h, float z, float theta)
{   // y coordinates start along the bottom
    theta *= kPI;
    float cx = x + w/2;
    float cy = y + h/2; //center

    x -= cx;
    y -= cy;

    float rx=0.0f;
    float ry=0.0f;

    glBegin(GL_QUADS);

    glTexCoord2f(0.0f,0.0f);
    rotate_point(x, y, theta, &rx, &ry);
    glVertex3f(rx+cx, ry+cy, z);  // Top left

    glTexCoord2f(0.0f,1.0f);
    rotate_point(x+w, y, theta, &rx, &ry);
    glVertex3f(rx+cx, ry+cy, z);  // Top right

    glTexCoord2f(1.0f,1.0f);
    rotate_point(x+w, y+h, theta, &rx, &ry);
    glVertex3f(rx+cx, ry+cy, z);  // Bottom right

    glTexCoord2f(1.0f,0.0f);
    rotate_point(x, y+h, theta, &rx, &ry);
    glVertex3f(rx+cx, ry+cy, z);  // Bottom left
    glEnd();
}


void draw_bound_texture_rotated(float x, float y, float w, float h,
                                float tx, float ty, float tw, float th,
                                float depth, float theta)
{
    theta *= kPI;
    float cx = x + w/2;
    float cy = y + h/2; //center

    x -= cx;
    y -= cy;

    float rx=0.0f;
    float ry=0.0f;

    glBegin(GL_QUADS);

    glTexCoord2f(tx,ty+th);
    rotate_point(x, y, theta, &rx, &ry);
    glVertex3f(rx+cx, ry+cy, depth);  // Top left

    glTexCoord2f(tx+tw, ty+th);
    rotate_point(x+w, y, theta, &rx, &ry);
    glVertex3f(rx+cx, ry+cy, depth);  // Top right

    glTexCoord2f(tx+tw, ty);
    rotate_point(x+w, y+h, theta, &rx, &ry);
    glVertex3f(rx+cx, ry+cy, depth);  // Bottom right

    glTexCoord2f(tx, ty);
    rotate_point(x, y+h, theta, &rx, &ry);
    glVertex3f(rx+cx, ry+cy, depth);  // Bottom left
    glEnd();
}


void draw_bound_texture_sprite(float x, float y, float w, float h, float z,
                               float sprite_x, float sprite_y,
                               float sprite_width, float sprite_height)
{   // draws from the bottom left
    glBegin(GL_QUADS);

    glTexCoord2f(sprite_x, sprite_y+sprite_height); // top left
        glVertex3f(x,y,z);  // bottom left

    glTexCoord2f(sprite_x+sprite_width, sprite_y+sprite_height);    // top right
        glVertex3f(x+w, y,z);   // bottom right

    glTexCoord2f(sprite_x+sprite_width, sprite_y);  // top
        glVertex3f(x+w, y+h, z);

    glTexCoord2f(sprite_x, sprite_y);
        glVertex3f(x, y+h, z);

    glEnd();
}


void draw_iso_cube(float x, float y, float scale, int side0, int side1, int side2)
{
    /*
    RGB scales:

    239
    160
    106

    255
    171
    113
    */

    //const float scale = 8.0;
    scale = scale / 2.0f;
    x += scale;
    y += scale;

    const float aa = sqrtf(3.0f)/2.0f;

    //const float b[8*3] =
    //{
        //0.0,0.0, -aa,0.5, 0.0,1.0, aa,0.5,
        //-aa,0.5, 0.0,0.0, 0.0,-1.0, -aa,-0.5,
        //0.0,0.0, aa,0.5, aa,-0.5, 0.0,-1.0
    //};
    static const float b[8*3] =
    {
        0.0f,0.0f, -aa,0.5f, 0.0f,1.0f, aa,0.5f,
        -aa,0.5f, 0.0f,0.0f, 0.0f,-1.0f, -aa,-0.5f,
        0.0f,0.0f, aa,0.5f, aa,-0.5f, 0.0f,-1.0f
    };

    float a[8*3];

    for (int i=0; i<24; i++)  a[i] = b[i]*scale;
    // flip upside down
    for (int i=1; i<24; i+=2) a[i] *= -1;

    int tex_id, i;
    float tx0,tx1;
    float ty0,ty1;

    const float h = 1.0f/16.0f;
    const float hh = 1.0f/512.0f;

    tex_id = side0;

    tx0 = (tex_id % 16)*h + hh;
    tx1 = (tex_id % 16)*h + h - hh;

    ty0 = (tex_id / 16)*h + hh;
    ty1 = (tex_id / 16)*h + h - hh;

    // top side
    i = 0;

    glColor3ub(255, 255,255);

    glTexCoord2f(tx0,ty0); // top left
    glVertex2f(x+a[i+2],y+a[i+3]);   // bottom right

    glTexCoord2f(tx0,ty1);    // top right
    glVertex2f(x+a[i+4],y+a[i+5]+1.0f);  // flipped

    glTexCoord2f(tx1,ty1);  // top
    glVertex2f(x+a[i+6],y+a[i+7]);

    glTexCoord2f(tx1,ty0);
    glVertex2f(x+a[i+0],y+a[i+1]);  // bottom left

    // left side

    // rotate all draw coordinates 90 degrees clockwise
    i = 8;

    tex_id = side1;

    tx0 = (tex_id%16)*h+hh;
    tx1 = (tex_id%16)*h+h-hh;

    ty0 = (tex_id/16)*h+hh;
    ty1 = (tex_id/16)*h+h-hh;

    glColor3ub(171, 171, 171);

    // correct rotation
    glTexCoord2f(tx0,ty0); // top left
    glVertex2f(x+a[i+0],y+a[i+1]);  // bottom left

    glTexCoord2f(tx0,ty1);    // top right
    glVertex2f(x+a[i+6],y+a[i+7]);

    glTexCoord2f(tx1,ty1);  // top
    glVertex2f(x+a[i+4],y+a[i+5]-1.0f);  // flipped

    glTexCoord2f(tx1,ty0);
    glVertex2f(x+a[i+2],y+a[i+3]);   // bottom right

    // right side
    i = 16;

    tex_id = side2;

    tx0 = (tex_id%16)*h+hh;
    tx1 = (tex_id%16)*h+h-hh;

    ty0 = (tex_id/16)*h+hh;
    ty1 = (tex_id/16)*h+h-hh;

    glColor3ub(113, 113, 113);

    glTexCoord2f(tx0,ty0); // top left
    glVertex2f(x+a[i+0],y+a[i+1]);  // bottom left

    glTexCoord2f(tx0,ty1);    // top right
    glVertex2f(x+a[i+6],y+a[i+7]-1.0f);  // flipped

    glTexCoord2f(tx1,ty1);  // top
    glVertex2f(x+a[i+4],y+a[i+5]);

    glTexCoord2f(tx1,ty0);
    glVertex2f(x+a[i+2],y+a[i+3]);   // bottom right

    glColor3ub(255, 255,255);
}
