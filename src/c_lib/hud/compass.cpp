#include "compass.hpp"

#include <math.h>
#include <common/common.hpp>
#include <common/color.hpp>

namespace Compass {

static const char file[] = "media/texture/compass.png";

static const float scale = 1.0f;
static const int x_margin = 4;
static const int y_margin = 4;

static int width = 128;
static int height = 128;
static float x;
static float y;
static float theta = 0.5f; // rotation

struct Color BASE_COLOR = {37,247,255};

static GLuint texture = 0;

void init_texture()
{
    int ret = create_texture_from_file((char*)file, &texture);
    GS_ASSERT(ret == 0);
    GS_ASSERT(texture != 0);
}

void init()
{
    x = _xresf - (((float)width)*scale + x_margin);
    y = _yresf - (((float)height)*scale + y_margin);
    init_texture();
}

// rotate compass texture
// this function points north
void update()
{
    Vec3 forward = current_camera->forward_vector();
    forward.z = 0;
    normalize_vector(&forward);

    Vec3 north = vec3_init(0,1,0);

    float t = acosf(vec3_dot(forward, north)) / kPI;
    if ((forward.x*north.y - forward.y*north.x) > 0)
        t = -t;
    theta = t;
}

void draw()
{
    update();
    static const float z = -0.5f;
    GS_ASSERT(texture != 0);
    if (texture == 0) return;

    glColor4ub(255,255,255,255);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glBindTexture(GL_TEXTURE_2D, texture);
    draw_bound_texture_rotated(x,y, ((float)width)*scale, ((float)height)*scale, z, theta);

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

}
