#include "reticle.hpp"

namespace HudReticle
{
    
Reticle reticle;
Reticle scope_reticle;

Reticle::Reticle()
:
inited(false),
x0(0), y0(0), x1(0), y1(0)
{}

void Reticle::init(char* filename)
{
    if (this->inited) return;
    _load_image_create_texture(filename, &this->tex_data);
    this->align_center(_xres, _yres);
    this->inited = true;
}

void Reticle::align_center(int window_width, int window_height)
{
    float center_x = ((float)window_width) / 2.0f;
    float center_y = ((float)window_height) / 2.0f;

    this->x0 = center_x - (((float)this->tex_data.w) / 2.0f);
    this->y0 = center_y - (((float)this->tex_data.h) / 2.0f);
    this->x1 = center_x + (((float)this->tex_data.w) / 2.0f);
    this->y1 = center_y + (((float)this->tex_data.h) / 2.0f);
}

void Reticle::draw()
{
    if (!this->inited) return;
    _blit_sprite(this->tex_data.tex, x0, y0, x1, y1, -0.1f);
}


void init()
{
    printf("reticle init\n");
    reticle.init((char*)"./media/texture/target.png");
    scope_reticle.init((char*)"./media/texture/target-zoom.png");
}


}
