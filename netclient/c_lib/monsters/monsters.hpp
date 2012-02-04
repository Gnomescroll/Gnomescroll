#pragma once

#include <c_lib/voxel/voxel_dat.hpp>

#include <c_lib/template/object_list.hpp>
#include <c_lib/physics/vector.hpp>
#include <c_lib/voxel/voxel_volume.hpp>
#include <c_lib/voxel/voxel_model.hpp>

#include <common/enum_types.hpp>

namespace Monsters {

extern VoxDat slime_vox_dat;

enum SLIME_BODY_PARTS {
    SLIME_PART_BODY
};

const int SLIME_HEALTH = 50;

class Slime;    // forward decl

class Slime {
    public:
    int id;
    float x,y,z;
    float vx,vy,vz;
    float theta,phi;
    int health;
    
    Voxel_model* vox;

    Object_types type;

    #ifdef DC_SERVER
    bool changed;
    int tick_num;
    #endif

    void tick();

    void set_state(float x, float y, float z, float vx, float vy, float vz);
    void set_state(float x, float y, float z, float vx, float vy, float vz, float theta, float phi);
    void set_angles(float theta, float phi);

    void init_vox();

    int take_damage(int dmg);

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

        void send_to_client(int client_id);
};

void test(int n);

}
