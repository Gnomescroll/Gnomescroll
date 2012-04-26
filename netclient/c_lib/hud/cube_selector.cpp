#include "cube_selector.hpp"

#include <compat_gl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL/draw_functions.hpp>
#include <agent/agent.hpp>
#include <agent/agent_weapons.hpp>
#include <weapons/weapons.hpp>

#include <t_map/glsl/texture.hpp>

namespace HudCubeSelector
{

void CubeSelector::init()
{
    if (this->inited) return;
    for(int i=0;i<256;i++)
    {
        cubes[i].cube_id = 255;
        cubes[i].tex_id = 1;
    }
    this->inited = 1;
}

void CubeSelector::set_block_selector(int pos, int cube_id, int tex_id)
{
    //printf("set block selector: %d %d %d\n", pos, cube_id, tex_id);
    if(pos == -1 || tex_id == -1) return;
    if(pos < 0 || pos >= 64)
    {
        printf("CubeSelector::load_cube_property error!\n");
        return;
    }
    cubes[pos].cube_id = cube_id;
    cubes[pos].tex_id = tex_id;
}

void CubeSelector::set_position(float x, float y)
{
    this->x = _xresf - x;
    this->y = y;
}

int CubeSelector::get_active_id()    //get the cube selected by hud
{
    return cubes[this->get_active_pos()].cube_id;
}

int CubeSelector::get_active_pos()
{
    return this->pos;
}

void CubeSelector::set_active_pos(int pos)
{
    this->pos = pos;
    this->pos_x = pos % 8;
    this->pos_y = pos / 8;

    this->update_block_applier();
}

void CubeSelector::set_active_id(int id)
{
    if (id != 255)
        for (int i=0; i<256; i++)
            if (cubes[i].cube_id == id)
            {
                this->set_active_pos(i);
                break;
            }
}

void CubeSelector::update_block_applier()
{
    if (ClientState::playerAgent_state.you == NULL) return;
    ClientState::playerAgent_state.you->weapons.blocks.set_block(this->get_active_id());
}

void CubeSelector::draw()
{
    if (!this->inited) 
    {
        printf("CubeSelector::draw() error, not inited\n");
        return;
    }

    int i,j;
    float x0,y0,x1,y1;

    int ti, tj;
    int tex_id;
    float tx_min,ty_min,tx_max,ty_max;

    const float _ssize = 16;
    const float sborder = 1;
    const float z_ = -0.5;

    const float txmargin = 0.001;

    glColor3ub(255,255,255);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, t_map::block_textures_normal);

    glBegin(GL_QUADS);

    for(i=0;i<8;i++)
    for(j=0;j<8;j++)
    {
        x0 = x + i*(_ssize+sborder) + sborder/2;
        x1 = x0 + _ssize - sborder;
        y0 = y - j*(_ssize+sborder) + sborder/2;
        y1 = y0 + _ssize - sborder;

        tex_id = cubes[i+8*j].tex_id;
        ti = tex_id % 16;
        tj = tex_id / 16;

        tx_min = ti*0.0625 + txmargin;
        ty_min = tj*0.0625 + txmargin;
        tx_max = ti*0.0625 + 0.0625 - txmargin;
        ty_max = tj*0.0625 + 0.0625 - txmargin;

        glTexCoord2f(tx_min,ty_max);
        glVertex3f(x0, y0, z_);  // Top left

        glTexCoord2f(tx_max,ty_max);
        glVertex3f(x1,y0, z_);  // Top right

        glTexCoord2f(tx_max,ty_min );
        glVertex3f(x1,y1, z_);  // Bottom right

        glTexCoord2f(tx_min,ty_min );
        glVertex3f(x0,y1, z_);  // Bottom left
    }

    glEnd();
    glDisable(GL_TEXTURE_2D);

    // draw selected cube outline
    const float s_margin = 1.0; //selector margin
    i = this->pos_x;
    j = this->pos_y;
    //printf("i= %i, j= %i\n",i,j);

    x0 = x + i*(_ssize+sborder) + sborder/2 - s_margin;
    x1 = x + i*(_ssize+sborder) + _ssize - sborder/2 + s_margin;
    y0 = y - j*(_ssize+sborder) + sborder/2 - s_margin;
    y1 = y - j*(_ssize+sborder) + _ssize - sborder/2 + s_margin;
    
    x0 = x + i*(_ssize+sborder) + sborder/2;
    x1 = x0 + _ssize - sborder;
    y0 = y - j*(_ssize+sborder) + sborder/2;
    y1 = y0 + _ssize - sborder;
    
    glLineWidth(1.0);
    glColor4ub(0,0,255,0);  // blue
    glBegin(GL_LINE_STRIP);

    glVertex3f(x0, y0, z_);  // Top left

    glVertex3f(x1,y0, z_);  // Top right

    glVertex3f(x1,y1, z_);  // Bottom right

    glVertex3f(x0,y1, z_);  // Bottom left
    glVertex3f(x0, y0, z_);

    glEnd();
    glLineWidth(1.0);
    glColor3ub(255,255,255);
}

/* Controls */
void CubeSelector::up()
{
    this->vertical(true);
}
void CubeSelector::down()
{
    this->vertical(false);
}
void CubeSelector::left()
{
    this->horizontal(true);
}
void CubeSelector::right()
{
    this->horizontal(false);
}

void CubeSelector::horizontal(bool left)
{
    int shift = (left) ? -1 : 1;
    int row = this->pos / this->n_x;
    int col = this->pos % this->n_x;

    int n = (col + shift) % this->n_x;
    if (n < 0) n += this->n_x;
    n += row * this->n_x;

    if (n < 0 || n > (this->n_x*this->n_y-1)) return;

    this->set_active_pos(n);
}

void CubeSelector::vertical(bool up)
{
    int shift = (up) ? -1 : 1;
    int row = this->pos / this->n_x;
    int col = this->pos % this->n_x;

    row = (row + shift) % this->n_y;
    if (row < 0) row += this->n_y;
    int n = (row * this->n_x) + col;

    if (n < 0 || n > (this->n_x*this->n_y-1)) return;

    this->set_active_pos(n);
}

CubeSelector::CubeSelector()
:
inited(false),
x(0),y(0),
size(0),
mode(0),
n_x(8), n_y(8),
pos(0),
pos_x(0), pos_y(0)
{}

CubeSelector cube_selector;

void init()
{
    cube_selector.init();
    cube_selector.set_position(150,150);
}
}

void set_cube_hud(int pos, int cube_id, int tex_id)
{
    HudCubeSelector::cube_selector.set_block_selector(pos, cube_id, tex_id);
}
