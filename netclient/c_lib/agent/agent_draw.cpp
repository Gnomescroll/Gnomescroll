#include "agent_draw.h"


/*
 *  Client only
 */
#ifdef DC_CLIENT

namespace Agent_draw {

void draw_agents(Agent_list* agent_list) {
    
    printf("Drawing agents\n");

    struct Agent_state* g = NULL;
    int i;
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    for(i=0; i<1024; i++) {
        if(agent_list[i] != NULL) {
            g = agent_list[i];
            draw_agent(g);
        }
    }
    glDisable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);

}


void draw_agent(Agent_state* g);  {
    //printf("agent draw \n");
    draw_agent_bounding_box(g.x,g.y, g.z, 0.4, 1.8, 2.5);
}

void draw_agent_aiming_direction(float x, float y, float z, float xangle, float yangle) {
    float distance = 15;
    float density = 3;
    float delta = 1/density;

    float _x,_y,_z;
    float dx,dy,dz;

    dx = cos(xangle*PI)*cos(yangle*PI);
    dy = sin(xangle*PI)*cos(yangle*PI);
    dz = sin(yangle);
    struct Vector dv = Vector_init(dx,dy,dz);
    normalize_vector(&dv);

    glBegin(GL_POINTS);
    glColor3ub((unsigned char) 255,(unsigned char)0,(unsigned char)0);
    float l =0;
    while(l < distance) {
        l += delta;
        _x = x+dv.x*l;
        _y = y+dv.y*l;
        _z = z+dv.z*l;
        glVertex3f(_x,_y,_z);
    }
    glEnd();
    glColor3ub((unsigned char) 255,(unsigned char)255,(unsigned char)255);
}

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


void draw_agent_bounding_box(float x, float y, float z, float radius, float head_height, float height) {

    //float zm = height+z;
    float xmin,xmax,ymin,ymax;
    xmin = x - radius;
    xmax = x + radius;
    ymin = x - radius;
    ymax = x + radius;

    int i,j;
    //int a,b;
    float _x,_y,_z;

    glBegin(GL_LINES);
    glColor3ub((unsigned char)255,(unsigned char)0,(unsigned char)0);
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

    glColor3ub((unsigned char)180,(unsigned char)0,(unsigned char)0);

    for(i=0; i<12; i++) {
            j = 3*vertex_index2[2*i+0];
            _x = x + v_set2[j+0]*radius;
            _y = y + v_set2[j+1]*radius;
            _z = z+head_height + 0.50 + v_set2[j+2]*0.50;
            glVertex3f(_x,_y,_z);
            j = 3*vertex_index2[2*i+1];
            _x = x + v_set2[j+0]*radius;
            _y = y + v_set2[j+1]*radius;
            _z = z+head_height + 0.50 + v_set2[j+2]*0.50;
            glVertex3f(_x,_y,_z);
    }
    glEnd();
    glColor3ub((unsigned char) 255,(unsigned char)255,(unsigned char)255);
}

void draw_agent_cube_selection(int x, int y, int z, int r, int g, int b) {

    //printf("Draw box at %i,%i,%i \n", x,y,z);
    const float radius = 0.49;

    int i,j;
    float _x,_y,_z;
    float __x,__y,__z;
    __x = x + 0.5;
    __y = y + 0.5;
    __z = z + 0.5;

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

void draw_agent_cube_side_selection(int x, int y, int z, int cx, int cy, int cz, int r, int g, int b) {
    const float radius = 0.51;
    int i,j;
    float _x,_y,_z;
    float __x,__y,__z;
    __x = x + 0.5;
    __y = y + 0.5;
    __z = z + 0.5;

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

}
#endif
