#include "flag.hpp"

VoxBody flag_vox_dat;

void Flag_vox::init_parts(VoxBody* vox_dat) {
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

            if (vp->colors.team
            && r == vp->colors.team_r
            && g == vp->colors.team_g
            && b == vp->colors.team_b)
            {
                printf("Flag:: setting team color\n");
                ClientState::get_team_color(this->a->team, &r, &g, &b);
                printf("%d %d %d\n", r,g,b);
            }

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

void Flag::set_position(float x, float y, float z) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->vox->update(&flag_vox_dat);
}

Flag::Flag()
:
theta(0), phi(0),
team(0),
type(OBJ_TYPE_FLAG)
{
    this->vox = new Flag_vox(this, &flag_vox_dat);
    #ifdef DC_CLIENT
    this->vox->set_draw(true);
    #endif
}

Flag::~Flag()
{
    if (this->vox != NULL) free(this->vox);
}
