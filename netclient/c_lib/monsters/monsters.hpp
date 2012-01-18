#pragma once

#include <c_lib/voxel/voxel_body.hpp>

#include <c_lib/template/object_list.hpp>
#include <c_lib/physics/vector.hpp>
#include <c_lib/voxel/voxel_volume.hpp>
#include <c_lib/voxel/object_vox.hpp>

#include <common/enum_types.hpp>

namespace Monsters {

extern VoxBody slime_vox_dat;

const int SLIME_PART_NUM = 1;
enum SLIME_BODY_PARTS {
    SLIME_PART_BODY
};

class Slime;    // forward decl

class Slime_vox: public Object_vox<Slime, SLIME_PART_NUM>
{
    public:
    Slime_vox(Slime* a, VoxBody* vox_dat) : Object_vox<Slime, SLIME_PART_NUM>(a, vox_dat) {}
};

class Slime {
    public:
    int id;
    float x,y,z;
    float vx,vy,vz;
    double theta,phi;
    
    Slime_vox* vox;

    Object_types type;

    void tick();

    Slime();
    Slime(int id);
    Slime(float x, float y, float z, float vx, float vy, float vz);
    Slime(int id, float x, float y, float z, float vx, float vy, float vz);
    ~Slime();
};

#define SLIME_MAX 1024
class Slime_list: public Object_list<Slime,SLIME_MAX>
{
    private:
        const char* name() { return "Slime"; }
    public:
        void draw();
        void update();
        void tick();
};

//#ifdef DC_CLIENT
void test();
//#endif

}
