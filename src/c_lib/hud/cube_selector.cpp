#include "cube_selector.hpp"

#include <common/compat_gl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL/draw_functions.hpp>
#include <agent/agent.hpp>
//#include <agent/agent_weapons.hpp>
//#include <weapons/weapons.hpp>

#include <t_map/glsl/texture.hpp>

namespace HudCubeSelector
{

void CubeSelector::init()
{
    GS_ASSERT(this->cubes == NULL);
    if (this->cubes != NULL) return;
    this->cubes = (struct cube_select_element*)malloc(N_CUBES * sizeof(struct cube_select_element));
    for(int i=0;i<N_CUBES;i++)
    {
        cubes[i].cube_id = 255;
        cubes[i].tex_id = 1;
    }
}

void CubeSelector::set_block_selector(int pos, int cube_id, int tex_id)
{
    GS_ASSERT(tex_id >= 0);
    GS_ASSERT(pos >= 0 && pos < 64);
    if (tex_id < 0) return;
    if (pos < 0 || pos >= 64) return;
    
    GS_ASSERT(cubes[pos].cube_id == 255);
    cubes[pos].cube_id = cube_id;

    if (cube_id != 255)
        for (int i=0; i<64; i++)
            if (cubes[i].tex_id != 1)
                GS_ASSERT(cubes[i].tex_id != tex_id); 
    
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
}

void CubeSelector::set_active_id(int id)
{
    if (id == 255) return;

    for (int i=0; i<256; i++)
        if (cubes[i].cube_id == id)
        {
            this->set_active_pos(i);
            break;
        }
}

void CubeSelector::draw()
{
    GS_ASSERT(this->cubes != NULL);
    if (this->cubes == NULL) return;
    int i,j;
    float x0,y0,x1,y1;

    int ti, tj;
    int tex_id;
    float tx_min,ty_min,tx_max,ty_max;

    const float _ssize = 16;
    const float sborder = 1;
    const float z_ = -0.5f;

    const float txmargin = 0.001f;

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

        tx_min = ti*0.0625f + txmargin;
        ty_min = tj*0.0625f + txmargin;
        tx_max = ti*0.0625f + 0.0625f - txmargin;
        ty_max = tj*0.0625f + 0.0625f - txmargin;

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
    const float s_margin = 1.0f; //selector margin
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
    
    glLineWidth(1.0f);
    glColor4ub(0,0,255,255);  // blue
    glBegin(GL_LINE_STRIP);

    glVertex3f(x0, y0, z_);  // Top left

    glVertex3f(x1,y0, z_);  // Top right

    glVertex3f(x1,y1, z_);  // Bottom right

    glVertex3f(x0,y1, z_);  // Bottom left
    glVertex3f(x0, y0, z_);

    glEnd();
    glLineWidth(1.0f);
    glColor4ub(255,255,255,255);
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

bool CubeSelector::set_block_type(int block_type)
{
    GS_ASSERT(this->cubes != NULL);
    if (this->cubes == NULL) return false;
    
    ASSERT_VALID_CUBE_ID(block_type);
    GS_ASSERT(t_map::isInUse(block_type));
    GS_ASSERT(!t_map::isErrorBlock(block_type));
    if (!t_map::isInUse(block_type) || t_map::isErrorBlock(block_type)) return false;
    IF_INVALID_CUBE_ID(block_type) return false;

    for (int i=0; i<this->n_x*this->n_y; i++)
    {
        if (this->cubes[i].cube_id == block_type)
        {
            this->set_active_pos(i);
            return true;
        } 
    }
    return false;
}

CubeSelector::CubeSelector() :
    x(0),y(0),
    size(0),
    mode(0),
    n_x(8), n_y(8),
    pos(0),
    pos_x(0), pos_y(0),
    cubes(NULL)
{}

CubeSelector cube_selector;

void init()
{
    cube_selector.init();
    cube_selector.set_position(150,250);
}

}

void set_cube_hud(int hudx, int hudy, int cube_id, int tex_id)
{
    if(hudx < 1 || hudy < 1 || hudx > 8 || hudy > 8)
    {
        printf("set_cube_hud error! hux= %i hudy= %i \n", hudx,hudy);
        return;
    }
    hudx--;
    hudy--;
    HudCubeSelector::cube_selector.set_block_selector(8*hudy+hudx, cube_id, tex_id);
}
