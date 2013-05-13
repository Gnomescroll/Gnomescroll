#include "agent_draw.hpp"

#if DC_SERVER
dont_include_this_file_in_server
#endif

#include <agent/constants.hpp>
#include <voxel/constants.hpp>

namespace AgentDraw
{

void draw_agent(Agents::Agent* g)
{
    Agents::AgentState s = g->get_state();
    draw_agent_bounding_box(s.x, s.y, s.z, g->box.box_r, CAMERA_HEIGHT,
                            g->box.height, Color(0, 0, 255));
}

void draw_agent_aiming_direction(float x, float y, float z, float theta, float phi)
{
    const float distance = 15.0f;
    const float density = 3.0f;
    const float delta = 1.0f/density;
    float dx = cosf(theta * PI) * cosf(phi * PI);
    float dy = sinf(theta * PI) * cosf(phi * PI);
    float dz = sinf(phi);
    Vec3 dv = vec3_init(dx, dy, dz);
    dv = vec3_normalize(dv);

    GL_ASSERT(GL_TEXTURE_2D, false);
    glBegin(GL_POINTS);
    glColor4ub(255, 0, 0, 255);
    for (int i=0; i<=distance/delta; i++)
    {
        float _x = x + dv.x * (delta * i);
        float _y = y + dv.y * (delta * i);
        float _z = z + dv.z * (delta * i);
        glVertex3f(_x,_y,_z);
    }
    glColor4ub(255, 255, 255, 255);
    glEnd();
}

void draw_agent_bounding_box(float x, float y, float z, float radius,
                             float head_height, float height, Color color)
{
    glColor4ub(color.r, color.g, color.b, color.a);
    glBegin(GL_LINES);
    for (int i=0; i<12; i++)
    {
        int j = 3*vertex_index2[2*i+0];
        float _x = x + v_set2[j+0]*radius;
        float _y = y + v_set2[j+1]*radius;
        float _z = z+ head_height/2 + v_set2[j+2]*head_height/2;
        glVertex3f(_x,_y,_z);
        j = 3*vertex_index2[2*i+1];
        _x = x + v_set2[j+0]*radius;
        _y = y + v_set2[j+1]*radius;
        _z = z+head_height/2 + v_set2[j+2]*head_height/2;
        glVertex3f(_x,_y,_z);
    }

    for (int i=0; i<12; i++)
    {
        int j = 3*vertex_index2[2*i+0];
        float _x = x + v_set2[j+0]*radius;
        float _y = y + v_set2[j+1]*radius;
        float _z = z+head_height + 0.50f + v_set2[j+2]*0.50f;
        glVertex3f(_x,_y,_z);
        j = 3*vertex_index2[2*i+1];
        _x = x + v_set2[j+0]*radius;
        _y = y + v_set2[j+1]*radius;
        _z = z+head_height + 0.50f + v_set2[j+2]*0.50f;
        glVertex3f(_x,_y,_z);
    }
    glEnd();
    glColor4ub(255, 255, 255, 255);
}

void draw_agent_bounding_box(float x, float y, float z, float radius,
                             float head_height, float height)
{
    draw_agent_bounding_box(x, y, z, radius, head_height, height, Color(255, 0, 0));
}

void draw_agent_cube_selection(int x, int y, int z, Color color)
{
    const float radius = 0.49f;
    x += 0.5f;
    y += 0.5f;
    z += 0.5f;

    glColor4ub(color.r, color.g, color.b, color.a);
    glBegin(GL_LINES);
    for (int i=0; i<12; i++)
    {
        int j = 3*vertex_index2[2*i+0];
        float _x = x + v_set2[j+0]*radius;
        float _y = y + v_set2[j+1]*radius;
        float _z = z + v_set2[j+2]*radius;
        glVertex3f(_x,_y,_z);
        j = 3*vertex_index2[2*i+1];
        _x = x + v_set2[j+0]*radius;
        _y = y + v_set2[j+1]*radius;
        _z = z + v_set2[j+2]*radius;
        glVertex3f(_x,_y,_z);
    }
    glEnd();
    glColor4ub(255, 255, 255, 255);
}

}   // AgentDraw
