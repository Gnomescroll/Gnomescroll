#pragma once

#include <common/color.hpp>

struct VertexDraw {
    float x,y,z;
    float tx,ty;
    unsigned char r,g,b,a;
};

struct Quad {
  struct VertexDraw vertex[4];
};

int _draw_line(unsigned char r, unsigned char g, unsigned char b, float x0, float y0, float z0, float x1, float y1, float z1);
int _draw_point(unsigned char r, unsigned char g, unsigned char b, float x0, float y0, float z0);
int _blit_sprite(int tex, float x0, float y0, float x1, float y1, float z);
int _draw_rect(unsigned char r, unsigned char g, unsigned char b, float x, float y, float w, float h);
int _draw_rect(Color color, float x, float y, float w, float h);
int _draw_border_rect(unsigned char r, unsigned char g, unsigned char b, float x, float y, float w, float h);

int _bind_VBO(struct Quad* quad_list, int v_num);

// Draw full texture
void draw_bound_texture(float x, float y, float w, float h, float z);
void draw_bound_texture(float x, float y, float w, float h);
void draw_bound_texture_rotated(float x, float y, float w, float h, float z, float theta);
void draw_bound_texture_rotated(float x, float y, float w, float h, float tx, float ty, float tw, float th, float depth, float theta);

// Blit sprites from a larger bound texture
void draw_bound_texture_sprite(float x, float y, float w, float h, float z, float sprite_x, float sprite_y, float sprite_width, float sprite_height);

void draw_iso_cube(float x, float y, float scale, int side0, int side1, int side2);
