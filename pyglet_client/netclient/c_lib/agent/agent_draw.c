#include "agent_draw.h"

/*
        self.draw_aiming_direction()
        self.draw_bounding_box()
        self.draw_position(points=10, seperation = 0.10)
        self.draw_velocity(point_density=15, units=200)
        P.event("draw acceleration")

        P.event("ray cast")
        pos = ray_cast_farest_empty_block(self.x,self.y,self.z,self.x_angle,self.y_angle)
        if pos != None:
            #print str(pos)
            (x,y,z) = pos
            draw_cube(x,y,z,[0,155,0])
*/

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

void draw_agent_bounding_box(float x, float y, float z, float radius, float head_height, float height) {

    float zm = height+z;
    float xmin,xmax,ymin,ymax;
    xmin = x - radius;
    xmax = x + radius;
    ymin = x - radius;
    ymax = x + radius;

    int i,j;
    int a,b;
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

}


