#pragma once

#include <c_lib/voxel/voxel_body.hpp>

#include <c_lib/template/object_list.hpp>
#include <c_lib/physics/vector.hpp>
#include <c_lib/voxel/voxel_volume.hpp>
#include <c_lib/voxel/object_vox.hpp>

namespace Monsters {

extern VoxBody slime_vox_dat;

const int SLIME_PART_NUM = 1;
enum SLIME_BODY_PARTS {
    SLIME_PART_BODY
};

class Slime;    // forward decl

//class Slime_vox {
    //public:
        //void right(Vector *f, float theta);
        //void forward(Vector *f, float theta);
        //void look(Vector *f, float theta, float phi);

        //Slime* a;

        //Voxel_volume vv[SLIME_PART_NUM];

        //void init_parts();
        //void update();
        //void set_draw(bool draw);

        //Slime_vox(Slime* a);
        //~Slime_vox();
//};

//class Slime_vox: public Object_vox<Slime, SLIME_PART_NUM, slime_vox_dat>
//{};

class Slime_vox: public Object_vox<Slime, SLIME_PART_NUM>
{
    public:
    explicit Slime_vox(Slime* a, VoxBody* vox_dat) : Object_vox(a, vox_dat) {}
};

class Slime {
    public:
    int id;
    float x,y,z;
    float vx,vy,vz;
    float theta,phi;
    
    Slime_vox* vox;

    Slime();
    Slime(int id);
    Slime(float x, float y, float z, float vx, float vy, float vz);
    Slime(int id, float x, float y, float z, float vx, float vy, float vz);
};


#define SLIME_MAX 1024
class Slime_list: public Object_list<Slime,SLIME_MAX>
{
    private:
        const char* name() { return "Slime"; }
    public:
        void draw();
};

#ifdef DC_CLIENT
void test();
#endif

}
