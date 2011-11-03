#pragma once

#include <math.h>

#include <compat.h>
#ifdef DC_CLIENT
#include <compat_gl.h>
#endif

#include <physics/vector.h>

#define AGENT_PART_NUM 6
#define AGENT_PART_HEAD 0
#define AGENT_PART_TORSO 1
#define AGENT_PART_LARM 2
#define AGENT_PART_RARM 3
#define AGENT_PART_LLEG 4
#define AGENT_PART_RLEG 5

struct Voxel {
unsigned char r,g,b,a;
};

class Vox {
    public:
        float part_num;
        struct Vector f,n,u; //forward, normal, up
        struct Vector a; // base anchor
        struct Vector anchor; // working anchor

        unsigned short xdim;
        unsigned short ydim;
        unsigned short zdim;
        float vox_size;
        float radius;
        struct Voxel* vox; // will be malloc'd
        unsigned int num_vox;

        struct Vector c; //center
        float length;
        Vox(int pt_num,
            unsigned short _xdim, unsigned short _ydim, unsigned short _zdim,
            float vosize) {
            part_num = pt_num;
            f = Vector_init(0.0f, 0.0f, 0.0f);
            n = Vector_init(0.0f, 0.0f, 0.0f);
            u = Vector_init(0.0f, 0.0f, 0.0f);
            a = Vector_init(0.0f, 0.0f, 0.0f);
            anchor = Vector_init(0.0f, 0.0f, 0.0f);
            xdim = _xdim;
            ydim = _ydim;
            zdim = _zdim;
            vox_size = vosize;
            radius = sqrt((vox_size*xdim)*(vox_size*xdim) + (vox_size*ydim)*(vox_size*ydim) + (vox_size*zdim)*(vox_size*zdim));
            num_vox = _xdim*_ydim*_zdim;
            vox = (struct Voxel*) malloc(num_vox*sizeof(struct Voxel));

            unsigned int i;
            for(i=0; i<num_vox; i++) {
                vox[i].r = 0;
                vox[i].g = 0;
                vox[i].b = 255;
                vox[i].a = 0; //if alpha is zero, dont draw
            }

            length = 1.0;
            a.x = 4;
            a.y = 0;
            a.z = 0.0;
        }
        
        ~Vox() {
            free(vox);
        }

        void set_anchor_point(float ax, float ay, float az);
        void set_base_anchor_point(float len, float ax, float ay, float az);
        void set_direction(float fx, float fy, float fz, float nx, float ny, float nz);
        void set_volume(int x, int y, int z, int r, int g, int b, int a);
        void draw(struct Vector forward, struct Vector right, float x, float y, float z);
        void draw_head(struct Vector look, struct Vector right, float x, float y, float z);

        void rotate_anchor(float theta);
};

class Agent_vox {
    public:
        float lv,ly,lz; //looking vector
        float camera_height;
        float cbox_height;
        float cbox_radius; // collision box
        class Vox* vox_part[AGENT_PART_NUM]; //head,torso, larm,rarm, lleg, rleg
        int vox_ready;
        Agent_vox() {
            lv=ly=lz=0.0;
            camera_height=2.5;
            cbox_height=3.0;
            cbox_radius=0.45;
            for (int i=0; i<AGENT_PART_NUM; i++) {
                vox_part[i] = NULL;
            }
            vox_ready = 0;
        }
        
        ~Agent_vox() {
            if (vox_ready) {
                for (int  i=0; i<AGENT_PART_NUM; i++) {
                    if (vox_part[i] != NULL) {
                        delete vox_part[i];
                    }
                }
            }
        }

        void init_vox_done();
        void init_vox_part(int part, int _xdim, int _ydim, int _zdim, float vosize);
        void set_limb_direction(int part, float fx, float fy, float fz, float nx, float ny, float nz);
        void set_limb_anchor_point(int part, float ax, float ay, float az);
        void set_limb_base_anchor_point(int part, float len, float ax, float ay, float az);
        void set_vox_volume(int part, int x, int y, int z, int r, int g, int b, int a);

        #ifdef DC_CLIENT
        //void draw_volume(int part, struct Vector right, float x, float y, float z);
        void draw(float x, float y, float z, float theta, float phi);
        #endif
};
