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

void Reticle::init(const char* filename)
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
    //glColor3ub(127,127,127);
    //_blit_sprite(this->tex_data.tex, x0, y0, x1, y1, -0.1f);

    const float z = -0.1f;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, this->tex_data.tex );
    
    glEnable(GL_BLEND);
    

    //glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //glEnable(GL_COLOR_LOGIC_OP); 
    //glLogicOp(GL_XOR); 


    //glColor3ub(127, 127, 127);

    //glEnable(GL_COLOR_LOGIC_OP);
    //glLogicOp(GL_INVERT);


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
    //glDisable(GL_COLOR_LOGIC_OP);


    //glColor3ub(255, 255, 255);

}


void init()
{
    reticle.init("./media/sprites/reticle/reticle_02.png"); //target.png");
    scope_reticle.init("./media/sprites/reticle/target-zoom.png");
}


}
