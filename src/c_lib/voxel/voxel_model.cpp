#include "voxel_model.hpp"

#include <common/defines.h>
#include <common/random.hpp>
#include <physics/affine.hpp>
#include <physics/quadrant.hpp>
#include <voxel/voxel_hitscan.hpp>

#if DC_CLIENT
#include <voxel/voxel_render.hpp>
#include <state/client_state.hpp>
#endif

#if DC_SERVER
#include <state/server_state.hpp>
#endif

//set offset and rotation
void Voxel_model::set_skeleton_root(float x, float y, float z, float theta)
{
    GS_ASSERT(this->skeleton_inited);
    if (!this->skeleton_inited) return;
    vox_skeleton_world_matrix[0] = affine_euler_rotation_and_translation(x,y,z, theta,0.0f,0.0f);
    vox_skeleton_world_matrix[0].c = translate_position(vox_skeleton_world_matrix[0].c);
}

void Voxel_model::set_skeleton_root(float *data)
{
    GS_ASSERT(this->skeleton_inited);
    if (!this->skeleton_inited) return;
    vox_skeleton_world_matrix[0] = affine_euler_rotation_and_translation(
        data[0], data[1], data[2],
        data[3], data[4], data[5]
    );
    vox_skeleton_world_matrix[0].c = translate_position(vox_skeleton_world_matrix[0].c);
}

int Voxel_model::get_parent_node_index(int part)
{
    GS_ASSERT(part >= 0 && part < this->n_parts);
    if (part < 0 || part >= this->n_parts) return -1;
    VoxPart* vp = vox_dat->vox_part[part];
    if (vp == NULL) return -1;
    return vp->skeleton_parent_matrix;
}

void Voxel_model::set_node_rotation_by_part(int part, float theta, float phi, float rho)
{
    GS_ASSERT(this->skeleton_inited);
    if (!this->skeleton_inited) return;
    int node = this->get_parent_node_index(part);
    if (node == -1) return;
    this->set_node_rotation(node, theta, phi, rho);
}

void Voxel_model::set_node_rotation(int node, float theta, float phi, float rho)
{
    GS_ASSERT(this->skeleton_inited);
    if (!this->skeleton_inited) return;
    GS_ASSERT(node >= 0 && node < this->n_skeleton_nodes);
    if (node < 0 || node >= this->n_skeleton_nodes) return;
    
    float x  = vox_dat->vox_skeleton_local_matrix_reference[node][0];
    float y  = vox_dat->vox_skeleton_local_matrix_reference[node][1];
    float z  = vox_dat->vox_skeleton_local_matrix_reference[node][2];
    float rx = vox_dat->vox_skeleton_local_matrix_reference[node][3];
    float ry = vox_dat->vox_skeleton_local_matrix_reference[node][4];
    float rz = vox_dat->vox_skeleton_local_matrix_reference[node][5];
    vox_skeleton_local_matrix[node] = affine_euler_rotation_and_translation(x,y,z, rx - theta, ry - phi, rz - rho);
}

void Voxel_model::set_biaxial_nodes(float phi)
{   // in the editor, head and arm rotate the same way
    // here, they rotate opposite directions
    GS_ASSERT(this->skeleton_inited);
    if (!this->skeleton_inited) return;
    for (int i=0; i<this->n_skeleton_nodes; i++)
    {
        if (!biaxial_nodes[i]) continue;
        this->set_node_rotation(i, 0, phi, 0);
    }
}

void Voxel_model::update_skeleton()
{
    GS_ASSERT(this->skeleton_inited);
    if (!this->skeleton_inited) return;
    const int debug = 0;
    if(debug) printf("update skeleton: %i nodes \n", n_skeleton_nodes);

    for(int i=1; i<n_skeleton_nodes; i++)
    {
        if(debug) 
        {   
            printf("i=%i vox_skeleton_transveral_list[i]= %i \n", i, vox_skeleton_transveral_list[i]);
            print_affine(vox_skeleton_world_matrix[vox_skeleton_transveral_list[i]]);
            printf("\n");
            print_affine(vox_skeleton_local_matrix[i]);
        }

        vox_skeleton_world_matrix[i] = affine_mult( 
            vox_skeleton_world_matrix[vox_skeleton_transveral_list[i]],  
            vox_skeleton_local_matrix[i]
        );
        vox_skeleton_world_matrix[i].c = translate_position(vox_skeleton_world_matrix[i].c);
    }

    for(int i=0; i<this->n_parts; i++)
    {
        VoxelVolume* vv = &this->vv[i];
        vv->world_matrix = affine_mult(*vv->parent_world_matrix, vv->local_matrix);
        vv->world_matrix.c = translate_position(vv->world_matrix.c);
    }
}

void Voxel_model::draw_skeleton()
{
    GS_ASSERT(this->skeleton_inited);
    if (!this->skeleton_inited) return;
    #if DC_CLIENT
    glDisable(GL_TEXTURE_2D);

    glLineWidth(3.0f);

    glBegin(GL_LINES);

    //float x,y,z;

    struct Vec3 v;

    const float len = 0.25f;

    // node matrix orientation
    for(int i=0; i<n_skeleton_nodes; i++)
    {
        struct Affine m = vox_skeleton_world_matrix[i];

        glColor3ub(255, 0, 0);
        v = m.v[3];
        glVertex3f(v.x,v.y,v.z);
        v = vec3_add(m.v[3], vec3_scalar_mult(m.v[0], len));
        glVertex3f(v.x,v.y,v.z);

        glColor3ub(0, 255, 0);
        v = m.v[3];
        glVertex3f(v.x,v.y,v.z);
        v = vec3_add(m.v[3], vec3_scalar_mult(m.v[1], len));
        glVertex3f(v.x,v.y,v.z);

        glColor3ub(0, 0, 255);
        v = m.v[3];
        glVertex3f(v.x,v.y,v.z);
        v = vec3_add(m.v[3], vec3_scalar_mult(m.v[2], len));
        glVertex3f(v.x,v.y,v.z);

    }

    // node->node connection
    for(int i=1; i<n_skeleton_nodes; i++)
    {

        struct Affine m1 = vox_skeleton_world_matrix[vox_skeleton_transveral_list[i]];
        struct Affine m2 = vox_skeleton_world_matrix[i];

        glColor3ub(160, 32, 240);
        v = m1.v[3];
        glVertex3f(v.x,v.y,v.z);
        
        glColor3ub(255, 165, 0);
        v = m2.v[3];
        glVertex3f(v.x,v.y,v.z);
    }


    // part matrix orientation
    for(int i=0; i<this->n_parts; i++)
    {
        struct Affine m = this->vv[i].world_matrix;

        glColor3ub(255, 0, 0);
        v = m.v[3];
        glVertex3f(v.x,v.y,v.z);
        v = vec3_add(m.v[3], vec3_scalar_mult(m.v[0], len));
        glVertex3f(v.x,v.y,v.z);

        glColor3ub(0, 255, 0);
        v = m.v[3];
        glVertex3f(v.x,v.y,v.z);
        v = vec3_add(m.v[3], vec3_scalar_mult(m.v[1], len));
        glVertex3f(v.x,v.y,v.z);

        glColor3ub(0, 0, 255);
        v = m.v[3];
        glVertex3f(v.x,v.y,v.z);
        v = vec3_add(m.v[3], vec3_scalar_mult(m.v[2], len));
        glVertex3f(v.x,v.y,v.z);
    }

    // node->part connection
    for(int i=0; i<this->n_parts; i++)
    {
        struct Affine m1 = *(this->vv[i].parent_world_matrix);
        struct Affine m2 = this->vv[i].world_matrix;

        glColor3ub(0, 0, 0);
        v = m1.v[3];
        glVertex3f(v.x,v.y,v.z);
        
        glColor3ub(255, 255, 255);
        v = m2.v[3];
        glVertex3f(v.x,v.y,v.z);
    }

    glEnd();

    glColor3ub(255, 255, 255);
    glEnable(GL_TEXTURE_2D);
    glLineWidth(1.0f);
    #endif
}

void Voxel_model::init_skeleton()
{
    GS_ASSERT(!this->skeleton_inited);
    if (this->skeleton_inited) return;

    this->n_skeleton_nodes = vox_dat->n_skeleton_nodes;
    GS_ASSERT(this->n_skeleton_nodes > 0);
    if (this->n_skeleton_nodes <= 0) return;

    vox_skeleton_transveral_list = (int*)calloc(this->n_skeleton_nodes, sizeof(int));
    vox_skeleton_local_matrix = (struct Affine*)calloc(this->n_skeleton_nodes, sizeof(struct Affine));
    vox_skeleton_world_matrix = (struct Affine*)calloc(this->n_skeleton_nodes, sizeof(struct Affine));
    
    biaxial_nodes = (bool*)calloc(this->n_skeleton_nodes, sizeof(bool));
    for (int i=0; i<vox_dat->n_parts; i++)
    {
        VoxPart* vp = vox_dat->vox_part[i];
        if (vp == NULL)
            continue;
        biaxial_nodes[vp->skeleton_parent_matrix] = vp->biaxial;
    }

    this->skeleton_inited = true;

    this->reset_skeleton();
}

// implemented for the voxel editor
void Voxel_model::reset_skeleton()
{
    this->n_skeleton_nodes = vox_dat->n_skeleton_nodes;
    this->skeleton_needs_update = true;
    
    for (int i=0; i<this->n_skeleton_nodes; i++)
    {
        vox_skeleton_transveral_list[i] = vox_dat->vox_skeleton_transveral_list[i];
        vox_skeleton_local_matrix[i] = vox_dat->vox_skeleton_local_matrix[i]; //this is zero
    }
    
    //set pointer in voxel volume back to the skeleton parent world matrix 
    for (int i=0; i<this->n_parts; i++)
    {
        class VoxelVolume* vv = &this->vv[i];
        class VoxPart *vp = vox_dat->vox_part[i];
        vv->parent_world_matrix = &vox_skeleton_world_matrix[vp->skeleton_parent_matrix];
        vv->local_matrix = vox_dat->vox_volume_local_matrix[i];
    }
}

void Voxel_model::init_parts(int id, EntityType type)
{
    // create each vox part from vox_dat conf
    GS_ASSERT(!this->vox_inited);
    if (this->vox_inited) return;

    for (int i=0; i<this->n_parts; i++)
    {
        VoxPart* vp = vox_dat->vox_part[i];
        int x = vp->dimension.x;
        int y = vp->dimension.y;
        int z = vp->dimension.z;

        VoxelVolume* vv = &(this->vv[i]);

        vv->init(x,y,z, vp->vox_size);
        vv->set_hitscan_properties(id, type, i);

        this->set_part_color(i);

        #if DC_CLIENT
        ClientState::voxel_render_list->register_voxel_volume(vv);
        #endif
    }
    this->vox_inited = true;
}

void Voxel_model::set_part_color(int part_num)
{
    GS_ASSERT(part_num >= 0 && part_num < this->n_parts);
    if (part_num < 0 || part_num >= this->n_parts) return;
    
    VoxPart *vp = vox_dat->vox_part[part_num];
    VoxelVolume* vv = &(this->vv[part_num]);
    int x = vp->dimension.x;
    int y = vp->dimension.y;
    int z = vp->dimension.z;
    
    unsigned char r,g,b,a;
    int ix,iy,iz;
    GS_ASSERT(vp->colors.n == x*y*z);
    for (int j=0; j < vp->colors.n; j++)
    {
        int k = j * 3;
        ix = vp->colors.index[k+0];
        iy = vp->colors.index[k+1];
        iz = vp->colors.index[k+2];
        GS_ASSERT(ix < x && iy < y && iz < z);

        k = j * 4;
        r = vp->colors.rgba[k+0];
        g = vp->colors.rgba[k+1];
        b = vp->colors.rgba[k+2];
        a = vp->colors.rgba[k+3];
        vv->set_color(ix, iy, iz, r,g,b,a);
    }
}

void Voxel_model::set_colors()
{
    for (int i=0; i<this->n_parts; i++)
        this->set_part_color(i);
}

void Voxel_model::fill_part_color(int part_num, struct Color color)
{
    GS_ASSERT(part_num >= 0 && part_num < this->n_parts);
    if (part_num < 0 || part_num >= this->n_parts) return;
    GS_ASSERT(color.r || color.g || color.b); // 0,0,0 is interpreted as invisible
    GS_ASSERT(color.r != 255 && color.g != 255 && color.b != 255);  // 255 wraps to 0 for whatever reason
    
    VoxPart *vp = vox_dat->vox_part[part_num];
    if (!vp->colorable) return;
    struct Color base_color = vp->base_color;

    VoxelVolume* vv = &(this->vv[part_num]);
    
    int ix,iy,iz;
    unsigned char r,g,b,a;
    for (int i=0; i<vp->colors.n; i++)
    {
        r = vp->colors.rgba[4*i + 0];
        g = vp->colors.rgba[4*i + 1];
        b = vp->colors.rgba[4*i + 2];
        a = vp->colors.rgba[4*i + 3];
        if (base_color.r != r || base_color.b != b || base_color.g != g)
            continue;

        ix = vp->colors.index[3*i + 0];
        iy = vp->colors.index[3*i + 1];
        iz = vp->colors.index[3*i + 2];
        
        vv->set_color(ix,iy,iz, color.r, color.g, color.b, a);
    }
}

void Voxel_model::fill_color(struct Color color)
{
    for (int i=0; i<this->n_parts; i++)
        this->fill_part_color(i, color);
}

void Voxel_model::set_draw(bool draw)
{
    for (int i=0; i<this->n_parts; this->vv[i++].draw = draw);
}

void Voxel_model::register_hitscan()
{
    GS_ASSERT(this->vv != NULL);
    if (this->vv == NULL) return;
    for (int i=0; i<this->n_parts; i++)
        STATE::voxel_hitscan_list->register_voxel_volume(&this->vv[i]);
}

void Voxel_model::unregister_hitscan()
{
    GS_ASSERT(this->vv != NULL);
    if (this->vv == NULL) return;
    for (int i=0; i<this->n_parts;
        STATE::voxel_hitscan_list->unregister_voxel_volume(&this->vv[i++]));
}

void Voxel_model::set_hitscan(bool hitscan)
{
    for (int i=0; i<this->n_parts; this->vv[i++].hitscan = hitscan);
}

void Voxel_model::update(float x, float y, float z, float theta, float phi) 
{
    if (this->frozen) return;
    if (this->was_updated) return;
 
    ASSERT_BOXED_POINTf(x);
    ASSERT_BOXED_POINTf(y);

    this->set_skeleton_root(x,y,z, theta);

    #if DC_CLIENT && !PRODUCTION
    if (!input_state.skeleton_editor)
        this->set_biaxial_nodes(phi);
    #else
    this->set_biaxial_nodes(phi);
    #endif

    this->update_skeleton();

    #if DC_CLIENT
    if (input_state.skeleton_editor)
        this->draw_skeleton();
    #endif
    this->was_updated = true;
}

void Voxel_model::set_vox_dat(VoxDat* vox_dat)
{
    this->vox_dat = vox_dat;
}

Voxel_model::Voxel_model(VoxDat* vox_dat, int id, EntityType type) :
    skeleton_inited(false),
    n_skeleton_nodes(0),
    vox_skeleton_transveral_list(NULL),
    vox_skeleton_local_matrix(NULL),
    vox_skeleton_world_matrix(NULL),
    biaxial_nodes(NULL),
    vox_inited(false),
    was_updated(false),
    frozen(false),
    vv(NULL)
{
    if (vox_dat == NULL) printf("WARNING: ::Voxel_model() ctor -- vox_dat is NULL\n");
    this->set_vox_dat(vox_dat);
    this->n_parts = vox_dat->n_parts;
    GS_ASSERT(this->n_parts > 0);
    if (this->n_parts > 0)
    {
        this->vv = new VoxelVolume[vox_dat->n_parts];
        GS_ASSERT(this->vv != NULL);
    }
    else
        this->n_parts = 0;  // make sure its not negative
    this->init_parts(id, type);
    this->init_skeleton();
}

Voxel_model::~Voxel_model()
{
    if (this->vv != NULL)
    {
        for (int i=0; i<this->n_parts; i++)
        {
            #if DC_CLIENT
            ClientState::voxel_render_list->unregister_voxel_volume(&(this->vv[i]));
            #endif
            STATE::voxel_hitscan_list->unregister_voxel_volume(&(this->vv[i]));
        }
        delete[] this->vv;
    }

    if (this->vox_skeleton_transveral_list != NULL) free(this->vox_skeleton_transveral_list);
    if (this->vox_skeleton_local_matrix != NULL) free(this->vox_skeleton_local_matrix);
    if (this->vox_skeleton_world_matrix != NULL) free(this->vox_skeleton_world_matrix);
    if (this->biaxial_nodes != NULL) free(this->biaxial_nodes);
}

void Voxel_model::freeze()
{
    this->frozen = true;
}

void Voxel_model::thaw()
{
    this->frozen = false;
}

float Voxel_model::largest_radius()
{
    float largest = 0.0f;
    if (this->vv == NULL) return largest;
    VoxelVolume* vv;
    for (int i=0; i<this->n_parts; i++)
    {
        vv = &this->vv[i];
        if (vv->radius > largest) largest = vv->radius;
    }
    return largest;
}

VoxelVolume* Voxel_model::get_part(int part)
{
    GS_ASSERT(part >= 0 && part < this->n_parts);
    if (part < 0 || part >= this->n_parts) return NULL;
    return &this->vv[part];
}

Vec3 Voxel_model::get_center()
{
    GS_ASSERT(this->n_parts > 0);
    if (this->n_parts <= 0) return vec3_init(0,0,0);
    return this->get_part(0)->get_center();
}

Vec3 Voxel_model::get_center(int part)
{
    GS_ASSERT(part < this->n_parts);
    GS_ASSERT(part >= 0);
    if (part >= this->n_parts || part < 0) return vec3_init(0,0,0);
    return this->get_part(part)->get_center();
}

float Voxel_model::get_radius()
{
    GS_ASSERT(this->n_parts > 0);
    if (this->n_parts <= 0) return 1.0f;
    return this->get_part(0)->radius;
}

float Voxel_model::get_radius(int part)
{
    GS_ASSERT(part >= 0 && part < this->n_parts);
    if (part < 0 || part >= this->n_parts) return 1.0f;
    return this->get_part(part)->radius;
}

Affine* Voxel_model::get_node(int node)
{
    GS_ASSERT(node >= 0 && node < this->n_skeleton_nodes);
    if (node < 0 || node >= this->n_skeleton_nodes) return NULL;
    return &this->vox_skeleton_world_matrix[node];
}

bool Voxel_model::in_sight_of(Vec3 source, Vec3* sink)
{   // ray cast from source to each body part center (shuffled)
    return this->in_sight_of(source, sink, 0.0f);
}

bool Voxel_model::in_sight_of(Vec3 source, Vec3* sink, float failure_rate)
{   // ray cast from source to each body part center (shuffled)
    GS_ASSERT(failure_rate >= 0.0f && failure_rate < 1.0f);
    GS_ASSERT(this->n_parts > 0);
    if (this->n_parts <= 0) return false;

    MALLOX(int, part_numbers, this->n_parts); //type, name, size

    for (int i=0; i<this->n_parts; i++)
        part_numbers[i] = i;
    shuffle<int>(part_numbers, this->n_parts);
    
    Vec3 c;
    for (int i=0; i<this->n_parts; i++)
    {
        if (randf() < failure_rate) continue;
        int pnum = part_numbers[i];
        GS_ASSERT(pnum >= 0 && pnum < this->n_parts);
        if (pnum < 0 || pnum >= this->n_parts) continue;
        c = this->vv[pnum].get_center(); // ray cast to center of volume
        c = quadrant_translate_position(source, c);
        if (!raytrace_terrain(source, c))
        {
            *sink = c;
            return true;
        }
    }
    return false;
}
