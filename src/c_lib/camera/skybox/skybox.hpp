/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

namespace Skybox
{

struct STAR
{
    int type;
    float brightness;
    float size;

    float x;
    float y;
    float z;

    float tx_min;
    float ty_min;
    float tx_max;
    float ty_max;
};

extern struct STAR* star_list;
extern int star_num;

extern GLuint star_sheet;

void init_shader();
void init_texture();
void generate_sky();
void init();
void teardown();
void pack_vertex_list();
void prep_skybox();
void draw();

}    // Skybox

extern "C"
{
void add_star(int type, float brightness, float size, float phi, float theta)
{

}

void clear_stars()
{
    Skybox::star_num = 0;
}
}
