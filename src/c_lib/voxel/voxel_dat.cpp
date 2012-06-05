#include "voxel_dat.hpp"

/* *
 * Dat storage
 * */


/* Color */

//#if DC_CLIENT
void VoxColors::init(int dx, int dy, int dz)
{
    if (this->rgba != NULL)
    {
        printf("WARNING VoxColors::init -- rgba is not NULL (called twice?)\n");
        return;
    }
    this->n = dx*dy*dz;
    this->rgba = (unsigned char**)malloc(sizeof(unsigned char*)*this->n);
    this->index = (int**)malloc(sizeof(int*)*this->n);
    int i;
    int x=0,y=0,z=0;
    for (i=0; i<n; i++) {
        this->rgba[i] = (unsigned char*)malloc(sizeof(unsigned char)*4);
        this->rgba[i][0] = 0;
        this->rgba[i][1] = 0;
        this->rgba[i][2] = 0;
        this->rgba[i][3] = 0;

        this->index[i] = (int*)malloc(sizeof(int)*3);
        this->index[i][0] = x;
        this->index[i][1] = y;
        this->index[i][2] = z;

        x++;
        if (x==dx) {
            y++;
            if (y==dy) {
                z++;
            }
        }
        x %= dx;
        y %= dy;
        z %= dz;
    }
}

void VoxColors::set(int i, int x, int y, int z, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    if (i >= this->n)
    {
        printf("VoxColors::set -- index %d exceeds voxel count %d\n", i, this->n);
        return;
    }

    if (this->rgba == NULL)
    {
        printf("WARNING VoxColors::set, rgba is NULL\n");
        return;
    }
    if (this->index == NULL)
    {
        printf("WARNING VoxColors::set, index is NULL\n");
        return;
    }
    
    this->rgba[i][0] = r;
    this->rgba[i][1] = g;
    this->rgba[i][2] = b;
    this->rgba[i][3] = a;

    this->index[i][0] = x;
    this->index[i][1] = y;
    this->index[i][2] = z;
}

void VoxColors::set_team(bool team)
{
    this->team = team;
}

void VoxColors::set_team_base(unsigned char r, unsigned char g, unsigned char b)
{
    this->team_r = r;
    this->team_g = g;
    this->team_b = b;
}

VoxColors::VoxColors()
:
rgba(NULL), index(NULL), n(0),
team(false), team_r(0), team_g(0), team_b(0)
{}

VoxColors::~VoxColors()
{
    if (this->n <= 0) return;
    
    int i;
    if (this->rgba != NULL) {
        for (i=0; i<this->n; i++) {
            if (this->rgba[i] == NULL) continue;
            free(this->rgba[i]);
        }
        free(this->rgba);
    }

    if (this->index != NULL) {
        for (i=0; i<this->n; i++) {
            if (this->index[i] == NULL) continue;
            free(this->index[i]);
        }
        free(this->index);
    }
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

void VoxPart::set_filename(char *filename)
{
    int len = strlen(filename);
    this->filename = (char*)realloc(this->filename, sizeof(char) * (len+1));
    strcpy(this->filename, filename);
}


VoxPart::VoxPart(
    VoxDat* dat,
    int part_num,
    float vox_size,
    int dimension_x, int dimension_y, int dimension_z,
    char* filename,
    bool biaxial
):
dimension(dimension_x, dimension_y, dimension_z),
dat(dat),
part_num(part_num),
vox_size(vox_size),
biaxial(biaxial)
{
    //#if DC_CLIENT
    colors.init(dimension_x, dimension_y, dimension_z);
    //#endif
    int len = strlen(filename);
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
    this->dat->vox_volume_local_matrix[part_num] =  affine_euler_rotation_and_translation(
        this->sx, this->sy, this->sz,
        this->srx, this->sry, this->srz
    );
}


/* Skeleton Nodes */

void VoxDat::init_skeleton(int n_skeleton)
{
    if( voxel_skeleton_inited == true)
    {
        printf("VoxDat::init_skeleton ERROR!! skeletons inited twice!\n");
        return;
    }
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
    if (voxel_skeleton_inited != true)
    {
        printf("VoxDat::reset_skeleton_local_matrix ERROR!! skeletons not inited!\n");
        return;
    }

    if (node < 0 || node >= this->n_skeleton_nodes)
    {
        printf("VoxDat::reset_skeleton_local_matrix -- node %d invalid\n", node);
        return;
    }

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
    if( voxel_skeleton_inited != true)
    {
        printf("VoxDat::set_skeleton_local_matrix ERROR!! skeletons not inited!\n");
        return;
    }

    const int debug = 0;

    vox_skeleton_local_matrix[node] = affine_euler_rotation_and_translation(x,y,z, rx,ry,rz);
    vox_skeleton_local_matrix_reference[node][0] = x;
    vox_skeleton_local_matrix_reference[node][1] = y;
    vox_skeleton_local_matrix_reference[node][2] = z;
    vox_skeleton_local_matrix_reference[node][3] = rx;
    vox_skeleton_local_matrix_reference[node][4] = ry;
    vox_skeleton_local_matrix_reference[node][5] = rz;
    
    if(debug) 
    {
        printf("!!! VoxDat::set_skeleton_local_matrix i=%i \n", node);

        print_affine( vox_skeleton_local_matrix[node] );
        printf("\n");
    }
}

void VoxDat::set_skeleton_node_parent(int node, int parent)
{
    if( voxel_skeleton_inited != true)
    {
        printf("VoxDat::set_skeleton_node_parent ERROR!! skeletons not inited!\n");
        return;
    }
    vox_skeleton_transveral_list[node] = parent;
}

/* Body (contains parts) */

void VoxDat::init_parts(int n_parts) {
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
    char* filename,
    bool biaxial
)
{
    if (!voxel_volume_inited) printf("ERROR WARNING: VoxDat not inited\n");
    VoxPart* p = vox_part[part_num];
    if (p==NULL) {
        p = new VoxPart(
            this,
            part_num,
            vox_size,
            dimension_x, dimension_y, dimension_z,
            filename,
            biaxial
        );
        vox_part[part_num] = p;
    } else {
        p->dat = this;
        p->part_num = part_num;
        p->vox_size = vox_size;
        p->set_dimension(dimension_x, dimension_y, dimension_z);
        //#if DC_CLIENT
        p->colors.init(dimension_x, dimension_y, dimension_z);
        //#endif
        p->biaxial = biaxial;
        p->set_filename(filename);
    }
}

//anchor x,y,z then rotation x,y,z
void VoxDat::set_part_local_matrix( int part_num, float x, float y, float z, float rx, float ry, float rz)
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


//#if DC_CLIENT
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

#if DC_CLIENT
void VoxDat::set_team(int part, bool team, unsigned char r, unsigned char g, unsigned char b)
{
    VoxPart* p = vox_part[part];
    if (p==NULL)
    {
        printf("WARNING VoxDat::set_color -- part %d is NULL\n", part);
        return;
    }
    p->colors.set_team(team);
    p->colors.set_team_base(r,g,b);
}
#endif

VoxDat::VoxDat()
:
voxel_volume_inited(false),
vox_part(NULL),
vox_skeleton_local_matrix(NULL)
{}

VoxDat::~VoxDat()
{
    for (int i=0; i<n_parts; i++)  delete vox_part[i];
    delete[] vox_part;
    delete[] this->vox_skeleton_local_matrix;
    delete[] this->vox_volume_local_matrix;
    for (int i=0; i<this->n_skeleton_nodes; i++)
        free(this->vox_skeleton_local_matrix_reference[i]);
    free(this->vox_skeleton_local_matrix_reference);
}

void VoxDat::save(char* fn)
{
    int i;
    FILE* f = fopen(fn, "w");

    fprintf(f, "#skeleton nodes, voxel volumes\n");
    fprintf(f, "%d %d\n", this->n_skeleton_nodes, this->n_parts);
    
    fprintf(f, "#connection matrix\n");
    for (i=0; i<this->n_skeleton_nodes; i++)
        fprintf(f, "%d %d\n", i, this->vox_skeleton_transveral_list[i]);
    
    fprintf(f, "#skeleton_node: anchor x,y,z rotatation x,y,z\n");
    for (i=0; i<this->n_skeleton_nodes; i++)
        fprintf(f, "%d %f %f %f  %f %f %f\n", i,
            this->vox_skeleton_local_matrix_reference[i][0],
            this->vox_skeleton_local_matrix_reference[i][1],
            this->vox_skeleton_local_matrix_reference[i][2],
            this->vox_skeleton_local_matrix_reference[i][3],
            this->vox_skeleton_local_matrix_reference[i][4],
            this->vox_skeleton_local_matrix_reference[i][5]
        );

    fprintf(f, "#part number, node number, voxel format filename\n");
    for (i=0; i<this->n_parts; i++)
        fprintf(f, "%d %d %s\n", i,
            this->vox_part[i]->skeleton_parent_matrix,
            this->vox_part[i]->filename
        );
    
    fprintf(f, "#voxel: anchor x,y,z rotatation x,y,z\n");
    for (i=0; i<this->n_parts; i++)
        fprintf(f, "%d %f %f %f  %f %f %f\n", i,
            this->vox_part[i]->sx,
            this->vox_part[i]->sy,
            this->vox_part[i]->sz,
            this->vox_part[i]->srx,
            this->vox_part[i]->sry,
            this->vox_part[i]->srz
        );

    fclose(f);
}
