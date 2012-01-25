#include "object_vox.hpp"

#include <c_lib/voxel/voxel_hitscan.hpp>
#include <defines.h>

#ifdef DC_CLIENT
#include <c_lib/voxel/voxel_render.hpp>
// forward decl.
namespace ClientState {
    extern Voxel_render_list voxel_render_list;
    extern Voxel_hitscan_list voxel_hitscan_list;
    int get_team_color(int team, unsigned char *r, unsigned char *g, unsigned char *b);
}
#endif

#ifdef DC_SERVER
// forward decl.
namespace ServerState {
    extern Voxel_hitscan_list voxel_hitscan_list;
}
#endif

void Object_vox::init_parts(VoxBody* vox_dat, int id, int type) {
    // create each vox part from vox_dat conf
    if (this->inited) return;
    this->inited = true;
    int i;
    int x,y,z;
    VoxPart *vp;
    Voxel_volume* vv;
    float size = vox_dat->vox_size;
    for (i=0; i<this->n_parts; i++) {
        vp = vox_dat->vox_part[i];
        x = vp->dimension.x;
        y = vp->dimension.y;
        z = vp->dimension.z;

        vv = &(this->vv[i]);

        vv->init(x,y,z,size);
        vv->set_unit_axis();
        vv->set_hitscan_properties(id, type, i);

        #ifdef DC_CLIENT
        unsigned char r,g,b,a;
        int j;
        int ix,iy,iz;
        if (vp->colors.n != x*y*z) printf("WARNING: vp colors %d != xyz %d\n", vp->colors.n, x*y*z);
        for (j=0; j < vp->colors.n; j++) {
            ix = vp->colors.index[j][0];
            iy = vp->colors.index[j][1];
            iz = vp->colors.index[j][2];
            if (ix >= x || iy >= y || iz >= z) printf("WARNING color index %d,%d,%d is out of dimensions %d,%d,%d\n", ix,iy,iz, x,y,z);

            r = vp->colors.rgba[j][0];
            g = vp->colors.rgba[j][1];
            b = vp->colors.rgba[j][2];
            a = vp->colors.rgba[j][3];
            vv->set_color(ix, iy, iz, r,g,b,a);
        }

        ClientState::voxel_render_list.register_voxel_volume(vv);
        #endif
        if (vv->hitscan)
            STATE::voxel_hitscan_list.register_voxel_volume(vv);
    }
    #ifdef DC_CLIENT
    //ClientState::voxel_render_list.update_vertex_buffer_object();
    #endif
}

void Object_vox::set_draw(bool draw) {
    int i;
    for (i=0; i<this->n_parts; i++) {
        this->vv[i].draw = draw;
    }
}

void Object_vox::set_hitscan(bool hitscan) {
    int i;
    for (i=0; i<this->n_parts; i++) {
        this->vv[i].hitscan = hitscan;
    }
}

void Object_vox::update(VoxBody* vox_dat, float x, float y, float z, float theta, float phi) {

    VoxPart* vp;
    float ax,ay,az;
    int i;
    for (i=0; i<this->n_parts; i++) {
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

void Object_vox::right(Vector* f, float theta) {
    f->x = cos(theta * PI + PI/2);
    f->y = sin(theta * PI + PI/2);
    f->z = 0.0f;
    normalize_vector(f);
}

void Object_vox::forward(Vector* f, float theta) {
    f->x = cos(theta * PI);
    f->y = sin(theta * PI);
    f->z = 0.0f;
    normalize_vector(f);
}

void Object_vox::look(Vector* f, float theta, float phi) {
    f->x = cos(theta * PI) * cos(phi * PI);
    f->y = sin(theta * PI) * cos(phi * PI);
    f->z = sin(phi);
    normalize_vector(f);
}

void Object_vox::look(float f[3], float theta, float phi) {
    f[0] = cos(theta * PI) * cos(phi * PI);
    f[1] = sin(theta * PI) * cos(phi * PI);
    f[2] = sin(phi);
    normalize_vector_f(&f[0], &f[1], &f[2]);
}

void Object_vox::look(double f[3], double theta, double phi) {
    f[0] = cos(theta * PI) * cos(phi * PI);
    f[1] = sin(theta * PI) * cos(phi * PI);
    f[2] = sin(phi);
    double len = sqrt(f[0]*f[0] + f[1]*f[1] + f[2]*f[2]);
    f[0] /= len;
    f[1] /= len;
    f[2] /= len;
}

Object_vox::Object_vox(int num_parts)
:
inited(false)
{
    this->n_parts = num_parts;
    this->vv = new Voxel_volume[num_parts];
}

Object_vox::Object_vox(int num_parts, VoxBody* vox_dat, int id, int type)
:
inited(false)
{
    this->n_parts = num_parts;
    this->vv = new Voxel_volume[num_parts];
    this->init_parts(vox_dat, id, type);
}

Object_vox::Object_vox(int num_parts, VoxBody* vox_dat, int id, int type, int team)
:
inited(false)
{
    this->n_parts = num_parts;
    this->vv = new Voxel_volume[num_parts];
    this->init_parts(vox_dat, id, type, team);
}

Object_vox::~Object_vox() {
    if (this->vv != NULL)
    {
        for (int i=0; i<this->n_parts; i++) {
            #ifdef DC_CLIENT
            ClientState::voxel_render_list.unregister_voxel_volume(&(this->vv[i]));
            #endif
            STATE::voxel_hitscan_list.unregister_voxel_volume(&(this->vv[i]));
        }
        delete[] this->vv;
    }
}

float Object_vox::largest_radius() {
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

void Object_vox::init_parts(VoxBody* vox_dat, int id, int type, int team) {
    // create each vox part from vox_dat conf

    int i;
    int x,y,z;
    VoxPart *vp;
    Voxel_volume* vv;

    float size = vox_dat->vox_size;
    for (i=0; i<this->n_parts; i++) {
        vp = vox_dat->vox_part[i];
        x = vp->dimension.x;
        y = vp->dimension.y;
        z = vp->dimension.z;

        vv = &(this->vv[i]);

        vv->init(x,y,z,size);
        vv->set_unit_axis();
        vv->set_hitscan_properties(id, type, i);

        #ifdef DC_CLIENT
        unsigned char team_r, team_g, team_b;
        int ret = STATE::get_team_color(team, &team_r, &team_g, &team_b);
        if (ret) printf("WARNING:: get_team_color failed.\n");
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

            if (vp->colors.team
            && r == vp->colors.team_r
            && g == vp->colors.team_g
            && b == vp->colors.team_b)
            {
                r = team_r;
                g = team_g;
                b = team_b;
            }

            vv->set_color(ix, iy, iz, r,g,b,a);
        }

        ClientState::voxel_render_list.register_voxel_volume(vv);
        #endif
        if (vv->hitscan)
            STATE::voxel_hitscan_list.register_voxel_volume(vv);
    }
    #ifdef DC_CLIENT
    //ClientState::voxel_render_list.update_vertex_buffer_object();
    #endif
}

void Object_vox::update_team_color(VoxBody* vox_dat, int team)
{
    #ifdef DC_CLIENT
    unsigned char team_r, team_g, team_b;
    int ret = ClientState::get_team_color(team, &team_r, &team_g, &team_b);
    if (ret) return;

    int i;
    VoxPart* vp;
    Voxel_volume* vv;
    for (i=0; i<this->n_parts; i++)
    {
        vp = vox_dat->vox_part[i];
        vv = &(this->vv[i]);

        int j;
        int ix,iy,iz;
        unsigned char r,g,b,a;
        for (j=0; j<vp->colors.n; j++)
        {
            ix = vp->colors.index[j][0];
            iy = vp->colors.index[j][1];
            iz = vp->colors.index[j][2];
            r = vp->colors.rgba[j][0];
            g = vp->colors.rgba[j][1];
            b = vp->colors.rgba[j][2];
            a = vp->colors.rgba[j][3];

            if (vp->colors.team
            && r == vp->colors.team_r
            && g == vp->colors.team_g
            && b == vp->colors.team_b)
            {
                r = team_r;
                g = team_g;
                b = team_b;
            }

            vv->set_color(ix, iy, iz, r,g,b,a);
        }
    }
    #endif
}
