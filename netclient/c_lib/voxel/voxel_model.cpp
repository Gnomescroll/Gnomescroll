#include "voxel_model.hpp"

#include <physics/affine.hpp>

#include <c_lib/voxel/voxel_hitscan.hpp>
#include <defines.h>

// forward declarations
#ifdef DC_CLIENT
#include <c_lib/voxel/voxel_render.hpp>
namespace ClientState {
    extern Voxel_render_list voxel_render_list;
    extern Voxel_hitscan_list voxel_hitscan_list;
    int get_team_color(int team, unsigned char *r, unsigned char *g, unsigned char *b);
}
#endif
#ifdef DC_SERVER
namespace ServerState {
    extern Voxel_hitscan_list voxel_hitscan_list;
}
#endif

//set offset and rotation
void Voxel_model::set_skeleton_root(float x, float y, float z, float theta)
{
    vox_skeleton_world_matrix[0] = affine_euler_rotation_and_translation(x,y,z, theta,0.0,0.0);
}
void Voxel_model::set_skeleton_root(float *data)
{
    vox_skeleton_world_matrix[0] = affine_euler_rotation_and_translation(
        data[0], data[1], data[2],
        data[3], data[4], data[5]
    );
}

void Voxel_model::update_skeleton()
{
    const int debug = 0;
    if(debug) printf("update skeleton: %i nodes \n", n_skeleton_nodes);

    for(int i=1; i<n_skeleton_nodes; i++)
    {
        if(debug) 
        {   
            printf("i=%i vox_skeleton_transveral_list[i]= %i \n", i, vox_skeleton_transveral_list[i]);
            print_affine( vox_skeleton_world_matrix[vox_skeleton_transveral_list[i]] );
            printf("\n");
            print_affine( vox_skeleton_local_matrix[i] );
        }

        vox_skeleton_world_matrix[i] = affine_mult( 
            vox_skeleton_world_matrix[vox_skeleton_transveral_list[i]],  
            vox_skeleton_local_matrix[i]
        );
    }

    for(int i=0; i<this->n_parts; i++)
    {
        Voxel_volume* vv = &this->vv[i];
        vv->world_matrix = affine_mult( *vv->parent_world_matrix, vv->local_matrix );
    }

}


void Voxel_model::draw_skeleton()
{
#ifdef DC_CLIENT


    for(int i=1; i<n_skeleton_nodes; i++)
    {

        struct Affine m = vox_skeleton_world_matrix[i];

        
    }
#endif
}


void Voxel_model::init_skeleton(VoxDat* vox_dat)
{
    if( skeleton_inited == true )
    {
        printf("Voxel_model::init_skeleton error!! inited twice \n");
        return;
    }
    skeleton_inited = true;

    n_skeleton_nodes = vox_dat->n_skeleton_nodes;

    int num_skeleton_nodes = vox_dat->n_skeleton_nodes;

    vox_skeleton_transveral_list = new int[num_skeleton_nodes];
    vox_skeleton_local_matrix = new Affine[num_skeleton_nodes];
    vox_skeleton_world_matrix = new Affine[num_skeleton_nodes];

    this->reset_skeleton(vox_dat);
}

// implemented for the voxel editor
void Voxel_model::reset_skeleton(VoxDat* vox_dat)
{
    int num_skeleton_nodes = vox_dat->n_skeleton_nodes;
    skeleton_needs_update = true;
    
    const int debug_0 = 0;

    if(debug_0) printf("Voxel_model::init_skeleton, number of nodes= %i \n", num_skeleton_nodes );

    for(int i=0; i<num_skeleton_nodes; i++)
    {
        vox_skeleton_transveral_list[i] = vox_dat->vox_skeleton_transveral_list[i];
        vox_skeleton_local_matrix[i] = vox_dat->vox_skeleton_local_matrix[i]; //this is zero

        if( debug_0 )
        {
            printf("vox_skeleton_local_matrix[%i]= \n", i);
            print_affine( vox_skeleton_local_matrix[i] );
            printf("vox_dat->vox_skeleton_local_matrix[%i]= \n", i);
            print_affine( vox_dat->vox_skeleton_local_matrix[i] );
        }
    }

    const int debug_1 = 0;

    if(debug_1) printf("Voxel_model::init_skeleton, number of volumes= %i \n", this->n_parts );
    //set pointer in voxel volume back to the skeleton parent world matrix 
    for(int i=0; i<this->n_parts; i++)
    {
        class Voxel_volume* vv = &this->vv[i];
        class VoxPart *vp = vox_dat->vox_part[i];

        vv->parent_world_matrix = &vox_skeleton_world_matrix[vp->skeleton_parent_matrix];
        vv->local_matrix = vox_dat->vox_volume_local_matrix[i];

        if( debug_1 )
        {
            printf("voxel volume %i local matrix= \n", i);
            print_affine( vv->local_matrix );
        }
    }

}

void Voxel_model::init_parts(VoxDat* vox_dat, int id, int type) {
    // create each vox part from vox_dat conf
    if (this->vox_inited)
    { 
        printf("Voxel_model::init_parts error!! inited twice \n");
        return;
    }
    this->vox_inited = true;
    int i;
    int x,y,z;
    VoxPart *vp;
    Voxel_volume* vv;
    for (i=0; i<this->n_parts; i++) {
        vp = vox_dat->vox_part[i];
        x = vp->dimension.x;
        y = vp->dimension.y;
        z = vp->dimension.z;

        vv = &(this->vv[i]);

        vv->init(x,y,z, vp->vox_size);
        vv->set_hitscan_properties(id, type, i);

        #ifdef DC_CLIENT
        this->set_part_color(vox_dat, i);
        ClientState::voxel_render_list.register_voxel_volume(vv);
        #endif
    }
}

void Voxel_model::update_team_color(VoxDat* vox_dat, int team)
{
    #ifdef DC_CLIENT
    unsigned char team_r, team_g, team_b;
    int ret = ClientState::ctf.get_team_color(team, &team_r, &team_g, &team_b);
    if (ret) return;
    for (int i=0; i<this->n_parts; i++)
        this->set_part_team_color(vox_dat, i, team_r, team_g, team_b);
    #endif
}

void Voxel_model::set_part_team_color(VoxDat* vox_dat, int part_num, unsigned char team_r, unsigned char team_g, unsigned char team_b)
{   // VERIFIED
    #ifdef DC_CLIENT
    VoxPart *vp = vox_dat->vox_part[part_num];
    // if team base color is 0,0,0 abort. this means team base color was not set. 0,0,0 is reserved for empty voxels
    if (vp->colors.team_r == 0
      && vp->colors.team_g == 0
      && vp->colors.team_g == 0)
          return;
          
    Voxel_volume* vv = &(this->vv[part_num]);
    int x,y,z;
    x = vp->dimension.x;
    y = vp->dimension.y;
    z = vp->dimension.z;
    
    unsigned char r,g,b,a;
    int ix,iy,iz;
    if (vp->colors.n != x*y*z) printf("WARNING: vp colors %d != xyz %d\n", vp->colors.n, x*y*z);
    for (int j=0; j < vp->colors.n; j++) {
        ix = vp->colors.index[j][0];
        iy = vp->colors.index[j][1];
        iz = vp->colors.index[j][2];
        if (ix >= x || iy >= y || iz >= z) printf("WARNING color index %d,%d,%d is out of dimensions %d,%d,%d\n", ix,iy,iz, x,y,z);

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
    #endif

}

void Voxel_model::set_part_color(VoxDat* vox_dat, int part_num)
{
    #ifdef DC_CLIENT
    VoxPart *vp = vox_dat->vox_part[part_num];
    Voxel_volume* vv = &(this->vv[part_num]);
    int x,y,z;
    x = vp->dimension.x;
    y = vp->dimension.y;
    z = vp->dimension.z;
    
    unsigned char r,g,b,a;
    int ix,iy,iz;
    if (vp->colors.n != x*y*z) printf("WARNING: vp colors %d != xyz %d\n", vp->colors.n, x*y*z);
    for (int j=0; j < vp->colors.n; j++) {
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
    #endif
}

void Voxel_model::set_colors(VoxDat* vox_dat)
{
    for (int i=0; i<this->n_parts; i++)
        this->set_part_color(vox_dat, i);
}

void Voxel_model::set_draw(bool draw) {
    for (int i=0; i<this->n_parts; this->vv[i++].draw = draw);
}

void Voxel_model::register_hitscan()
{
    if (this->vv == NULL)
    {
        printf("ERROR Voxel_model::register_hitscan -- voxel volume array is NULL\n");
        return;
    }
    for (int i=0; i<this->n_parts;
        STATE::voxel_hitscan_list.register_voxel_volume(&this->vv[i++]));
}

void Voxel_model::set_hitscan(bool hitscan) {
    for (int i=0; i<this->n_parts; this->vv[i++].hitscan = hitscan);
}

void Voxel_model::update(float x, float y, float z, float theta, float phi) 
{
    this->set_skeleton_root(x,y,z, theta);
    this->update_skeleton();
}

Voxel_model::Voxel_model(VoxDat* vox_dat, int id, int type)
:
skeleton_inited(false),
vox_inited(false)
{
    this->n_parts = vox_dat->n_parts;
    this->vv = new Voxel_volume[vox_dat->n_parts];
    this->init_parts(vox_dat, id, type);
    this->init_skeleton(vox_dat);
}

Voxel_model::Voxel_model(VoxDat* vox_dat, int id, int type, int team)
:
skeleton_inited(false),
vox_inited(false)
{
    this->n_parts = vox_dat->n_parts;
    this->vv = new Voxel_volume[vox_dat->n_parts];
    this->init_parts(vox_dat, id, type);
    this->update_team_color(vox_dat, team);
    this->init_skeleton(vox_dat);
}

Voxel_model::~Voxel_model() {
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

    if(skeleton_inited == true)
    {
        delete[] vox_skeleton_transveral_list;
        delete[] vox_skeleton_local_matrix;
        delete[] vox_skeleton_world_matrix;
    }
    else
    {
        printf("Voxel_model::~Voxel_model, error! skeleton not inited \n");
    }
}

float Voxel_model::largest_radius() {
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

Voxel_volume* Voxel_model::get_part(int part)
{
    if (part >= this->n_parts)
    {
        printf("Voxel_model::get_part -- part %d invalid\n", part);
        return NULL;
    }

    return &this->vv[part];
}

// restores voxels to starting state
void Voxel_model::restore(VoxDat* vox_dat, int team)
{
    this->set_colors(vox_dat);
    if (team >= 0)
        this->update_team_color(vox_dat, team);
}