#include "voxel_dat.hpp"

namespace Voxels
{

/* *
 * Dat storage
 * */

/* Color */

//#if DC_CLIENT
void VoxColors::init(int dx, int dy, int dz)
{
    GS_ASSERT(this->rgba == NULL);
    if (this->rgba != NULL) return;

    this->n = dx*dy*dz;
    this->rgba = (unsigned char*)malloc(sizeof(unsigned char)*this->n*4);
    this->index = (int*)malloc(sizeof(int)*this->n*3);
    int x=0,y=0,z=0;
    for (int i=0; i<n; i++)
    {
        int rgba_index = i*4;
        this->rgba[rgba_index+0] = 0;
        this->rgba[rgba_index+1] = 0;
        this->rgba[rgba_index+2] = 0;
        this->rgba[rgba_index+3] = 0;

        int index_index = i * 3;
        this->index[index_index+0] = x;
        this->index[index_index+1] = y;
        this->index[index_index+2] = z;

        x++;
        if (x == dx)
        {
            y++;
            if (y == dy)
                z++;
        }
        x %= dx;
        y %= dy;
        z %= dz;
    }
}

void VoxColors::set(int i, int x, int y, int z, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    GS_ASSERT(i < this->n);
    if (i >= this->n) return;
    GS_ASSERT(this->rgba != NULL);
    if (this->rgba == NULL) return;
    GS_ASSERT(this->index != NULL);
    if (this->index == NULL) return;

    int j = i*4;
    this->rgba[j+0] = r;
    this->rgba[j+1] = g;
    this->rgba[j+2] = b;
    this->rgba[j+3] = a;

    j = i * 3;
    this->index[j+0] = x;
    this->index[j+1] = y;
    this->index[j+2] = z;
}

VoxColors::VoxColors()
:
rgba(NULL), index(NULL), n(0)
{}

VoxColors::~VoxColors()
{
    if (this->rgba != NULL)
        free(this->rgba);
    if (this->index != NULL)
        free(this->index);
}
//#endif

/* Dimensions */

void VoxPartDimension::set(int x, int y, int z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}
int VoxPartDimension::count()
{
    return this->x * this->y * this->z;
}

VoxPartDimension::VoxPartDimension()
{}
VoxPartDimension::VoxPartDimension(int x, int y, int z)
:
x(x), y(y), z(z)
{}

/* Body Part (wraps properties) */

void VoxPart::set_dimension(int x, int y, int z)
{
    dimension.set(x,y,z);
}

void VoxPart::set_filename(const char *filename)
{
    int len = (int)strlen(filename);
    this->filename = (char*)realloc(this->filename, sizeof(char) * (len+1));
    strcpy(this->filename, filename);
}


VoxPart::VoxPart(
    VoxDat* dat,
    int part_num,
    float vox_size,
    int dimension_x, int dimension_y, int dimension_z,
    const char* filename,
    bool biaxial)
  : dimension(dimension_x, dimension_y, dimension_z),
    dat(dat),
    part_num(part_num),
    vox_size(vox_size),
    biaxial(biaxial),
    colorable(false)
{
    this->base_color.r = 1;
    this->base_color.g = 1;
    this->base_color.b = 1; // dont use 0,0,0 its reserved
    colors.init(dimension_x, dimension_y, dimension_z);
    int len = (int)strlen(filename);
    this->filename = (char*)malloc(sizeof(char) * (len + 1));
    strcpy(this->filename, filename);
}

VoxPart::~VoxPart()
{
    free(this->filename);
}

// uses cached x,y,z,rx,ry,rz values
void VoxPart::set_local_matrix()
{
    this->dat->vox_volume_local_matrix[part_num] =
        affine_euler_rotation_and_translation(this->sx, this->sy,
                                              this->sz, this->srx,
                                              this->sry, this->srz);
}


/* Skeleton Nodes */

void VoxDat::init_skeleton(int n_skeleton)
{
    GS_ASSERT(!this->voxel_skeleton_inited)
    if (this->voxel_skeleton_inited) return;
    voxel_skeleton_inited = true;

    this->n_skeleton_nodes = n_skeleton;
    vox_skeleton_local_matrix = new Affine[this->n_skeleton_nodes];
    vox_skeleton_local_matrix_reference = (float**)malloc(sizeof(float*) * this->n_skeleton_nodes);
    for (int i=0; i<this->n_skeleton_nodes; i++)
        vox_skeleton_local_matrix_reference[i] = (float*)malloc(sizeof(float) * 6); // x,y,z,rx,ry,rz
    vox_skeleton_transveral_list = new int[n_skeleton];
}

void VoxDat::reset_skeleton_local_matrix(int node)
{
    GS_ASSERT(this->voxel_skeleton_inited)
    if (!this->voxel_skeleton_inited) return;

    GS_ASSERT(node >= 0 && node < this->n_skeleton_nodes);
    if (node < 0 || node >= this->n_skeleton_nodes) return;

    float x,y,z,rx,ry,rz;
    x = vox_skeleton_local_matrix_reference[node][0];
    y = vox_skeleton_local_matrix_reference[node][1];
    z = vox_skeleton_local_matrix_reference[node][2];
    rx = vox_skeleton_local_matrix_reference[node][3];
    ry = vox_skeleton_local_matrix_reference[node][4];
    rz = vox_skeleton_local_matrix_reference[node][5];
    vox_skeleton_local_matrix[node] = affine_euler_rotation_and_translation(x,y,z, rx,ry,rz);
}

void VoxDat::set_skeleton_local_matrix(int node, float x, float y, float z, float rx, float ry, float rz)
{
    GS_ASSERT(this->voxel_skeleton_inited)
    if (!this->voxel_skeleton_inited) return;

    GS_ASSERT(node >= 0 && node < this->n_skeleton_nodes);
    if (node < 0 || node >= this->n_skeleton_nodes) return;

    vox_skeleton_local_matrix[node] = affine_euler_rotation_and_translation(x,y,z, rx,ry,rz);
    vox_skeleton_local_matrix_reference[node][0] = x;
    vox_skeleton_local_matrix_reference[node][1] = y;
    vox_skeleton_local_matrix_reference[node][2] = z;
    vox_skeleton_local_matrix_reference[node][3] = rx;
    vox_skeleton_local_matrix_reference[node][4] = ry;
    vox_skeleton_local_matrix_reference[node][5] = rz;
}

void VoxDat::set_skeleton_node_parent(int node, int parent)
{
    GS_ASSERT(this->voxel_skeleton_inited)
    if (!this->voxel_skeleton_inited) return;

    GS_ASSERT(node >= 0 && node < this->n_skeleton_nodes);
    if (node < 0 || node >= this->n_skeleton_nodes) return;

    vox_skeleton_transveral_list[node] = parent;
}

/* Body (contains parts) */

void VoxDat::init_parts(int n_parts)
{
    if (voxel_volume_inited)
    {
        printf("WARNING VoxDat::init_part -- called more than once\n");
        return;
    }
    voxel_volume_inited = true;

    this->n_parts = n_parts;
    vox_part = new VoxPart*[n_parts]; //array of pointers

    this->vox_volume_local_matrix = new Affine[n_parts];

    for (int i=0; i<n_parts; vox_part[i++] = NULL);
}

void VoxDat::set_part_properties(
    int part_num,
    float vox_size,
    int dimension_x, int dimension_y, int dimension_z,
    const char* filename,
    bool biaxial)
{
    if (!voxel_volume_inited) printf("ERROR WARNING: VoxDat not inited\n");
    VoxPart* p = vox_part[part_num];
    if (p == NULL)
    {
        p = new VoxPart(this,
                        part_num,
                        vox_size,
                        dimension_x, dimension_y, dimension_z,
                        filename,
                        biaxial);
        vox_part[part_num] = p;
    }
    else
    {
        p->dat = this;
        p->part_num = part_num;
        p->vox_size = vox_size;
        p->set_dimension(dimension_x, dimension_y, dimension_z);
        p->colors.init(dimension_x, dimension_y, dimension_z);
        p->biaxial = biaxial;
        p->set_filename(filename);
    }
}

//anchor x,y,z then rotation x,y,z
void VoxDat::set_part_local_matrix(int part_num, float x, float y, float z, float rx, float ry, float rz)
{
    if (!voxel_volume_inited) printf("ERROR WARNING: VoxDat not inited\n");
    VoxPart* p = vox_part[part_num];
    if (p == NULL)
    {
        printf("ERROR VoxDat::set_part_spatials -- part %d is NULL. Abort\n", part_num);
        return;
    }
    this->vox_volume_local_matrix[part_num] =  affine_euler_rotation_and_translation(x,y,z, rx,ry,rz);

    p->sx = x;
    p->sy = y;
    p->sz = z;
    p->srx = rx;
    p->sry = ry;
    p->srz = rz;
}

void VoxDat::set_colorable(int part, bool colorable)
{
    GS_ASSERT(part >= 0 && part < this->n_parts);
    if (part < 0 || part >= this->n_parts) return;
    VoxPart* p = vox_part[part];
    GS_ASSERT(p != NULL);
    if (p == NULL) return;
    p->colorable = colorable;
    GS_ASSERT(!p->colorable || // dont use base color 0,0,0 and say its colorable. undesired effect
        (p->base_color.r || p->base_color.g || p->base_color.b));
}

void VoxDat::set_base_color(int part, unsigned char r, unsigned char g, unsigned char b)
{
    GS_ASSERT(part >= 0 && part < this->n_parts);
    if (part < 0 || part >= this->n_parts) return;
    VoxPart* p = vox_part[part];
    GS_ASSERT(p != NULL);
    if (p == NULL) return;
    p->base_color.r = r;
    p->base_color.g = g;
    p->base_color.b = b;
    GS_ASSERT(!p->colorable || (r || g || b)); // dont use base color 0,0,0 and say its colorable. undesired effect
}

void VoxDat::set_skeleton_parent_matrix(int part, int parent)
{
    VoxPart* vp = this->vox_part[part];
    if (vp == NULL)
    {
        printf("VoxDat::set_skeleton_parent_matrix ERROR!! vp is null, wrong init order \n");
        return;
    }
    vp->skeleton_parent_matrix = parent;
}

void VoxDat::set_color(int part, int x, int y, int z, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    VoxPart* p = vox_part[part];
    if (p==NULL)
    {
        printf("WARNING VoxDat::set_color -- part %d is NULL\n", part);
        return;
    }
    int i = x + y*p->dimension.x + z*p->dimension.y*p->dimension.x;
    p->colors.set(i, x,y,z, r,g,b,a);
}

VoxDat::VoxDat()
:
voxel_volume_inited(false),
n_parts(0),
vox_part(NULL),
vox_volume_local_matrix(NULL),
vox_skeleton_local_matrix(NULL),
vox_skeleton_local_matrix_reference(NULL)
{}

VoxDat::~VoxDat()
{
    if (this->vox_part != NULL)
    {
        for (int i=0; i<n_parts; i++)
            if (this->vox_part[i] != NULL)
                delete vox_part[i];
        delete[] vox_part;
    }
    if (this->vox_skeleton_local_matrix != NULL)
        delete[] this->vox_skeleton_local_matrix;
    if (this->vox_volume_local_matrix != NULL)
        delete[] this->vox_volume_local_matrix;
    if (this->vox_skeleton_local_matrix_reference != NULL)
    {
        for (int i=0; i<this->n_skeleton_nodes; i++)
            if (this->vox_skeleton_local_matrix_reference[i] != NULL)
                free(this->vox_skeleton_local_matrix_reference[i]);
        free(this->vox_skeleton_local_matrix_reference);
    }
}

void VoxDat::save(char* fn)
{
    GS_ASSERT(fn != NULL);
    if (fn == NULL) return;

    FILE* f = fopen(fn, "w");
    GS_ASSERT(f != NULL);
    if (f == NULL) return;

    fprintf(f, "#skeleton nodes, voxel volumes\n");
    fprintf(f, "%d %d\n", this->n_skeleton_nodes, this->n_parts);

    fprintf(f, "#connection matrix\n");
    for (int i=0; i<this->n_skeleton_nodes; i++)
        fprintf(f, "%d %d\n", i, this->vox_skeleton_transveral_list[i]);

    fprintf(f, "#skeleton_node: anchor x,y,z rotatation x,y,z\n");
    for (int i=0; i<this->n_skeleton_nodes; i++)
        fprintf(f, "%d %f %f %f  %f %f %f\n", i,
                this->vox_skeleton_local_matrix_reference[i][0],
                this->vox_skeleton_local_matrix_reference[i][1],
                this->vox_skeleton_local_matrix_reference[i][2],
                this->vox_skeleton_local_matrix_reference[i][3],
                this->vox_skeleton_local_matrix_reference[i][4],
                this->vox_skeleton_local_matrix_reference[i][5]);

    fprintf(f, "#part number, node number, voxel format filename\n");
    for (int i=0; i<this->n_parts; i++)
        fprintf(f, "%d %d %s\n", i,
                this->vox_part[i]->skeleton_parent_matrix,
                this->vox_part[i]->filename);

    fprintf(f, "#voxel: anchor x,y,z rotatation x,y,z\n");
    for (int i=0; i<this->n_parts; i++)
        fprintf(f, "%d %f %f %f  %f %f %f\n", i,
                this->vox_part[i]->sx,
                this->vox_part[i]->sy,
                this->vox_part[i]->sz,
                this->vox_part[i]->srx,
                this->vox_part[i]->sry,
                this->vox_part[i]->srz);

    fclose(f);
}

}   // Voxels
