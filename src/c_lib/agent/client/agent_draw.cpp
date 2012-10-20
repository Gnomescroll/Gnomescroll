#include "agent_draw.hpp"

#if DC_SERVER
dont_include_this_file_in_server
#endif

#include <agent/constants.hpp>

const int SNAPSHOT_DRAW_ARRAY_SIZE = 32;

namespace AgentDraw
{

//static int snapshot_draw_array_i = 0;
//static int snapshot_draw_last_seq = 0;
//AgentState snapshot_draw_array[SNAPSHOT_DRAW_ARRAY_SIZE];

//static int _last_seq = 0;

void add_snapshot_to_history(Agent* g)
{
    //if(g->state_snapshot.seq != snapshot_draw_last_seq) {
        //snapshot_draw_last_seq = g->state_snapshot.seq;
        //snapshot_draw_array[snapshot_draw_array_i] = g->state_snapshot;
        //snapshot_draw_array_i = (snapshot_draw_array_i+1)%SNAPSHOT_DRAW_ARRAY_SIZE;


        //if(g->state_snapshot.seq != (_last_seq + SNAPSHOT_DRAW_ARRAY_SIZE)%256) {
            
            //printf("AgentDraw, add_snapshot_to_history !!! ERROR: seq= %i, last_seq= %i \n", g->state_snapshot.seq, _last_seq);

        //}
        //_last_seq = g->state_snapshot.seq;
    //}
}

void draw_agent(Agent* g) {

    //printf("agent draw \n");
    AgentState s = g->get_state();
    //draw_agent_bounding_box(s->x,s->y, s->z, 0.4, 1.8, 2.5, 0,0,255);
    //printf("id=%i, x= %f, y= %f, z= %f \n", g->id, s->x, s->y, s->z);
    //printf("draw\n");

    //AgentState* ss = &g->state_snapshot;
    //draw_agent_bounding_box(g->x,g->y, g->z, 0.4, 1.8, 2.5);s

/*
    int i;
    for(i=0; i < SNAPSHOT_DRAW_ARRAY_SIZE; i++) {
        draw_agent_bounding_box(snapshot_draw_array[i].x,snapshot_draw_array[i].y, snapshot_draw_array[i].z, 0.4, 1.0, 2.0);  
    }
*/
    draw_agent_bounding_box(s.x, s.y, s.z, 0.4f, 1.8f, 2.5f, 0,0,255);
}

void draw_agent_aiming_direction(float x, float y, float z, float xangle, float yangle) {
    const float distance = 15.0f;
    const float density = 3.0f;
    const float delta = 1.0f/density;

    float _x,_y,_z;
    float dx,dy,dz;

    dx = cosf(xangle*PI)*cosf(yangle*PI);
    dy = sinf(xangle*PI)*cosf(yangle*PI);
    dz = sinf(yangle);
    Vec3 dv = vec3_init(dx,dy,dz);
    normalize_vector(&dv);

    glBegin(GL_POINTS);

    glDisable(GL_TEXTURE_2D);
    
    //glEnable(GL_CULL_FACE);

    glColor3ub((unsigned char) 255,(unsigned char)0,(unsigned char)0);
    float l =0;
    while(l < distance) {
        l += delta;
        _x = x+dv.x*l;
        _y = y+dv.y*l;
        _z = z+dv.z*l;
        glVertex3f(_x,_y,_z);
    }
    glColor3ub((unsigned char) 255,(unsigned char)255,(unsigned char)255);
    //glDisable(GL_CULL_FACE);

    glEnable(GL_TEXTURE_2D);
    glEnd();
}

#include <voxel/constants.hpp>
/*
int v_set2[3*8] = {
        -1,-1,-1,
        1,-1,-1,
        1,1,-1,
        -1,1,-1,
        -1,-1,1,
        1,-1,1,
        1,1,1,
        -1,1,1,
    };

int vertex_index2[2*12] = {
        0,1,
        1,2,
        2,3,
        3,0,

        4,5,
        5,6,
        6,7,
        7,4,

        0,4,
        1,5,
        2,6,
        3,7,
    };

int side_v2[4*6] =  {
     4,5,6,7 ,   //top (z=1)
     0,1,2,3 ,   //bottom (z=0)
     1,5,9,10 ,  //north (y=1)
     7,3,11,8 ,  //south (y=0)
     6,2,10,11 , //west (x=0)
     4,0,9,8 ,   //east (x=1)
    };

float v_set3[3*8] = {
        0,0,0,
        1,0,0,
        1,1,0,
        0,1,0,
        0,0,1,
        1,0,1,
        1,1,1,
        0,1,1,
    };
*/

void draw_agent_bounding_box(float x, float y, float z,
    float radius, float head_height, float height,
    unsigned char r, unsigned char g, unsigned char b)
{
    int i,j;
    float _x,_y,_z;

    glBegin(GL_LINES);
    glColor3ub(r,g,b);
    for(i=0; i<12; i++) {
            j = 3*vertex_index2[2*i+0];
            _x = x + v_set2[j+0]*radius;
            _y = y + v_set2[j+1]*radius;
            _z = z+ head_height/2 + v_set2[j+2]*head_height/2;
            glVertex3f(_x,_y,_z);
            j = 3*vertex_index2[2*i+1];
            _x = x + v_set2[j+0]*radius;
            _y = y + v_set2[j+1]*radius;
            _z = z+head_height/2 + v_set2[j+2]*head_height/2;
            glVertex3f(_x,_y,_z);
    }

    glColor3ub(r,g,b);

    for(i=0; i<12; i++) {
            j = 3*vertex_index2[2*i+0];
            _x = x + v_set2[j+0]*radius;
            _y = y + v_set2[j+1]*radius;
            _z = z+head_height + 0.50f + v_set2[j+2]*0.50f;
            glVertex3f(_x,_y,_z);
            j = 3*vertex_index2[2*i+1];
            _x = x + v_set2[j+0]*radius;
            _y = y + v_set2[j+1]*radius;
            _z = z+head_height + 0.50f + v_set2[j+2]*0.50f;
            glVertex3f(_x,_y,_z);
    }
    glEnd();
    glColor3ub( 255,255,255);
}

void draw_agent_bounding_box(float x, float y, float z,
    float radius, float head_height, float height)
{
    draw_agent_bounding_box(x,y,z,radius,head_height,height, 255,0,0);
}

void draw_agent_cube_selection(int x, int y, int z, int r, int g, int b) {

    //printf("Draw box at %i,%i,%i \n", x,y,z);
    const float radius = 0.49f;

    int i,j;
    float _x,_y,_z;
    float __x,__y,__z;
    __x = x + 0.5f;
    __y = y + 0.5f;
    __z = z + 0.5f;

    glBegin(GL_LINES);
    glColor3ub((unsigned char)r,(unsigned char)g,(unsigned char)b);
    for(i=0; i<12; i++) {
            j = 3*vertex_index2[2*i+0];
            _x = __x + v_set2[j+0]*radius;
            _y = __y + v_set2[j+1]*radius;
            _z = __z + v_set2[j+2]*radius;
            glVertex3f(_x,_y,_z);
            j = 3*vertex_index2[2*i+1];
            _x = __x + v_set2[j+0]*radius;
            _y = __y + v_set2[j+1]*radius;
            _z = __z + v_set2[j+2]*radius;
            glVertex3f(_x,_y,_z);
    }
    glEnd();

    glColor3ub((unsigned char) 255,(unsigned char)255,(unsigned char)255);
}

}   // AgentDraw
