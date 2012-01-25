#include "voxel_body.hpp"

/* *
 * Dat storage
 * */


/* Color */

#ifdef DC_CLIENT
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
#endif

/* Rotation */

void VoxPartRotation::set(float x, float y, float z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}

VoxPartRotation::VoxPartRotation()
{}
VoxPartRotation::VoxPartRotation(float x, float y, float z)
:
x(x), y(y), z(z)
{}

/* Anchor */

void VoxPartAnchor::set(float x, float y, float z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}

VoxPartAnchor::VoxPartAnchor()
{}
VoxPartAnchor::VoxPartAnchor(float x, float y, float z)
:
x(x), y(y), z(z)
{}


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

void VoxPart::set_rotation(float x, float y, float z)
{
    rotation.set(x,y,z);
}

void VoxPart::set_anchor(float x, float y, float z)
{
    anchor.set(x,y,z);
}

void VoxPart::set_dimension(int x, int y, int z)
{
    dimension.set(x,y,z);
}

VoxPart::VoxPart(
    int part_num,
    float vox_size,
    int dimension_x, int dimension_y, int dimension_z,
    float anchor_x, float anchor_y, float anchor_z,
    float rotation_x, float rotation_y, float rotation_z,
    bool biaxial
):
rotation(rotation_x, rotation_y, rotation_z),
anchor(anchor_x, anchor_y, anchor_z),
dimension(dimension_x, dimension_y, dimension_z),
part_num(part_num),
vox_size(vox_size),
biaxial(biaxial)
{
    #ifdef DC_CLIENT
    colors.init(dimension_x, dimension_y, dimension_z);
    #endif
}

VoxPart::VoxPart(
    int part_num,
    float vox_size,
    int dimension_x, int dimension_y, int dimension_z,
    bool biaxial
):
dimension(dimension_x, dimension_y, dimension_z),
part_num(part_num),
vox_size(vox_size),
biaxial(biaxial)
{
    #ifdef DC_CLIENT
    colors.init(dimension_x, dimension_y, dimension_z);
    #endif
}


/* Skeleton Nodes */

void VoxBody::init_skeleton(int n_skeleton)
{
    this->n_skeleton_nodes = n_skeleton;
    vox_skeleton_local_matrix = new Mat4[n_skeleton];
    vox_skeleton_transveral_list = new int[n_skeleton];
}

void VoxBody::set_skeleton_node_matrix(int node, float x, float y, float z, float rx, float ry, float rz)
{
    vox_skeleton_local_matrix[node] = mat4_euler_rotation_and_translation( x,y,z, rx,ry,rz);
}

void VoxBody::set_skeleton_node_parent(int node, int parent)
{
    vox_skeleton_transveral_list[node] = parent;
}

/* Body (contains parts) */

void VoxBody::init_parts(int n_parts) {
    if (inited) {
        printf("WARNING VoxBody::init_part -- called more than once\n");
        return;
    }
    inited = true;
    
    this->n_parts = n_parts;
    vox_part = (VoxPart**)malloc(sizeof(VoxPart*)*n_parts);
    for (int i=0; i<n_parts; vox_part[i++] = NULL);
}

void VoxBody::set_part(
    int part_num,
    float vox_size,
    int dimension_x, int dimension_y, int dimension_z,
    float anchor_x, float anchor_y, float anchor_z,
    float rotation_x, float rotation_y, float rotation_z,
    bool biaxial
)
{
    if (!inited) printf("ERROR WARNING: VoxBody not inited\n");
    VoxPart* p = vox_part[part_num];
    if (p==NULL) {
        p = new VoxPart(
                part_num,
                vox_size,
                dimension_x, dimension_y, dimension_z,
                anchor_x, anchor_y, anchor_z,
                rotation_x, rotation_y, rotation_z,
                biaxial
            );
        vox_part[part_num] = p;
    } else {
        p->part_num = part_num;
        p->vox_size = vox_size;
        p->set_dimension(dimension_x, dimension_y, dimension_z);
        p->set_rotation(rotation_x, rotation_y, rotation_z);
        p->set_anchor(anchor_x, anchor_y, anchor_z);
        #ifdef DC_CLIENT
        p->colors.init(dimension_x, dimension_y, dimension_z);
        #endif
        p->biaxial = biaxial;
    }
}

void VoxBody::set_part_properties(
    int part_num,
    float vox_size,
    int dimension_x, int dimension_y, int dimension_z,
    bool biaxial
)
{
    if (!inited) printf("ERROR WARNING: VoxBody not inited\n");
    VoxPart* p = vox_part[part_num];
    if (p==NULL) {
        p = new VoxPart(
            part_num,
            vox_size,
            dimension_x, dimension_y, dimension_z,
            biaxial
        );
        vox_part[part_num] = p;
    } else {
        p->part_num = part_num;
        p->vox_size = vox_size;
        p->set_dimension(dimension_x, dimension_y, dimension_z);
        #ifdef DC_CLIENT
        p->colors.init(dimension_x, dimension_y, dimension_z);
        #endif
        p->biaxial = biaxial;
    }
}

void VoxBody::set_part_spatials(
    int part_num,
    float anchor_x, float anchor_y, float anchor_z,
    float rotation_x, float rotation_y, float rotation_z
)
{
    if (!inited) printf("ERROR WARNING: VoxBody not inited\n");
    VoxPart* p = vox_part[part_num];
    if (p == NULL)
    {
        printf("ERROR VoxBody::set_part_spatials -- part %d is NULL. Abort\n", part_num);
        return;
    }
    p->set_rotation(rotation_x, rotation_y, rotation_z);
    p->set_anchor(anchor_x, anchor_y, anchor_z);
}

#ifdef DC_CLIENT
void VoxBody::set_color(int part, int x, int y, int z, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    VoxPart* p = vox_part[part];
    if (p==NULL) {
        printf("WARNING VoxBody::set_color -- part %d is NULL\n", part);
        return;
    }
    int i = x + y*p->dimension.x + z*p->dimension.y*p->dimension.x;
    p->colors.set(i, x,y,z, r,g,b,a);
}

void VoxBody::set_team(int part, bool team, unsigned char r, unsigned char g, unsigned char b)
{
    VoxPart* p = vox_part[part];
    if (p==NULL) {
        printf("WARNING VoxBody::set_color -- part %d is NULL\n", part);
        return;
    }
    p->colors.set_team(team);
    p->colors.set_team_base(r,g,b);
}
#endif

VoxBody::VoxBody()
:
vox_part(NULL),
inited(false),
vox_skeleton_local_matrix(NULL)
{}

VoxBody::~VoxBody()
{
    if (vox_part != NULL) {
        int i;
        for (i=0; i<n_parts; i++) {
            if (vox_part[i] == NULL) continue;
            delete vox_part[i];
        }
        free(vox_part);
    }

    if (this->vox_skeleton_local_matrix != NULL)
        delete[] this->vox_skeleton_local_matrix;
}
