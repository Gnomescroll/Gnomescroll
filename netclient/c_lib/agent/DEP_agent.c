#include "DEP_agent.h"
#include "agent_vox.h"

struct Agent_vox* Agent_list[1024];

int agent_id_counter = 0;
int a_count = 0;

void agent_Tick_vox(struct Agent_vox* g);
#ifdef DC_CLIENT
void agent_Draw_vox(struct Agent_vox* g);
#endif

struct Agent_vox* get_agent_vox(int id) {
    //struct Agent_vox* g = NULL;
    int i;
    for(i=0; i<1024; i++) {
        if(Agent_list[i] != NULL && Agent_list[i]->id == id) {
            return Agent_list[i];
        }
    }
    return NULL;
}

void agent_Tick_vox(struct Agent_vox* g) {
    g->xangle += 0.01;
    g->xangle = 0.0;
    g->yangle += 0; //0.005; //0.35;
}

void agent_tick_vox() {
    struct Agent_vox* g = NULL;
    int i;
    for(i=0; i<1024; i++) {
        if(Agent_list[i] != NULL) {
            g = Agent_list[i];
            agent_Tick_vox(g);
        }
    }
}

int create_agent_vox(float x, float y, float z) {
    struct Agent_vox* g = NULL;
    int i;
    for(i=0; i<1024; i++) {
        if(Agent_list[i] == NULL) {
            g = (struct Agent_vox *) malloc (sizeof(struct Agent_vox));
            Agent_list[i] = g;
            a_count++;
            break;
        }
    }
    if(a== NULL) { printf("Bug: max agent number reached!\n"); return -1;}
    agent_id_counter +=1;
    g->id = agent_id_counter;
    printf("Agent Created: id= %i \n", g->id);
    g->x=x;
    g->y=y;
    g->z=z;
    g->vx=0;
    g->vy=0;
    g->vz=0;
    g->xangle=0;
    g->yangle=0;
    g->camera_height= 2.5;
    g->cbox_height= 3.0;
    g->cbox_radius = 0.45;

    for(i=0; i<6;i++) {
        g->vox_part[i].vox = NULL;
    }
    return g->id;
}

void destroy_agent_vox(int id) {
    struct Agent_vox* g = NULL;
    int i;
    for(i=0; i<1024; i++) {
        if(Agent_list[i]->id == id) {
            g = Agent_list[i];
            break;
        }
    }
    if( g != NULL) {
        Agent_list[i] = NULL;
        for(i=0; i < AGENT_PART_NUM; i++) {
            destroy_vox(&g->vox_part[i]);
            }


        free(g);
        g_count--;
    } else {
        printf("Destroy agent failed: agent does not exist, id=%i \n", id);
    }

}

void set_Agent_vox(int id, float xangle, float yangle) {
    struct Agent_vox* g = get_agent_vox(id);
    if(g==NULL) { printf("agent.set_Agent_vox: agent id does not exist: %i \n", id); return;}

    g->xangle = xangle;
    g->yangle = yangle;
    /*
    c->xl = c->x + cos( c->x_angle * PI) * cos( c->y_angle * PI);
    c->yl = c->y + sin( c->x_angle * PI) * cos( c->y_angle * PI);
    c->zl = c->z + sin( c->y_angle);

    length = sqrt(c->xl*c->xl + c->yl*c->yl + c->zl*c->zl);
    */
}


struct Vox* get_agent_vox_part(int id, int part) {
    struct Agent_vox* g = get_agent_vox(id);
    if(g==NULL) { printf("get_agent_vox_part: error \n"); return NULL; }
    struct Vox* v = &g->vox_part[part];
    return v;
}


/*
 *  Client only
 */
#ifdef DC_CLIENT

void agent_Draw_vox(struct Agent_vox* g) {
    //printf("agent draw \n");

    //draw_agent_bounding_box(g->x,g->y, g->z, 0.4, 1.8, 2.5);
    //return; //get voxel model working later

    float theta = g->xangle;
    float phi = g->yangle;
    struct Vector c,up,forward,right;
    struct Vector look, look2;
    c = Vector_init(g->x, g->y, g->z);
    up = Vector_init(0.0,0.0,1.0);
    forward = Vector_init(cos(PI*theta), sin(PI*theta), 0);

    //printf("phi=%f \n", phi);

    //look = Vector_init()
    look.x = cos( theta * PI) * cos( phi * PI);
    look.y = sin( theta * PI) * cos( phi * PI);
    look.z = sin( phi);
    normalize_vector(&look);

    look2.x = cos( theta * PI);
    look2.y = sin( theta * PI);
    look2.z = 0;
    normalize_vector(&look2);

    right = Vector_init(cos(theta*PI+PI/2), sin(theta*PI+PI/2), 0);

    glBegin(GL_LINES);

    glColor3ub((unsigned char)255,(unsigned char)0,(unsigned char)0);
    glVertex3f(c.x,c.y,c.z);
    glVertex3f(c.x+up.x, c.y+up.y, c.z+up.z);

    glColor3ub((unsigned char)0,(unsigned char)255,(unsigned char)0);
    glVertex3f(c.x,c.y,c.z);
    glVertex3f(c.x+right.x, c.y+right.y, c.z+right.z);

    glColor3ub((unsigned char)0,(unsigned char)0,(unsigned char)255);
    glVertex3f(c.x,c.y,c.z);
    glVertex3f(c.x+forward.x, c.y+forward.y, c.z+forward.z);

    glColor3ub((unsigned char)0,(unsigned char)255,(unsigned char)255);
    glVertex3f(c.x,c.y,c.z);
    glVertex3f(c.x+look.x, c.y+look.y, c.z+look.z);

    glEnd();

    //agent_vox_draw_head(&g->vox_part[AGENT_PART_HEAD], look, right, g);

    agent_vox_draw_vox_volume(&g->vox_part[AGENT_PART_HEAD], right, g);

    agent_vox_draw_vox_volume(&g->vox_part[AGENT_PART_TORSO], right, g);

    agent_vox_draw_vox_volume(&g->vox_part[AGENT_PART_LARM], right, g);
    agent_vox_draw_vox_volume(&g->vox_part[AGENT_PART_RARM], right, g);

    agent_vox_draw_vox_volume(&g->vox_part[AGENT_PART_LLEG], right, g);
    agent_vox_draw_vox_volume(&g->vox_part[AGENT_PART_RLEG], right, g);

}

void agent_draw_vox() {
    struct Agent_vox* g = NULL;
    int i;
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    for(i=0; i<1024; i++) {
        if(Agent_list[i] != NULL) {
            g = Agent_list[i];
            agent_Draw_vox(g);
        }
    }
    glDisable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
}

#endif
