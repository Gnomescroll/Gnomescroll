#include "t_viz.h"

#define xw 5
#define yh 5

const float _D = 0.5;

int T_VIZ_STYLE  = 0;

#define T_VIZ_STYLE2 0
#define _D2 0.5

//add alpha style?
int _D2_ss = 4; //step size 2 or 4
float _D2_ps = 2.0; //point size 1 or 2

int _vbo_indicator_style = 3;

void _toggle_t_viz_vbo_indicator_style() {
    _vbo_indicator_style = (_vbo_indicator_style+1)%4;

    if(_vbo_indicator_style == 0) {
        _D2_ss = 4; //step size 2 or 4
        _D2_ps = 2.0;
    }
    if(_vbo_indicator_style == 1) {
        _D2_ss = 3; //step size 2 or 4
        _D2_ps = 1.0;
    }
    if(_vbo_indicator_style == 2) {
        _D2_ss = 2; //step size 2 or 4
        _D2_ps = 2.0;
    }
    if(_vbo_indicator_style == 3) {
        _D2_ss = 1; //step size 2 or 4
        _D2_ps = 1.0;
    }
    printf("t_viz: vbo_indicator_style= %i\n", _vbo_indicator_style);
}

void _toggle_t_viz_perf_graph_style() { //toggle drawing style
    T_VIZ_STYLE = (T_VIZ_STYLE +1) % 2;
}

int _draw_vbo_indicator(float x, float y, float z) {
    int i,j;
    struct vm_map* m;
    struct vm_column* c;
    //float step = 1.0;

    m = _get_map();
    glPointSize(_D2_ps);
    int blend_enabled = 0;

    unsigned char alpha = 0;
    if(_vbo_indicator_style == 3 || _vbo_indicator_style == 2) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        alpha = 50;
        blend_enabled = 1;
    }

    glBegin(GL_POINTS);
    for(i=0; i<vm_map_dim; i++) {
    for(j=0; j<vm_map_dim;j++) {
        c = &m->column[j*vm_map_dim+i];

        if(flag_is_true(c, VBO_drawn)) {
            glColor4ub((unsigned char)255,(unsigned char)0,(unsigned char)0, alpha);
            if(T_VIZ_STYLE2 == 1) {
                glVertex3f(_D2 +x+_D2_ss*i, _D2 +y+_D2_ss*j, z);
            } else {
                glVertex2f(_D2 +x+_D2_ss*i, _D2 +y+_D2_ss*j);
            }
        }

        else if(flag_is_true(c, VBO_loaded)) {
            glColor4ub((unsigned char)0,(unsigned char)255,(unsigned char)255, alpha);

            if(T_VIZ_STYLE2 == 1) {
                glVertex3f(_D2 +x+_D2_ss*i, _D2 +y+_D2_ss*j, z);
            } else {
                glVertex2f(_D2 +x+_D2_ss*i, _D2 +y+_D2_ss*j);
            }

            ///glVertex3f(x+_D2_ss*i, y+_D2_ss*j, z);
        } else {
            glColor4ub((unsigned char)0,(unsigned char)0,(unsigned char)255, alpha);
            if(T_VIZ_STYLE2 == 1) {
                glVertex3f(_D2 +x+_D2_ss*i, _D2 +y+_D2_ss*j, z);
            } else {
                glVertex2f(_D2 +x+_D2_ss*i, _D2 +y+_D2_ss*j);
            }
            //glVertex3f(x+step*i, y+step*j, z);
        }

    }}
    glEnd();

    if(blend_enabled != 0) {
        glDisable(GL_BLEND);
    }
    glPointSize(1.00);
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
        if(T_VIZ_STYLE == 0) {
            glVertex3f(x+xstep*i+_D, y+ystep*fa[j]+_D, z);
        } else {
            glVertex2i(x+xstep*i, y+ystep*fa[j]);
        }
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

            if(T_VIZ_STYLE == 0){
                glVertex3f(_D+x+xstep*i, _D+y, z);
                glVertex3f(_D+x+xstep*i, _D+y+ystep*5, z);
                glVertex3f(_D+x+xstep*i, _D+y+ystep*10, z);
                glVertex3f(_D+x+xstep*i, _D+y+ystep*15, z);
                glVertex3f(_D+x+xstep*i, _D+y+ystep*20, z);
                glVertex3f(_D+x+xstep*i, _D+y+ystep*25, z);
            } else {
                glVertex2i(x+xstep*i, y);
                glVertex2i(x+xstep*i, y+ystep*5);
                glVertex2i(x+xstep*i, y+ystep*10);
                glVertex2i(x+xstep*i, y+ystep*15);
                glVertex2i(x+xstep*i, y+ystep*20);
                glVertex2i(x+xstep*i, y+ystep*25);
                //glVertex3f(x+xstep*i, y+ystep*30, z);
            }
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
