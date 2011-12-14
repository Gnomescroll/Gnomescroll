#pragma once

#include <math.h>

#include <compat.h>
#ifdef DC_CLIENT
#include <compat_gl.h>
#endif

#include <c_lib/voxel/common.h>
#include <physics/vector.hpp>

const int AGENT_PART_NUM = 6;
#define AGENT_PART_HEAD 0
#define AGENT_PART_TORSO 1
#define AGENT_PART_LARM 2
#define AGENT_PART_RARM 3
#define AGENT_PART_LLEG 4
#define AGENT_PART_RLEG 5

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

        void set_color(int r, int g, int b, int a);
        void set_color(int x, int y, int z, int r, int g, int b, int a);


        void rotate_anchor(float theta);
};

class Agent_vox {
    public:
        float lv,ly,lz; //looking vector
        float camera_height;
        float cbox_height;
        float cbox_radius; // collision box
        class Vox* vox_part[AGENT_PART_NUM]; //head,torso, larm,rarm, lleg, rleg
        Agent_vox() {
            lv=ly=lz=0.0;
            camera_height=2.5;
            cbox_height=3.0;
            cbox_radius=0.45;
            for (int i=0; i<AGENT_PART_NUM; i++) {
                vox_part[i] = NULL;
            }
            init();
        }
        
        ~Agent_vox() {
            for (int  i=0; i<AGENT_PART_NUM; i++) {
                if (vox_part[i] != NULL) {
                    delete vox_part[i];
                }
            }
        }
        void init();
        void init_vox_part(int part, int _xdim, int _ydim, int _zdim, float vosize);
        void set_limb_direction(int part, float fx, float fy, float fz, float nx, float ny, float nz);
        void set_limb_anchor_point(int part, float ax, float ay, float az);
        void set_limb_base_anchor_point(int part, float len, float ax, float ay, float az);
        void set_vox_volume(int part, int x, int y, int z, int r, int g, int b, int a);

        #ifdef DC_CLIENT
        void draw(float x, float y, float z, float theta, float phi);
        #endif
};


/* Dat storage */

class VoxPartRotation {
    public:
        float fx,fy,fz;     // internal orientation
        float nx,ny,nz; // orientation vector relative to main anchor point ??
        void set(float fx, float fy, float fz, float nx, float ny, float nz) {
            this->fx = fx;
            this->fy = fy;
            this->fz = fz;
            this->nx = nx;
            this->ny = ny;
            this->nz = nz;
        }
        VoxPartRotation(){}
        VoxPartRotation(float fx, float fy, float fz, float nx, float ny, float nz)
        :   fx(fx), fy(fy), fz(fz), nx(nx), ny(ny), nz(nz) {}
};

class VoxPartAnchor {
    public:
        float length;
        float x,y,z;
        void set(float length, float x, float y, float z) {
            this->length = length;
            this->x = x;
            this->y = y;
            this->z = z;
        }
        VoxPartAnchor(){}
        VoxPartAnchor(float length, float x, float y, float z)
        :   length(length), x(x), y(y), z(z) {}
};

class VoxPartDimension {
    public:
        int x,y,z;
        void set(int x, int y, int z) {
            this->x = x;
            this->y = y;
            this->z = z;
        }
        VoxPartDimension(){}
        VoxPartDimension(int x, int y, int z)
        :   x(x), y(y), z(z) {}
};

struct VoxPart {
    public:
        struct VoxPartRotation rotation;
        struct VoxPartAnchor anchor;
        struct VoxPartDimension dimension;
        int part_num;

        void set_rotation(float fx, float fy, float fz, float nx, float ny, float nz) {
            rotation.set(fx, fy, fz, nx, ny, nz);
        }

        void set_anchor(float len, float x, float y, float z) {
            anchor.set(len, x, y, z);
        }

        void set_dimension(int x, int y, int z) {
            dimension.set(x,y,z);
        }
        
        VoxPart(
            float rot_fx, float rot_fy, float rot_fz,
            float rot_nx, float rot_ny, float rot_nz,
            float anc_len, float anc_x, float anc_y, float anc_z,
            int dim_x, int dim_y, int dim_z,
            int part_num
        ):  rotation(rot_fx, rot_fy, rot_fz, rot_nx, rot_ny, rot_nz),
            anchor(anc_len, anc_x, anc_y, anc_z),
            dimension(dim_x, dim_y, dim_z),
            part_num(part_num)
        {}
};

class VoxBody {
    public:
        class VoxPart* vox_part[AGENT_PART_NUM];
        float vox_size;

        void set_part(
            float rot_fx, float rot_fy, float rot_fz,
            float rot_nx, float rot_ny, float rot_nz,
            float anc_len, float anc_x, float anc_y, float anc_z,
            int dim_x, int dim_y, int dim_z,
            int part_num
        ) {
            VoxPart* p = vox_part[part_num];
            if (p==NULL) {
                p = new VoxPart(
                        rot_fx, rot_fy, rot_fz,
                        rot_nx, rot_ny, rot_nz,
                        anc_len, anc_x, anc_y, anc_z,
                        dim_x, dim_y, dim_z,
                        part_num
                    );
                vox_part[part_num] = p;
            } else {
                p->set_rotation(rot_fx, rot_fy, rot_fz, rot_nx, rot_ny, rot_nz);
                p->set_anchor(anc_len, anc_x, anc_y, anc_z);
                p->set_dimension(dim_x, dim_y, dim_z);
                p->part_num = part_num;
            }
        }

        VoxBody() {}
        VoxBody(float vox_size) : vox_size(vox_size) {}

        ~VoxBody() {
            int i;
            for (i=0; i<AGENT_PART_NUM; i++) {
                if (vox_part[i] == NULL) continue;
                delete vox_part[i];
            }
        }
};

extern VoxBody agent_vox_dat;
