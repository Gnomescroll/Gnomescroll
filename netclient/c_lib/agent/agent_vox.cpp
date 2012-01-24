#include "agent_vox.hpp"

VoxBody agent_vox_dat;

void Agent_vox::update(VoxBody* vox_dat) {

    float x,y,z,theta,phi;
    x = this->a->s.x;
    y = this->a->s.y;
    z = this->a->s.z;
    theta = this->a->s.theta;
    phi = this->a->s.phi;

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

void Agent_vox::init_parts(VoxBody* vox_dat) {
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
                ClientState::get_team_color(this->a->status.team, &r, &g, &b);
            }

            vv->set_color(ix, iy, iz, r,g,b,a);
        }

        ClientState::voxel_render_list.register_voxel_volume(vv);
        #endif
        if (vv->hitscan)
            STATE::voxel_hitscan_list.register_voxel_volume(vv);
    }
    #ifdef DC_CLIENT
    ClientState::voxel_render_list.update_vertex_buffer_object();
    #endif
}


void Agent_vox::update_team_color(VoxBody* vox_dat)
{
    #ifdef DC_CLIENT
    printf("UPDATE TEAM COLOR\n");
    printf("Team=%d\n", this->a->status.team);
    unsigned char _r,_g,_b;
    ClientState::get_team_color(this->a->status.team, &_r, &_g, &_b);
    printf("%d,%d,%d\n", _r,_g,_b);

    // investigate why team color is 200 on init
    if (this->a->status.team != 1 && this->a->status.team != 2) return; // Have get_team_color return a status

    unsigned char team_r, team_g, team_b;
    ClientState::get_team_color(this->a->status.team, &team_r, &team_g, &team_b);
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
                printf("Set team color\n");
            }

            vv->set_color(ix, iy, iz, r,g,b,a);
        }
    }
    ClientState::voxel_render_list.update_vertex_buffer_object();
    #endif
}

