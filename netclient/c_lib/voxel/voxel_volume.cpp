#include "voxel_volume.hpp"

#include <c_lib/state/client_state.hpp>
#include <c_lib/state/server_state.hpp>
#include <voxel/constants.hpp>

#ifdef DC_CLIENT
#include <voxel/voxel_render.hpp>

void Voxel_volume::register_with_renderer(Voxel_render_list* vrl)
{
    vrl->register_voxel_volume(this);
    voxel_render_list = vrl;
}
#endif

const int VOXEL_VERTEX_SCRATCH_SIZE = 65536;
static Voxel_vertex* voxel_vertex_scratch_buffer = NULL;

void init_voxel_volume()
{
    voxel_vertex_scratch_buffer = new Voxel_vertex[VOXEL_VERTEX_SCRATCH_SIZE];
}

void teardown_voxel_volume()
{
    if (voxel_vertex_scratch_buffer != NULL)
        delete[] voxel_vertex_scratch_buffer;
}

int Voxel_volume::voxel_ray_cast(float x0,float y0,float z0, float _dfx,float _dfy,float _dfz, float max_l, float* distance, int* collision)
{
    const static int _ssize = 256;
    const static int _bsize = 65536;
    // normalize direction
    float len2 = sqrt( _dfx*_dfx+_dfy*_dfy+_dfz*_dfz );
    _dfx /= len2;
    _dfy /= len2;
    _dfz /= len2;

    // calculate endpoint
    float x1,y1,z1;
    x1 = x0 + _dfx*max_l;
    y1 = y0 + _dfy*max_l;
    z1 = z0 + _dfz*max_l;

    const float len = max_l;

    int x,y,z;
    x = x0; //truncating conversion
    y = y0;
    z = z0;


    int _dx,_dy,_dz;
    _dx = ((x1-x0)/len) *_ssize;
    _dy = ((y1-y0)/len) *_ssize;
    _dz = ((z1-z0)/len) *_ssize;

    int cdx, cdy, cdz;
    cdx = _dx >= 0 ? 1 : -1;
    cdy = _dy >= 0 ? 1 : -1;
    cdz = _dz >= 0 ? 1 : -1;

    unsigned int dx,dy,dz;
    dx = _dx*cdx;
    dy = _dy*cdy;
    dz = _dz*cdz;

    int cx,cy,cz;
    float dummy;
    cx = cdx >=0 ? modff(x0, &dummy)*_bsize : _bsize - modff(x0, &dummy)*_bsize; //convert fractional part
    cy = cdy >=0 ? modff(y0, &dummy)*_bsize : _bsize - modff(y0, &dummy)*_bsize;
    cz = cdz >=0 ? modff(z0, &dummy)*_bsize : _bsize - modff(z0, &dummy)*_bsize;

    int i;
    int max_i = (_bsize / _ssize)*len + 1; //over project

    int col=0;

    for(i =0; i < max_i; i++) {
        cx += dx;
        cy += dy;
        cz += dz;
        if(cx >= _bsize || cy >= _bsize || cz >= _bsize) {
            if(cx >= _bsize) {
                cx -= _bsize;
                //_x = x;
                x += cdx;
                if(_test_occludes_safe(x,y,z) != 0) 
                {
                    col =1;
                    break;
                }
            }
            if(cy >= _bsize) {
                cy -= _bsize;
                //_y = y;
                y += cdy;
                if(_test_occludes_safe(x,y,z) != 0) 
                {
                    col=1;
                    break;
                }
            }
            if(cz >= _bsize) {
                cz -= _bsize;
                //_z = z;
                z += cdz;
                if(_test_occludes_safe(x,y,z) != 0) 
                {
                    col=1;
                    break;
                }
            }
        }
    }
    if(col == 1) 
    {
        *distance = len * (((float)i) / ((float)max_i));
        collision[0]=x; collision[1]=y; collision[2]=z; 
        return 1;
    } else 
    {
        //*distance = 0;
        return 0; //no collision
    }

}

// WARNING: HITSCAN_TEST_FAST 0 does not compile
#define HITSCAN_TEST_FAST 1
/*
    Enables using faster, Sagitta ray cast

*/
int Voxel_volume::hitscan_test(float x, float y, float z, float vx, float vy, float vz, float r2, int voxel[3])
{

    x -= world_matrix.v[3].x;
    y -= world_matrix.v[3].y;
    z -= world_matrix.v[3].z;

    struct Vec3 u;
    
    u.x = vx*world_matrix.v[0].x + vy*world_matrix.v[0].y + vz*world_matrix.v[0].z, 
    u.y = vx*world_matrix.v[1].x + vy*world_matrix.v[1].y + vz*world_matrix.v[1].z, 
    u.z = vx*world_matrix.v[2].x + vy*world_matrix.v[2].y + vz*world_matrix.v[2].z;

    struct Vec3 v;

    v.x = x*world_matrix.v[0].x + y*world_matrix.v[0].y + z*world_matrix.v[0].z, 
    v.y = x*world_matrix.v[1].x + y*world_matrix.v[1].y + z*world_matrix.v[1].z, 
    v.z = x*world_matrix.v[2].x + y*world_matrix.v[2].y + z*world_matrix.v[2].z;


#if HITSCAN_TEST_FAST
    const float s = this->radius - sqrt(r2); // Sagitta
    const float l = 0.01 + sqrt( s*(this->radius*2 - s) ) ;

    v.x = ((v.x - l*u.x) / scale) + xdim/2;
    v.y = ((v.y - l*u.y) / scale) + ydim/2;
    v.z = ((v.z - l*u.z) / scale) + zdim/2;

    //printf("radius= %f, l= %f \n", radius, l);
#else
    v.x = ((v.x - radius*u.x) / scale) + xdim/2;
    v.y = ((v.y - radius*u.y) / scale) + ydim/2;
    v.z = ((v.z - radius*u.z) / scale) + zdim/2;
#endif

    float distance;
    //int collision[3];

#if HITSCAN_TEST_FAST
    if(voxel_ray_cast(v.x,v.y,v.z, u.x,u.y,u.z, 2*radius/scale, &distance, voxel))
#else
    if(voxel_ray_cast(v.x,v.y,v.z, u.x,u.y,u.z, 2*l/scale, &distance, voxel))
#endif 
    {   
        distance *= scale;
        needs_vbo_update = true;
        return 1;
    }
    return 0;
}


void Voxel_volume::get_center(float *v)
{
    v[0] = world_matrix.v[3].f[0];
    v[1] = world_matrix.v[3].f[1];
    v[2] = world_matrix.v[3].f[2];
}
struct Vec3 Voxel_volume::get_center(){ return world_matrix.v[3]; }

void Voxel_volume::set_parameters(int xdim, int ydim, int zdim, float scale)
{
    this->xdim = xdim;
    this->ydim = ydim;
    this->zdim = zdim;
    this->scale = scale;
}

void Voxel_volume::init(int xdim, int ydim, int zdim, float scale)
{
    if(this->voxel != NULL)
    {
        printf("Voxel_volume::init, error voxel is not null, init called twice?\n");
        return;
    }

    //this->local_matrix = affine_identity();

    this->set_parameters(xdim, ydim, zdim, scale);

#ifdef DC_CLIENT
    voxel_render_list = NULL;
#endif
    voxel_hitscan_list = NULL;

    //printf("!!! %i %i %i \n", xdim,ydim,zdim );

    this->hdx = ((float) xdim) / 2;
    this->hdy = ((float) ydim) / 2;
    this->hdz = ((float) zdim) / 2;

    //printf("!!! %f %f %f \n", hdx, hdy,hdz);

    int powx = pow2_2(xdim);
    int powy = pow2_2(ydim);
    int powz = pow2_2(zdim);

    this->index1 = pow2_1(xdim);
    this->index12 = pow2_1(xdim) + pow2_1(ydim);
    this->index_max = powx*powy*powz;

    //update radius if changing scale
    this->radius = sqrt((hdx*hdz + hdy*hdy + hdz*hdz)) * scale; //radius of bounding sphere

    this->voxel = (union Voxel*)malloc(sizeof(union Voxel) * powx * powy * powz);
    needs_vbo_update = true;
}

Voxel_volume::Voxel_volume()
:
id(-1),
draw(true),
hitscan(true),
radius(0),
voxel(NULL),
damaged(false)
{}

Voxel_volume::Voxel_volume(int xdim, int ydim, int zdim, float scale)
:
id(-1),
draw(true),
hitscan(true),
radius(0),
damaged(false)
{
    this->voxel = NULL;
    this->init(xdim, ydim, zdim, scale);
}

Voxel_volume::~Voxel_volume()
{
    #ifdef DC_CLIENT
    if(voxel_render_list != NULL) printf("ERROR! voxel volume deconstructor, voxel_render_list not unregistered \n");
    #endif
    if (this->hitscan)
        if(voxel_hitscan_list != NULL) printf("ERROR! voxel volume deconstructor, voxel_hitscan_list not unregistered \n");
    free(this->voxel);
}

//external methods
void Voxel_volume::set(int x, int y, int z, Voxel* v)
{
    if (x<0 || x>= xdim || y<0 || y>=ydim || z<0 || z>=zdim) return;
    _set(x,y,z,v);
    needs_vbo_update = true;
    damaged = true;
}

inline void Voxel_volume::set(int x, int y, int z, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    if (x<0 || x>= xdim || y<0 || y>=ydim || z<0 || z>=zdim) return;
    _set(x,y,z,r,g,b,a);
    needs_vbo_update = true;
    damaged = true;
}

#ifdef DC_CLIENT

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

static inline int vCalcAdj(int side_1, int side_2, int corner)  __attribute((always_inline));
// Sets AO strength values

static inline int vCalcAdj(int side_1, int side_2, int corner) 
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

void Voxel_volume::push_voxel_quad(Voxel_vertex* scratch, int* index, int x, int y, int z, int side, float* vset, float ox,float oy,float oz)
{

    static const struct Voxel_normal voxel_normal_array[6] = { 
        {{{0,0,1,0}}},
        {{{0,0,-1,0}}},
        {{{1,0,0,0}}},
        {{{-1,0,0,0}}},
        {{{0,1,0,0}}},
        {{{0,-1,0,0}}}
        };

    static const struct Voxel_tex voxel_tex_array[4] = {
        {{{0,0,0,0}}},
        {{{0,1,0,0}}},
        {{{1,1,0,0}}},
        {{{1,0,0,0}}}
        };

    //struct voxTexElement

    static const int_fast8_t CI[6*8*3] = {
        1, 1, 1, 0, 1, 1, -1, 1, 1, -1, 0, 1, -1, -1, 1, 0, -1, 1, 1, -1, 1, 1, 0, 1,
        -1, 1, -1, 0, 1, -1, 1, 1, -1, 1, 0, -1, 1, -1, -1, 0, -1, -1, -1, -1, -1, -1, 0, -1,
        1, -1, 1, 1, -1, 0, 1, -1, -1, 1, 0, -1, 1, 1, -1, 1, 1, 0, 1, 1, 1, 1, 0, 1,
        -1, 1, 1, -1, 1, 0, -1, 1, -1, -1, 0, -1, -1, -1, -1, -1, -1, 0, -1, -1, 1, -1, 0, 1,
        1, 1, 1, 1, 1, 0, 1, 1, -1, 0, 1, -1, -1, 1, -1, -1, 1, 0, -1, 1, 1, 0, 1, 1,
        -1, -1, 1, -1, -1, 0, -1, -1, -1, 0, -1, -1, 1, -1, -1, 1, -1, 0, 1, -1, 1, 0, -1, 1 
        };

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
        
        for(int i=0; i<8; i++) 
        {
            int index = side*8*3+i*3;
            CX[i] = _test_occludes_safe(x+CI[index+0],y+CI[index+1],z+CI[index+2]);
        }

        {
            voxAOElement _ao;

            _ao.ao[0] = vCalcAdj(CX[7], CX[1], CX[0]);
            _ao.ao[1] = vCalcAdj(CX[5], CX[7], CX[6]);
            _ao.ao[2] = vCalcAdj(CX[1], CX[3], CX[2]);
            _ao.ao[3] = vCalcAdj(CX[3], CX[5], CX[4]);

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

    //if(*index >= 65536) printf("BUFFER OVERFLOW!!! \n");
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


#define VOXEL_RENDER_DEBUG_02 1

void Voxel_volume::update_vertex_list()
{
    static int compute_gamma_chart = 0;
    if(compute_gamma_chart == 0) 
    {
        compute_gamma_chart = 1;

        static const float gamma_correction = 2.2;
        for(int i=0; i< 255; i++)
        {
            float intensity = (float) i;
            intensity = pow(intensity/255, gamma_correction)*255;
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
    for(int i=0; i<72; i++) vset_dynamic[i] = this->scale*vset[i];
    
    const float ox = this->hdx*this->scale;
    const float oy = this->hdy*this->scale;
    const float oz = this->hdz*this->scale;

    int index = 0;

    for(int x=0; x < xdim; x++){
    for(int y=0; y < ydim; y++){
    for(int z=0; z < zdim; z++){
        if (get_as_int(x,y,z) == 0) continue;

    #if VOXEL_RENDER_DEBUG_02
        push_voxel_quad(voxel_vertex_scratch_buffer, &index, x,y,z, 0, vset_dynamic, ox,oy,oz);
        push_voxel_quad(voxel_vertex_scratch_buffer, &index, x,y,z, 1, vset_dynamic, ox,oy,oz);
        push_voxel_quad(voxel_vertex_scratch_buffer, &index, x,y,z, 2, vset_dynamic, ox,oy,oz);
        push_voxel_quad(voxel_vertex_scratch_buffer, &index, x,y,z, 3, vset_dynamic, ox,oy,oz);
        push_voxel_quad(voxel_vertex_scratch_buffer, &index, x,y,z, 4, vset_dynamic, ox,oy,oz);
        push_voxel_quad(voxel_vertex_scratch_buffer, &index, x,y,z, 5, vset_dynamic, ox,oy,oz);
    #else
        if(z+1 == zdim || get_as_int(x,y,z+1) == 0) push_voxel_quad(voxel_vertex_scratch_buffer, &index, x,y,z, 0, vset_dynamic, ox,oy,oz);
        if(z == 0 || get_as_int(x,y,z-1) == 0) push_voxel_quad(voxel_vertex_scratch_buffer, &index, x,y,z, 1, vset_dynamic, ox,oy,oz);
        if(x+1 == xdim || get_as_int(x+1,y,z) == 0) push_voxel_quad(voxel_vertex_scratch_buffer, &index, x,y,z, 2, vset_dynamic, ox,oy,oz);
        if(x == 0 || get_as_int(x-1,y,z) == 0) push_voxel_quad(voxel_vertex_scratch_buffer, &index, x,y,z, 3, vset_dynamic, ox,oy,oz);
        if(y+1 ==ydim || get_as_int(x,y+1,z) == 0) push_voxel_quad(voxel_vertex_scratch_buffer, &index, x,y,z, 4, vset_dynamic, ox,oy,oz);
        if(y == 0 || get_as_int(x,y-1,z) == 0) push_voxel_quad(voxel_vertex_scratch_buffer, &index, x,y,z, 5, vset_dynamic, ox,oy,oz);
    #endif
    }}}

    if (vvl.vertex_list != NULL)
    {
        delete[] vvl.vertex_list;
        vvl.vertex_list = NULL;
    }

    if (index == 0)
    {
        printf("Voxel_volume::update_vertex_list, FATAL ERROR, no quads in voxel model\n");
        vvl.vnum = 0;
        return;
    }
    
    if (index > VOXEL_VERTEX_SCRATCH_SIZE)
    {
        printf("WARNING: Voxel_volume::update_vertex_list -- vertex count %d exceeds allocated amount %d\n", index, 65536);
        printf("Shrinking vertex count -- expect bugs\n");
        index = VOXEL_VERTEX_SCRATCH_SIZE;
    }
    if (index < 0)
    {
        printf("WTF\n");
    }
    vvl.vertex_list = new Voxel_vertex[index];
    memcpy(vvl.vertex_list, voxel_vertex_scratch_buffer, index*sizeof(Voxel_vertex));
    
    vvl.vnum = index;
}
#endif

void Voxel_volume::set_color(int x, int y, int z, unsigned char r, unsigned char g, unsigned char b, unsigned char a) 
{
    _set(x,y,z, r,g,b,a);
    needs_vbo_update = true;
}

void Voxel_volume::set_color(int x, int y, int z, unsigned char rgba[4]) 
{
    _set(x,y,z, rgba[0], rgba[1], rgba[2], rgba[3] );
    needs_vbo_update = true;
}

void Voxel_volume::set_hitscan_properties(short entity_id, short entity_type, short part_id)
{
    this->vhe.entity_id = entity_id;
    this->vhe.entity_type = entity_type;
    this->vhe.part_id = part_id;
    this->vhe.vv = this;
}

void Voxel_volume::draw_bounding_box()
{
#ifdef DC_CLIENT
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

    for(i=0; i<12; i++) 
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

inline Voxel* Voxel_volume::get(unsigned int x, unsigned int y, unsigned int z) 
{   return &voxel[x+(y << index1)+(z << index12)]; }

inline unsigned int Voxel_volume::get_as_int(unsigned int x, unsigned int y, unsigned int z) 
{ return voxel[x+(y << index1)+(z << index12)].color; }

/*
Tests whether a voxel is occupied, for AO
*/
inline unsigned int Voxel_volume::_test_occludes_safe(int x, int y, int z) 
{ 
    if( x < 0 || y < 0 || z < 0 ) return 0;
    if( x >= xdim || y >= ydim || z >= zdim ) return 0;
    unsigned int index= x+(y << index1)+(z << index12);
    if(voxel[index].color == 0) return 0;
    return 1;
}
inline void Voxel_volume::_set(unsigned int x, unsigned int y, unsigned int z, Voxel* v)
{ voxel[x+(y << index1)+(z << index12)] = *v; }
inline void Voxel_volume::_set(unsigned int x, unsigned int y, unsigned int z, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{ Voxel* v = &voxel[x+(y << index1)+(z << index12)]; v->r = r;v->g = g;v->b = b;v->a = a; }


static void destroy_object_voxel(int id, int type, int part, int voxel[3])
{
    //#ifdef DC_CLIENT
    void* obj;
    Voxel_volume* vv;
    switch (type)
    {
        case OBJ_TYPE_AGENT:
            obj = STATE::agent_list->get(id);
            if (obj == NULL || ((Agent_state*)obj)->vox == NULL) return;
            vv = ((Agent_state*)obj)->vox->get_part(part);
            if (vv == NULL) return;
            vv->set(voxel[0], voxel[1], voxel[2],0,0,0,0);
            break;
        case OBJ_TYPE_SLIME:
            obj = STATE::slime_list->get(id);
            if (obj == NULL || ((Monsters::Slime*)obj)->vox == NULL) return;
            vv = ((Monsters::Slime*)obj)->vox->get_part(part);
            if (vv == NULL) return;
            vv->set(voxel[0], voxel[1], voxel[2],0,0,0,0);
            break;
        //case OBJ_TYPE_SPAWNER:
            //obj = STATE::spawner_list->get(id);
            //if (obj == NULL || ((Spawner*)obj)->vox == NULL) return;
            //vv = ((Spawner*)obj)->vox->get_part(part);
            //if (vv == NULL) return;
            //vv->set(voxel[0], voxel[1], voxel[2],0,0,0,0);
            //break;
        default: break;
    }
    //#endif
}

void destroy_object_voxel(int id, int type, int part, int voxel[3], int radius)
{
    int mx = voxel[0];
    int my = voxel[1];
    int mz = voxel[2];
    
    for (int i=0; i<radius; i++)
        for (int j=0; j<radius; j++)
            for (int k=0; k<radius; k++)
            {
                voxel[0] = mx + i;
                voxel[1] = my + j;
                voxel[2] = mz + k;
                destroy_object_voxel(id, type, part, voxel);
                voxel[0] = mx - i;
                destroy_object_voxel(id, type, part, voxel);
                voxel[1] = my - j;
                destroy_object_voxel(id, type, part, voxel);
                voxel[1] = my + j;
                voxel[2] = mz - k;
                destroy_object_voxel(id, type, part, voxel);
                voxel[0] = mx + i;
                voxel[1] = my - j;
                destroy_object_voxel(id, type, part, voxel);
                voxel[0] = mx - i;
                destroy_object_voxel(id, type, part, voxel);
            }
}

