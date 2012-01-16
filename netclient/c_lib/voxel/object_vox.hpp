#pragma once

#include <c_lib/physics/vector.hpp>
#include <c_lib/voxel/voxel_volume.hpp>
#include <c_lib/voxel/voxel_body.hpp>

#ifdef DC_CLIENT
#include <c_lib/voxel/voxel_render.hpp>
#include <c_lib/voxel/voxel_hitscan.hpp>

// forward decl.
namespace ClientState {
    extern Voxel_render_list voxel_render_list;
    extern Voxel_hitscan_list voxel_hitscan_list;
}
#endif

template <class Obj, int NUM_PARTS>
class Object_vox {
    public:
        int n_parts;
        void right(Vector *f, float theta);
        void forward(Vector *f, float theta);
        void look(Vector *f, float theta, float phi);

        Obj* a;

        Voxel_volume vv[NUM_PARTS];

        void init_parts(VoxBody* vox_dat);
        void update(VoxBody* vox_dat);
        void set_draw(bool draw);

        Object_vox(Obj* a, VoxBody* vox_dat);
        ~Object_vox();
};

template <class Obj, int NUM_PARTS>
void Object_vox<Obj, NUM_PARTS>::init_parts(VoxBody* vox_dat) {
    #ifdef DC_CLIENT
    // create each vox part from vox_dat conf
    int i,j;
    unsigned char r,g,b,a;
    int x,y,z;
    int ix,iy,iz;
    VoxPart *vp;
    float size = vox_dat->vox_size;
    for (i=0; i<NUM_PARTS; i++) {
        vp = vox_dat->vox_part[i];
        x = vp->dimension.x;
        y = vp->dimension.y;
        z = vp->dimension.z;

        this->vv[i].init(x,y,z,size);
        this->vv[i].set_unit_axis();

        for (j=0; j<vp->colors.n; j++) {
            ix = vp->colors.index[j][0];
            iy = vp->colors.index[j][1];
            iz = vp->colors.index[j][2];
            r = vp->colors.rgba[j][0];
            g = vp->colors.rgba[j][1];
            b = vp->colors.rgba[j][2];
            a = vp->colors.rgba[j][3];

            this->vv[i].set_color(ix, iy, iz, r,g,b,a);
        }

        ClientState::voxel_render_list.register_voxel_volume(&(this->vv[i]));
    }
    ClientState::voxel_render_list.update_vertex_buffer_object();
    #endif
}

template <class Obj, int NUM_PARTS>
void Object_vox<Obj,NUM_PARTS>::set_draw(bool draw) {
    int i;
    for (i=0; i<NUM_PARTS; i++) {
        this->vv[i].draw = draw;
    }
}

template <class Obj, int NUM_PARTS>
void Object_vox<Obj,NUM_PARTS>::update(VoxBody* vox_dat) {

    float x,y,z,theta,phi;
    x = this->a->x;
    y = this->a->y;
    z = this->a->z;
    theta = this->a->theta;
    phi = this->a->phi;

    struct Vector forward;
    this->forward(&forward, theta);

    Vector up = {0.0f,0.0f,1.0f};

    VoxPart* vp;
    float ax,ay,az;
    int i;
    for (i=0; i<NUM_PARTS; i++) {
        vp = vox_dat->vox_part[i];
        ax = vp->anchor.x;
        ay = vp->anchor.y;
        az = vp->anchor.z;
        this->vv[i].set_center(x+ax,y+ay,z+az); // add vox config offsets

        if (vp->biaxial) {
            this->vv[i].set_rotated_unit_axis(phi, 0.0f, theta);
        } else {
            this->vv[i].set_axis(&forward, &up);
        }
    }
}

template <class Obj, int NUM_PARTS>
void Object_vox<Obj,NUM_PARTS>::right(Vector* f, float theta) {
    f->x = cos(theta * PI + PI/2);
    f->y = sin(theta * PI + PI/2);
    f->z = 0.0f;
    normalize_vector(f);
}

template <class Obj, int NUM_PARTS>
void Object_vox<Obj,NUM_PARTS>::forward(Vector* f, float theta) {
    f->x = cos(theta * PI);
    f->y = sin(theta * PI);
    f->z = 0.0f;
    normalize_vector(f);
}

template <class Obj, int NUM_PARTS>
void Object_vox<Obj,NUM_PARTS>::look(Vector* f, float theta, float phi) {
    f->x = cos(theta * PI) * cos(phi * PI);
    f->y = sin(theta * PI) * cos(phi * PI);
    f->z = sin(phi);
    normalize_vector(f);
}

template <class Obj, int NUM_PARTS>
Object_vox<Obj,NUM_PARTS>::Object_vox(Obj* a, VoxBody* vox_dat)
:
a(a)
{
    this->n_parts = NUM_PARTS;
    this->init_parts(vox_dat);
}

template <class Obj, int NUM_PARTS>
Object_vox<Obj,NUM_PARTS>::~Object_vox() {
    #ifdef DC_CLIENT
    int i;
    for (i=0; i<NUM_PARTS; i++) {
        ClientState::voxel_render_list.unregister_voxel_volume(&(this->vv[i]));
    }
    #endif
}
