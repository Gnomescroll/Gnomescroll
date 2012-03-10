#include "compass.hpp"

namespace Compass {

static char file[] = "media/texture/compass.png";

static const float scale = 0.5f;
static const int x_margin = 10;
static const int y_margin = 10;

static int width;
static int height;
static float x;
static float y;
static float theta = 0.5f; // rotation

static SDL_Surface* surface = NULL;
static GLuint texture = 0;

void init()
{
    surface = create_surface_from_file(file);

    if (surface == NULL)
    {
        printf("Compass surface init failed\n");
        return;
    }

    width = surface->w;
    height = surface->h;
    x = _xresf - ((float)width * scale) - x_margin;
    y = _yresf - ((float)height * scale) - y_margin;

    Uint32 tex_format = GL_BGRA;
    if (surface->format->Rmask == 0x000000ff)
        tex_format = GL_RGBA;

    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D( GL_TEXTURE_2D, 0, 4, surface->w, surface->h, 0, tex_format, GL_UNSIGNED_BYTE, surface->pixels );
    glDisable(GL_TEXTURE_2D);

    SDL_FreeSurface(surface);
    surface = NULL;
}

#include <math.h>

// rotate compass texture
void update()
{
    using ClientState::playerAgent_state;
    using ClientState::ctf;
    
    Agent_state* a = playerAgent_state.you;
    AgentState* s = &playerAgent_state.camera_state;
    if (a == NULL || ctf == NULL)
    {   // just point in agent direction
        theta = s->theta;
        return;
    }

    Vec3 goal_pos;
    if (a->status.has_flag)
    {
        Base* b = ctf->get_base(a->status.team);
        if (b == NULL)
        {
            theta = s->theta;
            return;
        }
        goal_pos = vec3_init(b->x, b->y, 0);
    }
    else
    {
        Flag* f = ctf->get_enemy_flag(a->status.team);
        if (f == NULL)
        {
            theta = s->theta;
            return;
        }
        goal_pos = vec3_init(f->x, f->y, 0);
    }

    goal_pos.x -= s->x;
    goal_pos.y -= s->y;
    normalize_vector(&goal_pos);

    Vec3 forward = s->forward_vector();
    forward.z = 0;
    normalize_vector(&forward);

    float t = acos(vec3_dot(forward, goal_pos)) / kPI;
    if ((forward.x*goal_pos.y - forward.y*goal_pos.x) < 0)
        t = -t;
    theta = t;
}

void draw()
{
    static const float z = -0.5f;
    if (texture == 0) return;
    update();

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
