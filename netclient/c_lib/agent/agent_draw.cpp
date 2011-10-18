#include "agent_draw.hpp"


/*
 *  Client only
 */
#ifdef DC_CLIENT


namespace AgentDraw {

/*
void draw_agents(Agent_list* agent_list) {
    
    printf("Drawing agents\n");

    struct Agent_state* g = NULL;
    int i;
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    for(i=0; i<1024; i++) {
        if(agent_list->a[i] != NULL) {
            g = agent_list->a[i];
            draw_agent(g);
        }
    }
    glDisable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);

}
*/


int snapshot_draw_array_i = 0;
int snapshot_draw_last_seq = 0;
AgentState snapshot_draw_array[32];

void add_snapshot_to_history(Agent_state* g) {

    if(g->state_snapshot.seq != snapshot_draw_last_seq) {
        snapshot_draw_last_seq = g->state_snapshot.seq;
        snapshot_draw_array[snapshot_draw_array_i] = g->state_snapshot;
        snapshot_draw_array_i = (snapshot_draw_array_i+1)%32;
    }


}

void draw_agent(Agent_state* g) {

    //printf("agent draw \n");
    AgentState* s = &g->s;
    draw_agent_bounding_box(s->x,s->y, s->z, 0.4, 1.8, 2.5);

    //AgentState* ss = &g->state_snapshot;
    //draw_agent_bounding_box(g->x,g->y, g->z, 0.4, 1.8, 2.5);s

    if(g->id == 0) {    // remove this later
        int i;
        for(i=0; i < 32; i++) {
            draw_agent_bounding_box(snapshot_draw_array[i].x,snapshot_draw_array[i].y, snapshot_draw_array[i].z, 0.4, 1.8, 2.5);  
        }

        printf("Drawin this shit\n");
        struct Vector right;
        struct Vector look;

        look = Vector_init(
                cos( s->theta * PI) * cos( s->phi * PI),
                sin( s->theta * PI) * cos( s->phi * PI),
                sin( s->phi)
        );
        normalize_vector(&look);

        right = Vector_init(cos(s->theta*PI+PI/2), sin(s->theta*PI+PI/2), 0.0);
        normalize_vector(&right);

        g->vox->draw(look, right, s->x, s->y, s->z);
    }
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

void draw_agent_bounding_box(float x, float y, float z, float radius, float head_height, float height, int r, int g, int b) {

    int i,j;
    float _x,_y,_z;


    glBegin(GL_LINES);
    glColor3ub((unsigned char)r,(unsigned char)g,(unsigned char)b);
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

    glColor3ub((unsigned char)r,(unsigned char)g,(unsigned char)b);

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

void draw_agent_bounding_box(float x, float y, float z, float radius, float head_height, float height) {

    int i,j;
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
