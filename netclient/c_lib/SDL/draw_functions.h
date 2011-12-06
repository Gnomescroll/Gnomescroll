#pragma once

#include <compat_gl.h>
#include <stdio.h>

struct VertexDraw {
    float x,y,z;
    float tx,ty;
    unsigned char r,g,b,a;
};

struct Quad {
  struct VertexDraw vertex[4];
};

int _draw_line(int r, int g,int b, float x0, float y0, float z0, float x1, float y1, float z1);
int _draw_point(int r, int g,int b, float x0, float y0, float z0);
int _blit_sprite(int tex, float x0, float y0, float x1, float y1, float z);
int _draw_rect(int r, int g, int b, float x, float y, float w, float h);
int _draw_border_rect(int r, int g, int b, float x, float y, float w, float h);

int _bind_VBO(struct Quad* quad_list, int v_num);

void draw_bound_texture(float x, float y, float w, float h, float z);
