#include "compass.hpp"

#include <math.h>
#include <common/common.hpp>

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

struct Color base_color = {37,247,255};

static int current_texture_index = 0;
static GLuint texture[N_TEAMS] = {0};

void update_team_colors()
{
    //static struct Color cache[N_TEAMS] = {{0,0,0}};
    //unsigned char r,g,b;
    //if (ClientState::ctf == NULL) return;
    //for (int i=0; i<(int)N_TEAMS; i++)
    //{
        //if (ClientState::ctf->get_team_color(i+1, &r, &g, &b))
            //continue;
        //if (cache[i].r == r && cache[i].g == g && cache[i].b == b)
            //continue;
        //load_colored_texture(file, &texture[i],
            //base_color.r, base_color.g, base_color.b,
            //r, g, b
        //);
        //cache[i].r = r;
        //cache[i].g = g;
        //cache[i].b = b;
    //}
}

void init_texture()
{
    int ret = create_texture_from_file((char*)file, &texture[current_texture_index]);
    GS_ASSERT(ret == 0);
}

void init()
{
    x = _xresf - (((float)width)*scale + x_margin);
    y = _yresf - (((float)height)*scale + y_margin);
    init_texture();
}

//// rotate compass texture
//// this function points in direction of target destination (flag, base)
//void update()
//{
    //using ClientState::playerAgent_state;
    //using ClientState::ctf;
    
    //Agent_state* a = playerAgent_state.you;
    //AgentState* s = &playerAgent_state.camera_state;

    //if (a != NULL && a->status.team)    // set index to current team by default
        //current_texture_index = a->status.team - 1;

    //if (a == NULL || ctf == NULL || !a->status.team)
    //{   // just point in agent direction
        //theta = s->theta;
        //return;
    //}

    //Vec3 goal_pos;
    //if (a->status.has_flag)
    //{   // point to home base
        //Base* b = ctf->get_base(a->status.team);
        //if (b == NULL)
        //{
            //theta = s->theta;
            //return;
        //}
        //goal_pos = vec3_init(b->x, b->y, 0);
        //current_texture_index = a->status.team - 1;
    //}
    //else
    //{   // point to enemy's flag
        //Flag* f = ctf->get_enemy_flag(a->status.team);
        //if (f == NULL)
        //{
            //theta = s->theta;
            //return;
        //}
        //goal_pos = vec3_init(f->x, f->y, 0);
        //current_texture_index = (a->status.team == 1) ? 2-1 : 1-1;
    //}

    //goal_pos.x -= s->x;
    //goal_pos.y -= s->y;
    //normalize_vector(&goal_pos);

    //Vec3 forward = s->forward_vector();
    //forward.z = 0;
    //normalize_vector(&forward);

    //float t = acos(vec3_dot(forward, goal_pos)) / kPI;
    //if ((forward.x*goal_pos.y - forward.y*goal_pos.x) < 0)
        //t = -t;
    //theta = t;
//}

// rotate compass texture
// this function points north
void update()
{
    using ClientState::playerAgent_state;
    
    Agent_state* a = playerAgent_state.you;
    AgentState* s = &playerAgent_state.camera_state;

    //if (a != NULL && a->status.team)    // set index to current team by default
        //current_texture_index = a->status.team - 1;

    if (a == NULL)
    {   // just point in camera direction
        theta = s->theta;
        return;
    }

    Vec3 forward = s->forward_vector();
    forward.z = 0;
    normalize_vector(&forward);

    Vec3 north = vec3_init(0,1,0);

    float t = acos(vec3_dot(forward, north)) / kPI;
    if ((forward.x*north.y - forward.y*north.x) > 0)
        t = -t;
    theta = t;
}

void draw()
{
    update();
    static const float z = -0.5f;    
    if (texture[current_texture_index] == 0) return;

    glColor4ub(255,255,255,255);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glBindTexture(GL_TEXTURE_2D, texture[current_texture_index]);
    draw_bound_texture_rotated(x,y, ((float)width)*scale, ((float)height)*scale, z, theta);

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

}
