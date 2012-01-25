#include "voxel_body.hpp"

/* *
 * Dat storage
 * */


/* Color */

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

/* Rotation */

//void VoxPartRotation::set(float fx, float fy, float fz, float nx, float ny, float nz)
//{
    //// normalize
    //float len;
    //len = sqrt(fx*fx + fy*fy + fz*fz);
    //fx /= len;
    //fy /= len;
    //fz /= len;
    //len = sqrt(nx*nx + ny*ny + nz*nz);
    //nx /= len;
    //ny /= len;
    //nz /= len;
    
    //this->fx = fx;
    //this->fy = fy;
    //this->fz = fz;
    //this->nx = nx;
    //this->ny = ny;
    //this->nz = nz;
//}

//VoxPartRotation::VoxPartRotation()
//{}
//VoxPartRotation::VoxPartRotation(float fx, float fy, float fz, float nx, float ny, float nz)
//{
    //this->set(fx,fy,fz,nx,ny,nz);
//}


/* Anchors */

void VoxPartAnchor::set(float length, float x, float y, float z)
{
    this->length = length;
    this->x = x;
    this->y = y;
    this->z = z;
}

VoxPartAnchor::VoxPartAnchor()
{}
VoxPartAnchor::VoxPartAnchor(float length, float x, float y, float z)
:
length(length),
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

void VoxPart::set_rotation(float fx, float fy, float fz, float nx, float ny, float nz)
{
    rotation.set(fx, fy, fz, nx, ny, nz);
}

void VoxPart::set_anchor(float len, float x, float y, float z)
{
    anchor.set(len, x, y, z);
}

void VoxPart::set_dimension(int x, int y, int z)
{
    dimension.set(x,y,z);
}

VoxPart::VoxPart(
    int part_num,
    float vox_size,
    float rot_fx, float rot_fy, float rot_fz,
    float rot_nx, float rot_ny, float rot_nz,
    float anc_len, float anc_x, float anc_y, float anc_z,
    int dim_x, int dim_y, int dim_z,
    bool biaxial
):
rotation(rot_fx, rot_fy, rot_fz, rot_nx, rot_ny, rot_nz),
anchor(anc_len, anc_x, anc_y, anc_z),
dimension(dim_x, dim_y, dim_z),
part_num(part_num),
vox_size(vox_size),
biaxial(biaxial)
{
    colors.init(dim_x, dim_y, dim_z);
}


/* Body (contains parts) */

void VoxBody::init(int n_parts) {
    if (inited) {
        printf("WARNING VoxBody::init -- called more than once\n");
        return;
    }
    this->n_parts = n_parts;
    vox_part = (VoxPart**)malloc(sizeof(VoxPart*)*n_parts);
    inited = true;

    int i;
    for (i=0; i<n_parts; i++) {
        vox_part[i] = NULL;
    }
}

void VoxBody::set_part(
    int part_num,
    float vox_size,
    float rot_fx, float rot_fy, float rot_fz,
    float rot_nx, float rot_ny, float rot_nz,
    float anc_len, float anc_x, float anc_y, float anc_z,
    int dim_x, int dim_y, int dim_z,
    bool biaxial
)
{
    if (!inited) printf("ERROR WARNING: VoxBody not inited\n");
    VoxPart* p = vox_part[part_num];
    if (p==NULL) {
        p = new VoxPart(
                part_num,
                vox_size,
                rot_fx, rot_fy, rot_fz,
                rot_nx, rot_ny, rot_nz,
                anc_len, anc_x, anc_y, anc_z,
                dim_x, dim_y, dim_z,
                biaxial
            );
        vox_part[part_num] = p;
    } else {
        p->vox_size = vox_size;
        p->set_rotation(rot_fx, rot_fy, rot_fz, rot_nx, rot_ny, rot_nz);
        p->set_anchor(anc_len, anc_x, anc_y, anc_z);
        p->set_dimension(dim_x, dim_y, dim_z);
        p->colors.init(dim_x, dim_y, dim_z);
        p->part_num = part_num;
        p->biaxial = biaxial;
    }
}

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

VoxBody::VoxBody()
:
vox_part(NULL),
inited(false)
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
}
