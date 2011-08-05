#include "t_viz.h"


#define xw 5
#define yh 5

int _draw_vbo_indicator(float x, float y, float z) {
    int i,j;
    struct vm_map* m;
    struct vm_column* c;
    float step = 1.0;

    m = _get_map();
    glBegin(GL_POINTS);
    for(i=0; i<vm_map_dim; i++) {
    for(j=0; j<vm_map_dim;j++) {
        c = &m->column[j*vm_map_dim+i];

        if(flag_is_true(c, VBO_drawn)) {
            glColor3ub((unsigned char)255,(unsigned char)0,(unsigned char)0);
            glVertex3f(x+step*i, y+step*j, z);
        }

        else if(flag_is_true(c, VBO_loaded)) {
            glColor3ub((unsigned char)0,(unsigned char)255,(unsigned char)255);
            glVertex3f(x+step*i, y+step*j, z);
        } else {
            glColor3ub((unsigned char)0,(unsigned char)0,(unsigned char)255);
            glVertex3f(x+step*i, y+step*j, z);
        }

    }}
    glEnd();
}

#define f_count 350

int fn = 0;
int fa[f_count];
int intv;
int intva[f_count];

int _start_frame() {
int tc = SDL_GetTicks();
fa[fn] =  tc - fa[fn];
intv += fa[fn]; if(intv > 5000) { intv = 0; }
if(intv >= 1000) {
    intv -= 1000;
    intva[fn] = 1;
    } else {
    intva[fn] = 0;
    }

//printf("t= %i \n", fa[fn]);
fn = (fn+1) % f_count;
fa[fn] =  tc;
}

int _event(int id){ return 0; }

int _end_frame() {
return 0;
}

int _draw_perf_graph(float x, float y, float z) {
    int i,j;
    int t0, t1;
    i=0; j = fn;
    float xstep = 1.0;
    float ystep = 2.0;
    glBegin(GL_POINTS);
    while(1) {
        j--;
        if( j<0) { j = f_count-1;}
        i++;
        if(j==fn) break;
        glColor3ub((unsigned char)0,(unsigned char)0,(unsigned char)255);
        glVertex3f(x+xstep*i, y+ystep*fa[j], z);
        if(fa[j] > 20) {
            t1=fa[j]; t0=0;
            glColor3ub((unsigned char)00,(unsigned char)255,(unsigned char)0);
            while(t1 > 0) {

            glVertex3f(x+xstep*i, ystep*t0+y, z);
            t1 -=10;
            t0++;
        }}
        if(intva[j] == 1) {
            glColor3ub((unsigned char)0,(unsigned char)255,(unsigned char)0);
            glVertex3f(x+xstep*i, y, z);
            glVertex3f(x+xstep*i, y+ystep*5, z);
            glVertex3f(x+xstep*i, y+ystep*10, z);
            glVertex3f(x+xstep*i, y+ystep*15, z);
            glVertex3f(x+xstep*i, y+ystep*20, z);
            glVertex3f(x+xstep*i, y+ystep*25, z);
            //glVertex3f(x+xstep*i, y+ystep*30, z);
        }
    }
    glEnd();
    return 0;
}
/*
glColor3ub((unsigned char)r,(unsigned char)g,(unsigned char)b);
glBegin(GL_POINTS);
glVertex3f(x0,y0,z0); // point
glEnd();
return 0;
*/
