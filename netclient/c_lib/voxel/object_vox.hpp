#pragma once

#include <c_lib/physics/vector.hpp>
#include <c_lib/voxel/voxel_volume.hpp>
#include <c_lib/voxel/voxel_body.hpp>

#include <c_lib/voxel/voxel_hitscan.hpp>
#include <defines.h>

#ifdef DC_CLIENT
#include <c_lib/voxel/voxel_render.hpp>
// forward decl.
namespace ClientState {
    extern Voxel_render_list voxel_render_list;
    extern Voxel_hitscan_list voxel_hitscan_list;
}
#endif

#ifdef DC_SERVER
// forward decl.
namespace ServerState {
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
        void look(float f[3], float theta, float phi);
        void look(double f[3], double theta, double phi);

        float largest_radius(); // temporary, until real body collision detection

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
    // create each vox part from vox_dat conf
    int i;
    int x,y,z;
    VoxPart *vp;
    Voxel_volume* vv;
    float size = vox_dat->vox_size;
    for (i=0; i<NUM_PARTS; i++) {
        vp = vox_dat->vox_part[i];
        x = vp->dimension.x;
        y = vp->dimension.y;
        z = vp->dimension.z;

        vv = &(this->vv[i]);

        vv->init(x,y,z,size);
        vv->set_unit_axis();
        vv->set_hitscan_properties(this->a->id, this->a->type, i);

        #ifdef DC_CLIENT
        unsigned char r,g,b,a;
        int j;
        int ix,iy,iz;
        for (j=0; j<vp->colors.n; j++) {
            ix = vp->colors.index[j][0];
            iy = vp->colors.index[j][1];
            iz = vp->colors.index[j][2];
            r = vp->colors.rgba[j][0];
            g = vp->colors.rgba[j][1];
            b = vp->colors.rgba[j][2];
            a = vp->colors.rgba[j][3];

            vv->set_color(ix, iy, iz, r,g,b,a);
        }

        ClientState::voxel_render_list.register_voxel_volume(vv);
        #endif
        STATE::voxel_hitscan_list.register_voxel_volume(vv);
    }
    #ifdef DC_CLIENT
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
            this->vv[i].set_rotated_unit_axis(theta, phi, 0.0f);
        } else {
            this->vv[i].set_rotated_unit_axis(theta, 0.0f, 0.0f);
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
void Object_vox<Obj,NUM_PARTS>::look(float f[3], float theta, float phi) {
    f[0] = cos(theta * PI) * cos(phi * PI);
    f[1] = sin(theta * PI) * cos(phi * PI);
    f[2] = sin(phi);
    normalize_vector_f(&f[0], &f[1], &f[2]);
}

template <class Obj, int NUM_PARTS>
void Object_vox<Obj,NUM_PARTS>::look(double f[3], double theta, double phi) {
    f[0] = cos(theta * PI) * cos(phi * PI);
    f[1] = sin(theta * PI) * cos(phi * PI);
    f[2] = sin(phi);
    double len = sqrt(f[0]*f[0] + f[1]*f[1] + f[2]*f[2]);
    f[0] /= len;
    f[1] /= len;
    f[2] /= len;
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
    int i;
    for (i=0; i<NUM_PARTS; i++) {
        #ifdef DC_CLIENT
        ClientState::voxel_render_list.unregister_voxel_volume(&(this->vv[i]));
        #endif
        STATE::voxel_hitscan_list.unregister_voxel_volume(&(this->vv[i]));
    }
}

template <class Obj, int NUM_PARTS>
float Object_vox<Obj,NUM_PARTS>::largest_radius() {
    float largest = 0.0f;
    if (this->vv == NULL) return largest;
    int i;
    Voxel_volume* vv;
    for (i=0; i<this->n_parts; i++) {
        vv = &this->vv[i];
        if (vv->radius > largest) largest = vv->radius;
    }
    return largest;
}
