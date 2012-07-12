#include "voxel_model.hpp"

#include <common/defines.h>
#include <common/random.h>
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
    vox_skeleton_world_matrix[0] = affine_euler_rotation_and_translation(x,y,z, theta,0.0f,0.0f);
}
void Voxel_model::set_skeleton_root(float *data)
{
    vox_skeleton_world_matrix[0] = affine_euler_rotation_and_translation(
        data[0], data[1], data[2],
        data[3], data[4], data[5]
    );
}

int Voxel_model::get_parent_node_index(int part)
{
    if (part < 0 || part >+ this->n_parts)
    {
        printf("WARNING: Voxel_model::get_parent_node_index -- part %d out of range\n", part);
        return -1;
    }
    VoxPart* vp = vox_dat->vox_part[part];
    if (vp == NULL) return -1;
    return vp->skeleton_parent_matrix;
}

void Voxel_model::set_node_rotation_by_part(int part, float theta, float phi, float rho)
{
    int node = this->get_parent_node_index(part);
    if (node == -1) return;
    this->set_node_rotation(node, theta, phi, rho);
}

void Voxel_model::set_node_rotation(int node, float theta, float phi, float rho)
{
    if (node < 0 || node >= this->n_skeleton_nodes)
    {
        printf("WARNING: Voxel_mode::set_node_rotation -- node %d out of range\n", node);
        return;
    }
    float x,y,z,rx,ry,rz;
    x = vox_dat->vox_skeleton_local_matrix_reference[node][0];
    y = vox_dat->vox_skeleton_local_matrix_reference[node][1];
    z = vox_dat->vox_skeleton_local_matrix_reference[node][2];
    rx = vox_dat->vox_skeleton_local_matrix_reference[node][3];
    ry = vox_dat->vox_skeleton_local_matrix_reference[node][4];
    rz = vox_dat->vox_skeleton_local_matrix_reference[node][5];
    vox_skeleton_local_matrix[node] = affine_euler_rotation_and_translation(x,y,z, rx - theta, ry - phi, rz - rho);
}

void Voxel_model::set_biaxial_nodes(float phi)
{   // in the editor, head and arm rotate the same way
    // here, they rotate opposite directions
    for (int i=0; i<this->n_skeleton_nodes; i++)
    {
        if (!biaxial_nodes[i]) continue;
        this->set_node_rotation(i, 0, phi, 0);
    }
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
            print_affine(vox_skeleton_world_matrix[vox_skeleton_transveral_list[i]]);
            printf("\n");
            print_affine(vox_skeleton_local_matrix[i]);
        }

        vox_skeleton_world_matrix[i] = affine_mult( 
            vox_skeleton_world_matrix[vox_skeleton_transveral_list[i]],  
            vox_skeleton_local_matrix[i]
        );
    }

    for(int i=0; i<this->n_parts; i++)
    {
        Voxel_volume* vv = &this->vv[i];
        vv->world_matrix = affine_mult(*vv->parent_world_matrix, vv->local_matrix);
    }
}

void Voxel_model::draw_skeleton()
{
    #if DC_CLIENT
    glDisable(GL_TEXTURE_2D);
    //;

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

    //;
    #endif
}

void Voxel_model::init_skeleton()
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
    biaxial_nodes = (bool*)calloc(num_skeleton_nodes, sizeof(bool));
    for (int i=0; i<vox_dat->n_parts; i++)
    {
        VoxPart* vp = vox_dat->vox_part[i];
        if (vp == NULL)
            continue;
        biaxial_nodes[vp->skeleton_parent_matrix] = vp->biaxial;
    }

    this->reset_skeleton();
}

// implemented for the voxel editor
void Voxel_model::reset_skeleton()
{
    int num_skeleton_nodes = vox_dat->n_skeleton_nodes;
    skeleton_needs_update = true;
    
    const int debug_0 = 0;

    if(debug_0) printf("Voxel_model::init_skeleton, number of nodes= %i \n", num_skeleton_nodes);

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

void Voxel_model::init_parts(int id, ObjectType type)
{
    // create each vox part from vox_dat conf
    if (this->vox_inited)
    { 
        printf("Voxel_model::init_parts error!! inited twice \n");
        return;
    }
    this->vox_inited = true;
    int x,y,z;
    VoxPart *vp;
    Voxel_volume* vv;
    for (int i=0; i<this->n_parts; i++) {
        vp = vox_dat->vox_part[i];
        x = vp->dimension.x;
        y = vp->dimension.y;
        z = vp->dimension.z;

        vv = &(this->vv[i]);

        vv->init(x,y,z, vp->vox_size);
        vv->set_hitscan_properties(id, type, i);

        //#if DC_CLIENT
        this->set_part_color(i);
        #if DC_CLIENT
        ClientState::voxel_render_list->register_voxel_volume(vv);
        #endif
    }
}

void Voxel_model::set_part_color(int part_num)
{
    //#if DC_CLIENT
    VoxPart *vp = vox_dat->vox_part[part_num];
    Voxel_volume* vv = &(this->vv[part_num]);
    int x,y,z;
    x = vp->dimension.x;
    y = vp->dimension.y;
    z = vp->dimension.z;
    
    unsigned char r,g,b,a;
    int ix,iy,iz;
    if (vp->colors.n != x*y*z) printf("WARNING: vp colors %d != xyz %d\n", vp->colors.n, x*y*z);
    for (int j=0; j < vp->colors.n; j++)
    {
		int k = j * 3;
        ix = vp->colors.index[k+0];
        iy = vp->colors.index[k+1];
        iz = vp->colors.index[k+2];
        if (ix >= x || iy >= y || iz >= z) printf("WARNING color index %d,%d,%d is out of dimensions %d,%d,%d\n", ix,iy,iz, x,y,z);

		k = j * 4;
        r = vp->colors.rgba[k+0];
        g = vp->colors.rgba[k+1];
        b = vp->colors.rgba[k+2];
        a = vp->colors.rgba[k+3];
        vv->set_color(ix, iy, iz, r,g,b,a);
    }
    //#endif
}

void Voxel_model::set_colors()
{
    for (int i=0; i<this->n_parts; i++)
        this->set_part_color(i);
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
        STATE::voxel_hitscan_list->register_voxel_volume(&this->vv[i++]));
}

void Voxel_model::unregister_hitscan()
{
    if (this->vv == NULL)
    {
        printf("ERROR Voxel_model::register_hitscan -- voxel volume array is NULL\n");
        return;
    }
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

Voxel_model::Voxel_model(VoxDat* vox_dat, int id, ObjectType type)
:
skeleton_inited(false),
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
        this->vv = new Voxel_volume[vox_dat->n_parts];
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

    if(skeleton_inited == true)
    {
        delete[] vox_skeleton_transveral_list;
        delete[] vox_skeleton_local_matrix;
        delete[] vox_skeleton_world_matrix;
        free(biaxial_nodes);
    }
    else
        printf("Voxel_model::~Voxel_model, error! skeleton not inited \n");
}

void Voxel_model::freeze()
{
    this->frozen = true;
}

void Voxel_model::thaw()
{
    this->frozen = false;
}

float Voxel_model::largest_radius() {
    float largest = 0.0f;
    if (this->vv == NULL) return largest;
    Voxel_volume* vv;
    for (int i=0; i<this->n_parts; i++)
    {
        vv = &this->vv[i];
        if (vv->radius > largest) largest = vv->radius;
    }
    return largest;
}

Voxel_volume* Voxel_model::get_part(int part)
{
    if (part < 0 || part >= this->n_parts)
    {
        printf("Voxel_model::get_part -- part %d invalid\n", part);
        return NULL;
    }

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
    GS_ASSERT(part < this->n_parts);
    GS_ASSERT(part >= 0);
    if (part >= this->n_parts || part < 0) return 1.0f;
    return this->get_part(part)->radius;
}

Affine* Voxel_model::get_node(int node)
{
    if (node < 0 || node >= this->n_skeleton_nodes)
    {
        printf("Voxel_model::get_node -- part %d invalid\n", node);
        return NULL;
    }
    return &this->vox_skeleton_world_matrix[node];
}

bool Voxel_model::in_sight_of(Vec3 source, Vec3* sink)
{   // ray cast from source to each body part center (shuffled)
    return this->in_sight_of(source, sink, 1.0f);
}

bool Voxel_model::in_sight_of(Vec3 source, Vec3* sink, float acquisition_probability)
{   // ray cast from source to each body part center (shuffled)
    GS_ASSERT(this->n_parts > 0);
    if (this->n_parts <= 0) return false;
    
    MALLOX(int, part_numbers, this->n_parts); //type, name, size

    for (int i=0; i<this->n_parts; i++)
        part_numbers[i] = i;
    shuffle_int_array(part_numbers, this->n_parts);
    
    Voxel_volume* vv;
    Vec3 c;
    for (int i=0; i<this->n_parts; i++)
    {
        int pnum = part_numbers[i];
        GS_ASSERT(pnum >= 0 && pnum < this->n_parts);
        if (pnum < 0 || pnum >= this->n_parts) continue;
        vv = &this->vv[pnum];
        c = vv->get_center(); // ray cast to center of volume
		if (randf() < acquisition_probability) continue;	// TODO -- move this to beginning of loop. leave it here while testing for bug crash
        c = quadrant_translate_position(source, c);
        if (ray_cast_simple(source.x, source.y, source.z, c.x, c.y, c.z))
        {
            *sink = c;
            return true;
        }
    }
    return false;
}
