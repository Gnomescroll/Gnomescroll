#include "voxel_volume.hpp"

#if DC_CLIENT
# include <voxel/voxel_render.hpp>
#endif

#include <state/state.hpp>
#include <voxel/constants.hpp>
#include <entity/object/object.hpp>
#include <entity/objects.hpp>
#include <entity/components/voxel_model.hpp>

namespace Voxels
{

const int VOXEL_VERTEX_SCRATCH_SIZE = 0xFFFF;
static VoxelVertex* voxel_vertex_scratch_buffer = NULL;

void init_voxel_volume()
{
    GS_ASSERT(voxel_vertex_scratch_buffer == NULL);
    voxel_vertex_scratch_buffer = new VoxelVertex[VOXEL_VERTEX_SCRATCH_SIZE];
}

void teardown_voxel_volume()
{
    delete[] voxel_vertex_scratch_buffer;
}

int VoxelVolume::voxel_ray_cast(const Vec3& position, const Vec3& direction,
                                float max_len, float* distance, Vec3i& collision)
{
    float x0 = position.x;
    float y0 = position.y;
    float z0 = position.z;
    float _dfx = direction.x;
    float _dfy = direction.y;
    float _dfz = direction.z;

    *distance = 100000.0f;
    const static int _ssize = 0xFF;
    const static int _bsize = 0xFFFF;
    // normalize direction
    float len2 = 1.0f/sqrtf(_dfx*_dfx+_dfy*_dfy+_dfz*_dfz);
    _dfx *= len2;
    _dfy *= len2;
    _dfz *= len2;

    // calculate endpoint
    float x1 = x0 + _dfx*max_len;
    float y1 = y0 + _dfy*max_len;
    float z1 = z0 + _dfz*max_len;

    const float len = max_len;

    int x = x0; //truncating conversion
    int y = y0;
    int z = z0;

    int _dx = ((x1-x0)/len) *_ssize;
    int _dy = ((y1-y0)/len) *_ssize;
    int _dz = ((z1-z0)/len) *_ssize;

    int cdx = _dx >= 0 ? 1 : -1;
    int cdy = _dy >= 0 ? 1 : -1;
    int cdz = _dz >= 0 ? 1 : -1;

    int dx = _dx*cdx;
    int dy = _dy*cdy;
    int dz = _dz*cdz;

    float dummy;
    int cx = cdx >= 0 ? modff(x0, &dummy)*_bsize : _bsize - modff(x0, &dummy)*_bsize; //convert fractional part
    int cy = cdy >= 0 ? modff(y0, &dummy)*_bsize : _bsize - modff(y0, &dummy)*_bsize;
    int cz = cdz >= 0 ? modff(z0, &dummy)*_bsize : _bsize - modff(z0, &dummy)*_bsize;

    int i;
    int max_i = (_bsize / _ssize)*len + 1; //over project

    bool col = false;

    for (i = 0; i < max_i; i++)
    {
        cx += dx;
        cy += dy;
        cz += dz;
        if (cx >= _bsize || cy >= _bsize || cz >= _bsize)
        {
            if (cx >= _bsize)
            {
                cx -= _bsize;
                x += cdx;
                if (_test_occludes_safe(x,y,z))
                {
                    col = true;
                    break;
                }
            }
            if (cy >= _bsize)
            {
                cy -= _bsize;
                y += cdy;
                if (_test_occludes_safe(x,y,z))
                {
                    col = true;
                    break;
                }
            }
            if (cz >= _bsize)
            {
                cz -= _bsize;
                z += cdz;
                if (_test_occludes_safe(x,y,z))
                {
                    col = true;
                    break;
                }
            }
        }
    }

    if (col)
    {
        *distance = len * (float(i) / float(max_i));
        collision.x = x;
        collision.y = y;
        collision.z = z;
        return 1;
    }
    return 0; //no collision
}

// WARNING: HITSCAN_TEST_FAST 0 does not compile
#define HITSCAN_TEST_FAST 1
/*
    Enables using faster, Sagitta ray cast

*/
int VoxelVolume::hitscan_test(const Vec3& position, const Vec3& forward,
                              float r2, Vec3i& voxel)
{
    Vec3 p = vec3_sub(position, world_matrix.v[3]);
    Vec3 f = forward;

    struct Vec3 u;
    u.x = f.x*world_matrix.v[0].x + f.y*world_matrix.v[0].y + f.z*world_matrix.v[0].z,
    u.y = f.x*world_matrix.v[1].x + f.y*world_matrix.v[1].y + f.z*world_matrix.v[1].z,
    u.z = f.x*world_matrix.v[2].x + f.y*world_matrix.v[2].y + f.z*world_matrix.v[2].z;

    struct Vec3 v;
    v.x = p.x*world_matrix.v[0].x + p.y*world_matrix.v[0].y + p.z*world_matrix.v[0].z,
    v.y = p.x*world_matrix.v[1].x + p.y*world_matrix.v[1].y + p.z*world_matrix.v[1].z,
    v.z = p.x*world_matrix.v[2].x + p.y*world_matrix.v[2].y + p.z*world_matrix.v[2].z;


#if HITSCAN_TEST_FAST
    const float s = this->radius - sqrtf(r2); // Sagitta
    const float l = 0.01f + sqrtf(s * (2.0f * this->radius - s));

    v.x = ((v.x - l*u.x) / scale) + this->dimensions.x/2.0f;
    v.y = ((v.y - l*u.y) / scale) + this->dimensions.y/2.0f;
    v.z = ((v.z - l*u.z) / scale) + this->dimensions.z/2.0f;

    //printf("radius= %f, l= %f \n", radius, l);
#else
    v.x = ((v.x - radius*u.x) / scale) + this->dimensions.x/2.0f;
    v.y = ((v.y - radius*u.y) / scale) + this->dimensions.y/2.0f;
    v.z = ((v.z - radius*u.z) / scale) + this->dimensions.z/2.0f;
#endif

    float distance;
    //int collision[3];

#if HITSCAN_TEST_FAST
    if (voxel_ray_cast(v, u, 2*(radius/scale), &distance, voxel))
#else
    if (voxel_ray_cast(v, u, 2*(l/scale), &distance, voxel))
#endif
    {
        distance *= scale;
        return 1;
    }
    return 0;
}

#define DEBUG_POINT_COLLISION_TEST 1
int VoxelVolume::point_collision_test(const Vec3& p, Vec3i& voxel)
{
    #if DEBUG_POINT_COLLISION_TEST
    float x = p.x;
    float y = p.y;
    float z = p.z;

    x -= world_matrix.v[3].x;
    y -= world_matrix.v[3].y;
    z -= world_matrix.v[3].z;

    struct Vec3 v;

    v.x = x*world_matrix.v[0].x + y*world_matrix.v[0].y + z*world_matrix.v[0].z;
    v.y = x*world_matrix.v[1].x + y*world_matrix.v[1].y + z*world_matrix.v[1].z;
    v.z = x*world_matrix.v[2].x + y*world_matrix.v[2].y + z*world_matrix.v[2].z;

    v.x = (v.x / scale) + this->dimensions.x/2;
    v.y = (v.y / scale) + this->dimensions.y/2;
    v.z = (v.z / scale) + this->dimensions.z/2;
    #else

    p = vec3_sub(p, world_matrix_v[3]);
    struct Vec3 v;

    v.x = vec3_dot(p,world_matrix_v[0]);
    v.x = vec3_dot(p,world_matrix_v[1]);
    v.x = vec3_dot(p,world_matrix_v[2]);

    v.x = (v.x / scale) + this->dimensions.x/2;
    v.y = (v.y / scale) + this->dimensions.y/2;
    v.z = (v.z / scale) + this->dimensions.z/2;
    #endif

    voxel = vec3i_init(v);
    return _test_occludes_safe(v.x, v.y, v.z);
}

void VoxelVolume::set_parameters(const Vec3i& dimensions, float scale)
{
    this->dimensions = dimensions;
    this->scale = scale;
}

void VoxelVolume::init(const Vec3i& dimensions, float scale)
{
    IF_ASSERT(this->voxel != NULL) return;

    this->set_parameters(dimensions, scale);

    this->hdx = this->dimensions.x / 2.0f;
    this->hdy = this->dimensions.y / 2.0f;
    this->hdz = this->dimensions.z / 2.0f;

    int powx = pow2_2(this->dimensions.x);
    int powy = pow2_2(this->dimensions.y);
    int powz = pow2_2(this->dimensions.z);

    this->index1 = pow2_1(this->dimensions.x);
    this->index12 = pow2_1(this->dimensions.x) + pow2_1(this->dimensions.y);
    this->index_max = powx*powy*powz;

    //update radius if changing scale
    this->radius = sqrtf((hdx*hdz + hdy*hdy + hdz*hdz)) * scale; //radius of bounding sphere

    this->voxel = (union Voxel*)malloc(sizeof(union Voxel) * powx * powy * powz);
    needs_vbo_update = true;
}

VoxelVolume::VoxelVolume() :
    parent_world_matrix(NULL),
    render_id(-1),
    draw(true),
    hitscan(true),
    scale(1.0f),
    radius(0),
    dimensions(vec3i_init(1)),
    voxel(NULL),
    index1(0), index12(0),
    index_max(0),
    hdx(1),hdy(1),hdz(1),
    needs_vbo_update(false),
    damaged(false),
    voxel_hitscan_list(NULL)
    #if DC_CLIENT
    , voxel_render_list(NULL)
    , voxel_render_list_id(-1)
    #endif
{
    memset(&this->local_matrix, 0, sizeof(struct Affine));
    memset(&this->world_matrix, 0, sizeof(struct Affine));
    this->vhe.vv = this;
}

VoxelVolume::VoxelVolume(const Vec3i& dimensions, float scale) :
    parent_world_matrix(NULL),
    render_id(-1),
    draw(true),
    hitscan(true),
    scale(1.0f),
    radius(0),
    voxel(NULL),
    index1(0), index12(0),
    index_max(0),
    hdx(1),hdy(1),hdz(1),
    needs_vbo_update(false),
    damaged(false),
    voxel_hitscan_list(NULL)
    #if DC_CLIENT
    , voxel_render_list(NULL)
    , voxel_render_list_id(-1)
    #endif
{
    memset(&this->world_matrix, 0, sizeof(struct Affine));
    memset(&this->local_matrix, 0, sizeof(struct Affine));
    this->init(dimensions, scale);
    this->vhe.vv = this;
}

VoxelVolume::~VoxelVolume()
{
    #if DC_CLIENT
    if (voxel_render_list != NULL) printf("ERROR! voxel volume deconstructor, voxel_render_list not unregistered\n");
    #endif
    if (this->hitscan)
        if (voxel_hitscan_list != NULL) printf("ERROR! voxel volume deconstructor, voxel_hitscan_list not unregistered\n");
    free(this->voxel);
}

//external methods
void VoxelVolume::set(int x, int y, int z, Voxel* v)
{
    if (x >= this->dimensions.x || y >= this->dimensions.y || z >= this->dimensions.z) return;
    _set(x,y,z,v);
    needs_vbo_update = true;
    damaged = true;
}

inline void VoxelVolume::set(int x, int y, int z, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    if (x >= this->dimensions.x || y >= this->dimensions.y || z >= this->dimensions.z) return;
    _set(x,y,z,r,g,b,a);
    needs_vbo_update = true;
    damaged = true;
}

#if DC_CLIENT

#include <stdio.h>

static unsigned char _gamma_correction[256];

/*
static const int vnset[18] = { 0,0,1,
0,0,-1,
1,0,0 ,
-1,0,0 ,
0,1,0 ,
0,-1,0
};
*/

inline int get_ao_weight(int side_1, int side_2, int corner)
{
    static const int occ_array[3] = { 255, 128, 64 };

    int occ = (side_1 | side_2 | corner) + (side_1 & side_2);
    return occ_array[occ];
}

/*
    if vert_num == 0:
        tx = 0.0
        ty = 0.0
    elif vert_num == 1:
        tx = 0.0
        ty = 1.0
    elif vert_num == 2:
        tx = 1.0
        ty = 1.0
    elif vert_num == 3:
        tx = 1.0
        ty = 0.0
*/

void VoxelVolume::push_voxel_quad(VoxelVertex* scratch, int* index, int x, int y, int z, int side, float* vset, float ox,float oy,float oz)
{
    //struct voxTexElement

    //color
    {
        Voxel color;
        color.color = get_as_int(x,y,z);

        color.r = _gamma_correction[color.r];
        color.g = _gamma_correction[color.g];
        color.b = _gamma_correction[color.b];

        scratch[*index + 0].color = color.color;
        scratch[*index + 1].color = color.color;
        scratch[*index + 2].color = color.color;
        scratch[*index + 3].color = color.color;
    }

    //texture cordinates
        scratch[*index + 0].t = voxel_tex_array[0].t;
        scratch[*index + 1].t = voxel_tex_array[1].t;
        scratch[*index + 2].t = voxel_tex_array[2].t;
        scratch[*index + 3].t = voxel_tex_array[3].t;

    //AO
    {
        int CX[8];

        for (int i=0; i<8; i++)
        {
            int index = side*8*3+i*3;
            CX[i] = _test_occludes_safe(x+ao_perm[index+0],y+ao_perm[index+1],z+ao_perm[index+2]);
        }

        {
            VoxAOElement _ao;

            _ao.ao[0] = get_ao_weight(CX[7], CX[1], CX[0]);
            _ao.ao[1] = get_ao_weight(CX[5], CX[7], CX[6]);
            _ao.ao[2] = get_ao_weight(CX[1], CX[3], CX[2]);
            _ao.ao[3] = get_ao_weight(CX[3], CX[5], CX[4]);

            scratch[*index + 0].AO = _ao.AO;
            scratch[*index + 1].AO = _ao.AO;
            scratch[*index + 2].AO = _ao.AO;
            scratch[*index + 3].AO = _ao.AO;
        }
    }

    {
        scratch[*index + 0].n = voxel_normal_array[side].n;
        scratch[*index + 1].n = voxel_normal_array[side].n;
        scratch[*index + 2].n = voxel_normal_array[side].n;
        scratch[*index + 3].n = voxel_normal_array[side].n;
    }

    {
        float fx = this->scale*((float) x) - ox;
        float fy = this->scale*((float) y) - oy;
        float fz = this->scale*((float) z) - oz;

        int _side = side*12;

        scratch[*index + 0].x = fx + vset[_side + 0 ];
        scratch[*index + 0].y = fy + vset[_side + 1 ];
        scratch[*index + 0].z = fz + vset[_side + 2 ];

        scratch[*index + 1].x = fx + vset[_side + 3 ];
        scratch[*index + 1].y = fy + vset[_side + 4 ];
        scratch[*index + 1].z = fz + vset[_side + 5 ];

        scratch[*index + 2].x = fx + vset[_side + 6 ];
        scratch[*index + 2].y = fy + vset[_side + 7 ];
        scratch[*index + 2].z = fz + vset[_side + 8 ];

        scratch[*index + 3].x = fx + vset[_side + 9 ];
        scratch[*index + 3].y = fy + vset[_side + 10];
        scratch[*index + 3].z = fz + vset[_side + 11];
    }

    //if (*index >= 65536) printf("BUFFER OVERFLOW!!! \n");
    *index += 4;
}

/*
#north/south is +/- x
#west/east is +/- y
l = [
        1,1,1 , 0,1,1 , 0,0,1 , 1,0,1 , #top
        0,1,0 , 1,1,0 , 1,0,0 , 0,0,0 , #bottom
        1,0,1 , 1,0,0 , 1,1,0 , 1,1,1 , #north
        0,1,1 , 0,1,0 , 0,0,0 , 0,0,1 , #south
        1,1,1 , 1,1,0 , 0,1,0,  0,1,1 , #west
        0,0,1 , 0,0,0 , 1,0,0 , 1,0,1 , #east
]
*/


#define VOXEL_RENDER_DEBUG_02 0

void VoxelVolume::update_vertex_list()
{
    static int compute_gamma_chart = 0;
    if (compute_gamma_chart == 0)
    {
        compute_gamma_chart = 1;

        static const float gamma_correction = 2.2f;
        for (int i=0; i< 255; i++)
        {
            float intensity = (float) i;
            intensity = powf(intensity/255, gamma_correction)*255;
            _gamma_correction[i] = (unsigned char)((int) intensity);
        }
    }

    static const float vset[72] = {
        1,1,1 , 0,1,1 , 0,0,1 , 1,0,1 , //top
        0,1,0 , 1,1,0 , 1,0,0 , 0,0,0 , //bottom
        1,0,1 , 1,0,0 , 1,1,0 , 1,1,1 , //north
        0,1,1 , 0,1,0 , 0,0,0 , 0,0,1 , //south
        1,1,1 , 1,1,0 , 0,1,0,  0,1,1 , //west
        0,0,1 , 0,0,0 , 1,0,0 , 1,0,1 , //east
    };

    float vset_dynamic[72];
    for (int i=0; i<72; i++) vset_dynamic[i] = this->scale*vset[i];

    const float ox = this->hdx*this->scale;
    const float oy = this->hdy*this->scale;
    const float oz = this->hdz*this->scale;

    int index = 0;

    for (int x=0; x < this->dimensions.x; x++)
    for (int y=0; y < this->dimensions.y; y++)
    for (int z=0; z < this->dimensions.z; z++)
    {
        if (get_as_int(x,y,z) == 0) continue;

    #if VOXEL_RENDER_DEBUG_02
        push_voxel_quad(voxel_vertex_scratch_buffer, &index, x,y,z, 0, vset_dynamic, ox,oy,oz);
        push_voxel_quad(voxel_vertex_scratch_buffer, &index, x,y,z, 1, vset_dynamic, ox,oy,oz);
        push_voxel_quad(voxel_vertex_scratch_buffer, &index, x,y,z, 2, vset_dynamic, ox,oy,oz);
        push_voxel_quad(voxel_vertex_scratch_buffer, &index, x,y,z, 3, vset_dynamic, ox,oy,oz);
        push_voxel_quad(voxel_vertex_scratch_buffer, &index, x,y,z, 4, vset_dynamic, ox,oy,oz);
        push_voxel_quad(voxel_vertex_scratch_buffer, &index, x,y,z, 5, vset_dynamic, ox,oy,oz);
    #else
        if (z+1 == this->dimensions.z || get_as_int(x,y,z+1) == 0)
            push_voxel_quad(voxel_vertex_scratch_buffer, &index, x,y,z, 0, vset_dynamic, ox,oy,oz);
        if (z == 0 || get_as_int(x,y,z-1) == 0)
            push_voxel_quad(voxel_vertex_scratch_buffer, &index, x,y,z, 1, vset_dynamic, ox,oy,oz);
        if (x+1 == this->dimensions.x || get_as_int(x+1,y,z) == 0)
            push_voxel_quad(voxel_vertex_scratch_buffer, &index, x,y,z, 2, vset_dynamic, ox,oy,oz);
        if (x == 0 || get_as_int(x-1,y,z) == 0)
            push_voxel_quad(voxel_vertex_scratch_buffer, &index, x,y,z, 3, vset_dynamic, ox,oy,oz);
        if (y+1 ==this->dimensions.y || get_as_int(x,y+1,z) == 0)
            push_voxel_quad(voxel_vertex_scratch_buffer, &index, x,y,z, 4, vset_dynamic, ox,oy,oz);
        if (y == 0 || get_as_int(x,y-1,z) == 0)
            push_voxel_quad(voxel_vertex_scratch_buffer, &index, x,y,z, 5, vset_dynamic, ox,oy,oz);
    #endif
    }

    if (vvl.vertex_list != NULL)
    {
        delete[] vvl.vertex_list;
        vvl.vertex_list = NULL;
    }

    if (index == 0)
    {
        printf("VoxelVolume::update_vertex_list, FATAL ERROR, no quads in voxel model\n");
        vvl.vnum = 0;
        return;
    }

    IF_ASSERT(index >= VOXEL_VERTEX_SCRATCH_SIZE) return;
    IF_ASSERT(index < 0) return;

    vvl.vertex_list = new VoxelVertex[index];
    memcpy(vvl.vertex_list, voxel_vertex_scratch_buffer, index*sizeof(VoxelVertex));

    vvl.vnum = index;
}
#endif

void VoxelVolume::set_color(int x, int y, int z, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    _set(x,y,z, r,g,b,a);
    needs_vbo_update = true;
}

void VoxelVolume::set_color(int x, int y, int z, unsigned char rgba[4])
{
    _set(x,y,z, rgba[0], rgba[1], rgba[2], rgba[3]);
    needs_vbo_update = true;
}

void VoxelVolume::set_hitscan_properties(EntityID entity_id, EntityType entity_type, short part_id)
{
    this->vhe.entity_id = entity_id;
    this->vhe.entity_type = entity_type;
    this->vhe.part_id = part_id;
    this->vhe.vv = this;
}

void VoxelVolume::draw_bounding_box()
{
    #if DC_CLIENT
    glDisable(GL_TEXTURE_2D);
    glLineWidth(2.0f);

    int i,j;
    float _x,_y,_z;

    glBegin(GL_LINES);
    glColor3ub(255,0,0);

    struct Vec3 vx;
    struct Vec3 vy;
    struct Vec3 vz;

    Vec3 u;

    //world_matrix

    for (i=0; i<12; i++)
    {
            j = 3*vertex_index2[2*i+0];

            vx = vec3_scalar_mult(world_matrix.v[0], v_set2[j+0]*hdx*scale);
            vy = vec3_scalar_mult(world_matrix.v[1], v_set2[j+1]*hdy*scale);
            vz = vec3_scalar_mult(world_matrix.v[2], v_set2[j+2]*hdz*scale);

            u = vec3_add4(vx,vy,vz, world_matrix.v[3]);

            _x = u.x;
            _y = u.y;
            _z = u.z;

            glVertex3f(_x,_y,_z);
            j = 3*vertex_index2[2*i+1];

            vx = vec3_scalar_mult(world_matrix.v[0], v_set2[j+0]*hdx*scale);
            vy = vec3_scalar_mult(world_matrix.v[1], v_set2[j+1]*hdy*scale);
            vz = vec3_scalar_mult(world_matrix.v[2], v_set2[j+2]*hdz*scale);

            u = vec3_add4(vx,vy,vz, world_matrix.v[3]);

            _x = u.x;
            _y = u.y;
            _z = u.z;

            glVertex3f(_x,_y,_z);
    }

    glEnd();

    glColor3ub(255, 255, 255);
    glEnable(GL_TEXTURE_2D);
    glLineWidth(1.0f);
    #endif
}

inline Voxel* VoxelVolume::get(int x, int y, int z)
{
    return &voxel[x+(y << index1)+(z << index12)];
}

inline int VoxelVolume::get_as_int(int x, int y, int z)
{
    return voxel[x+(y << index1)+(z << index12)].color;
}

/*
Tests whether a voxel is occupied, for AO
*/
inline bool VoxelVolume::_test_occludes_safe(int x, int y, int z)
{
    if (x < 0 || x >= this->dimensions.x || y < 0 || y >= this->dimensions.y ||
        z < 0 || z >= this->dimensions.z) return false;
    int index = x+(y << index1)+(z << index12);
    return (voxel[index].color != 0);
}

inline void VoxelVolume::_set(int x, int y, int z, Voxel* v)
{
    voxel[x+(y << index1)+(z << index12)] = *v;
}

inline void VoxelVolume::_set(int x, int y, int z, unsigned char r,
                              unsigned char g, unsigned char b, unsigned char a)
{
    Voxel* v = &voxel[x+(y << index1)+(z << index12)];
    v->r = r;
    v->g = g;
    v->b = b;
    v->a = a;
}

}   // Voxels
