/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "reticle.hpp"

namespace HudReticle
{

Reticle reticle;
Reticle scope_reticle;

Reticle::Reticle() :
    inited(false), x0(0), y0(0), x1(0), y1(0)
{
    memset(&this->tex_data, 0, sizeof(this->tex_data));
}

void Reticle::init(const char* filename)
{
    IF_ASSERT(this->inited) return;
    _load_image_create_texture(filename, &this->tex_data);
    this->align_center(_xres, _yres);
    this->inited = true;
}

void Reticle::align_center(int window_width, int window_height)
{
    float center_x = float(window_width) / 2.0f;
    float center_y = float(window_height) / 2.0f;

    this->x0 = center_x - (float(this->tex_data.w) / 2.0f);
    this->y0 = center_y - (float(this->tex_data.h) / 2.0f);
    this->x1 = center_x + (float(this->tex_data.w) / 2.0f);
    this->y1 = center_y + (float(this->tex_data.h) / 2.0f);
}

void Reticle::draw()
{
    IF_ASSERT(!this->inited) return;
    IF_ASSERT(this->tex_data.tex == 0) return;

    const float z = -0.1f;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, this->tex_data.tex);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor3ub(255, 0, 185);
    glBegin(GL_QUADS);
    glTexCoord2i(0, 0);
    glVertex3f(x0, y0, z);
    glTexCoord2i(1, 0);
    glVertex3f(x1, y0, z);
    glTexCoord2i(1, 1);
    glVertex3f(x1, y1, z);
    glTexCoord2i(0, 1);
    glVertex3f(x0, y1, z);
    glEnd();
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
    glColor3ub(255, 255, 255);
}

void init()
{
    reticle.init(MEDIA_PATH "sprites/reticle/reticle_02.png");
    scope_reticle.init(MEDIA_PATH "sprites/reticle/target-zoom.png");
}

}   // HudReticle
