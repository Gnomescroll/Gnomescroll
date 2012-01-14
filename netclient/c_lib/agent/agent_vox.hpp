#pragma once

#include <math.h>

#ifdef DC_CLIENT
#include <compat_gl.h>
#endif

#include <c_lib/voxel/common.h>
#include <c_lib/voxel/voxel_render.hpp>
#include <physics/vector.hpp>

const int AGENT_PART_NUM = 6;
enum AGENT_BODY_PARTS {
    AGENT_PART_HEAD,
    AGENT_PART_TORSO,
    AGENT_PART_LARM,
    AGENT_PART_RARM,
    AGENT_PART_LLEG,
    AGENT_PART_RLEG
};

class Agent_state; // forward declaration

class Agent_vox {
    public:
        void right(Vector *f, float theta);
        void forward(Vector *f, float theta);
        void look(Vector *f, float theta, float phi);

        Agent_state* a;

        Voxel_volume vv[AGENT_PART_NUM];

        void init_parts();
        void update();
        void set_draw(bool draw);

        Agent_vox(Agent_state* a);
        ~Agent_vox();
};


/* Dat storage */

class VoxColors {
    public:
        unsigned char** rgba;
        int n;
        void init(int n) {
            if (this->rgba != NULL) return;
            this->n = n;
            this->rgba = (unsigned char**)malloc(sizeof(unsigned char)*n*4);
            int i;
            for (i=0; i<n; i++) {
                this->rgba[i] = (unsigned char*)malloc(sizeof(unsigned char)*4);
                this->rgba[i][0] = 0;
                this->rgba[i][1] = 0;
                this->rgba[i][2] = 0;
                this->rgba[i][3] = 0;                
            }
        }
        void set(int i, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
        {
            if (i >= this->n)
            {
                printf("VoxColors::set -- index %d exceeds voxel count %d\n", i, this->n);
                return;
            }

            if (this->rgba == NULL) return;
            
            this->rgba[i][0] = r;
            this->rgba[i][1] = g;
            this->rgba[i][2] = b;
            this->rgba[i][3] = a;
        }
        VoxColors()
        : rgba(NULL), n(0)
        {}
        ~VoxColors() {
            if (this->rgba == NULL) return;
            if (this->n <= 0) return;
            
            int i;
            for (i=0; i<this->n; i++) {
                if (this->rgba[i] == NULL) continue;
                free(this->rgba[i]);
            }
            free(this->rgba);
        }
};

class VoxPartRotation {
    public:
        float fx,fy,fz;     // internal orientation
        float nx,ny,nz; // orientation vector relative to main anchor point ??
        void set(float fx, float fy, float fz, float nx, float ny, float nz) {
            // normalize
            float len;
            len = sqrt(fx*fx + fy*fy + fz*fz);
            fx /= len;
            fy /= len;
            fz /= len;
            len = sqrt(nx*nx + ny*ny + nz*nz);
            nx /= len;
            ny /= len;
            nz /= len;
            
            this->fx = fx;
            this->fy = fy;
            this->fz = fz;
            this->nx = nx;
            this->ny = ny;
            this->nz = nz;
        }
        VoxPartRotation(){}
        VoxPartRotation(float fx, float fy, float fz, float nx, float ny, float nz)
        {
            this->set(fx,fy,fz,nx,ny,nz);
        }
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
        int count() {
            return this->x * this->y * this->z;
        }
        VoxPartDimension(){}
        VoxPartDimension(int x, int y, int z)
        :   x(x), y(y), z(z) {}
};

struct VoxPart {
    public:
        VoxPartRotation rotation;
        VoxPartAnchor anchor;
        VoxPartDimension dimension;
        VoxColors colors;
        
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
        {
            colors.init(dim_x*dim_y*dim_z);
        }
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
                p->colors.init(dim_x*dim_y*dim_z);
                p->part_num = part_num;
            }
        }

        void set_color(int part, int x, int y, int z, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
        {
            VoxPart* p = vox_part[part];
            if (p==NULL) return;
            int i = x + y*p->dimension.x + z*p->dimension.y*p->dimension.x;
            p->colors.set(i, r,g,b,a);
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
