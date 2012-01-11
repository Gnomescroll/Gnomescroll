#pragma once

#include <math.h>

#ifdef DC_CLIENT
#include <compat_gl.h>
#endif

#include <c_lib/voxel/common.h>
#include <c_lib/voxel/voxel_render.hpp>
#include <physics/vector.hpp>

const int AGENT_PART_NUM = 6;
#define AGENT_PART_HEAD 0
#define AGENT_PART_TORSO 1
#define AGENT_PART_LARM 2
#define AGENT_PART_RARM 3
#define AGENT_PART_LLEG 4
#define AGENT_PART_RLEG 5

class Agent_state; // forward declaration

class Agent_vox {
    public:
        //float lv,ly,lz; //looking vector
        Agent_state* a;

        Voxel_volume vv;
        Agent_vox(Agent_state* a);
        
        #ifdef DC_CLIENT
        void update();
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
